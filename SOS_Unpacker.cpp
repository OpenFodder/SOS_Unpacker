// Simple caps plug-in support example on Win32, should fit most users
//
// www.caps-project.org

#include "stdafx.h"
#include "windows.h"

#include "ComType.h"
#include "CapsAPI.h"

#include "CapsPlug.h"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>

const char gPathSeperator = (_WIN32 ? '\\' : '/');

static int caps_flags = DI_LOCK_DENVAR | DI_LOCK_DENNOISE | DI_LOCK_NOISE | DI_LOCK_UPDATEFD | DI_LOCK_TYPE | DI_LOCK_OVLBIT | DI_LOCK_TRKBIT;
const UWORD gTrackSyncMarker = 0x4489;

UWORD readLEWord(const void *buffer) {
    const UWORD *wordBytes = (const UWORD *)buffer;
    return *wordBytes;
}

UDWORD	readLEDWord(const void *buffer) {
    const UDWORD *dwordBytes = (const UDWORD *)buffer;
    return *dwordBytes;
}

UWORD readWord(const void *buffer) {
    const byte *byteNext = (const byte *)buffer;
    return (byteNext[0] << 8) + byteNext[1];
}

UDWORD readDWord(const void *buffer) {
    const byte *dwordNext = (const byte *)buffer;
    return (dwordNext[0] << 24) + (dwordNext[1] << 16) + (dwordNext[2] << 8) + (dwordNext[3]);
}

UDWORD swapDWord(UDWORD buffer) {
    const UWORD *DW = (UWORD*)&buffer;
    return (DW[0] << 16) + (DW[1]);
}

void trackbuffer_shift_bits(byte **pBuffer, int d0, int d1) {
    d0 -= 1;
    if (d0 >= 0) {
        byte *buffer = *pBuffer;
        buffer += d1; buffer += d1;
        d1 -= 2;

        do {
            UWORD *a3 = (UWORD*)buffer;
            int	 d2 = d1;
            bool extend = false;

            --a3;
            if (*a3 & 0x8000)
                extend = true;
            *a3 <<= 1;

            do {
                bool extend2 = false;
                --a3;
                if (*a3 & 0x8000)
                    extend2 = true;
                *a3 <<= 1;

                if (extend)
                    *a3 |= 0x01;
                extend = extend2;
            } while (--d2 >= 0);

        } while (--d0 >= 0);
    }

    // Skip past SYNC marker
    while (readLEWord(*pBuffer) == gTrackSyncMarker)
        *pBuffer += 2;
}

int sectorFindSync(byte **pBuffer, byte *pBufferEnd) {

    UDWORD	d0 = 0, d7 = 0;
    bool	found = false;

    for (;;) {
        d0 = swapDWord(readLEDWord(*pBuffer));

        for (d7 = 0; d7 != 0x10; ++d7) {

            if (gTrackSyncMarker == (UWORD)d0) {
                found = true;
                break;
            }

            d0 >>= 1;
        }

        if (found)
            break;

        *pBuffer += 2;
        if (*pBuffer >= pBufferEnd)
            return -1;
    }

    // 990E
    d0 = 0x10;
    d0 -= d7;
    if (d0 == 0x10) {
        *pBuffer += 2;
        d0 = 0;
    }

    return d0;
}

void mfmcopy(UINT16 *mfm, UINT8 *data, int len) {
    int memlen = (len + 7) / 8;
    for (int i = 0; i < memlen; i += 2) {
        if (i + 1 < memlen)
            *mfm++ = (data[i] << 8) + data[i + 1];
        else
            *mfm++ = (data[i] << 8);
    }
}

UDWORD getlw(UDWORD *pEven, UDWORD* pOdd) {
    UDWORD D0 = *pEven;
    UDWORD D1 = *pOdd;

    D0 &= 0x55555555;
    D1 &= 0x55555555;

    D1 += D1;
    D0 += D1;

    return D0;
}

void tool_EndianSwap(UINT8 *pBuffer, size_t pSize) {
    UINT8 *pDest = pBuffer;

    pSize /= 2;

    while (pSize--) {
        UINT8 al = *pBuffer++;
        UINT8 ah = *pBuffer++;

        *pDest++ = ah;
        *pDest++ = al;
    }
}

std::vector<std::string> GetSOSTracks(const std::string& pFilename) {
    CapsImageInfo CapsImageInfo;

    int CapsID = CapsAddImage();

    // lock this image into the container
    auto ret = CapsLockImage(CapsID, (PCHAR) pFilename.c_str());
    if (ret != imgeOk) {
        CapsRemImage(CapsID);
        return {};
    }

    // get some information about the image
    CapsGetImageInfo(&CapsImageInfo, CapsID);
    CapsLoadImage(CapsID, caps_flags);

    auto MaxTracks = (CapsImageInfo.maxcylinder - CapsImageInfo.mincylinder + 1) *  (CapsImageInfo.maxhead - CapsImageInfo.minhead + 1);

    std::vector<std::string> TrackBuffers;
    TrackBuffers.resize(MaxTracks);

    // Track 0 is standard Amiga DOS
    // Track 1 is unformatted
    // Track 2 is the first SOS track
    for (UDWORD Track = 2; Track < MaxTracks; ++Track) {

        CapsTrackInfoT2 TrackInfo;
        TrackInfo.type = ctitNoise;

        // SOS has the sides inverted
        UDWORD Head = 1;
        if ((Track & 1))
            Head = 0;

        auto Data = CapsLockTrack((PCAPSTRACKINFO)&TrackInfo, CapsID, Track / 2, Head, caps_flags);
        std::string Full;
        Full.resize(TrackInfo.tracklen / 8);
        mfmcopy((UINT16*)Full.data(), TrackInfo.trackbuf, TrackInfo.tracklen);

        byte* Ptr = (byte*)Full.data();
        byte* PtrEnd = (byte*)Full.data() + Full.size();

        // Find the track start marker
        auto SkipBits = sectorFindSync(&Ptr, Ptr + Full.size());
        if (SkipBits == -1) {
            std::cout << "Skipping Track " << Track << " (SYNC not found)\n";
            continue;
        }
        trackbuffer_shift_bits(&Ptr, SkipBits, (int)((PtrEnd - Ptr) / 2) - 8);

        byte* PtrOdd = Ptr + 0x180c;
        if (PtrOdd > PtrEnd) {
            std::cout << "Skipping Track " << Track << "\n";
            continue;
        }
        SDWORD Checksum = getlw((UDWORD*)Ptr, (UDWORD*)PtrOdd);
        Ptr += 4; PtrOdd += 4;

        // Check for Sensible Software marker
        if (swapDWord(Checksum) != 'SOS6') {
            std::cout << "Skipping Track " << Track << " (Invalid SOS6 Marker)\n";
            continue;
        }
        Checksum = -((SDWORD)swapDWord(Checksum));

        SDWORD data = getlw((UDWORD*)Ptr, (UDWORD*)PtrOdd);
        Ptr += 4; PtrOdd += 4;
        Checksum += swapDWord(data);

        data = getlw((UDWORD*)Ptr, (UDWORD*)PtrOdd);
        Ptr += 4; PtrOdd += 4;
        Checksum -= swapDWord(data);

        std::vector<UDWORD> TrackDecodedBuffer;
        for (int count = 0x1800 / 4 - 1; count >= 0; --count) {
            UDWORD NewData = getlw((UDWORD*)Ptr, (UDWORD*)PtrOdd);
            Ptr += 4; PtrOdd += 4;
            Checksum -= swapDWord(NewData);
            TrackDecodedBuffer.push_back(NewData);
        }

        if (Checksum) {
            std::cout << "Track " << Track << " has invalid checksum.\n";
        }

        // Track CRC Matches
        byte* TrackData = (byte*)TrackDecodedBuffer.data();
        tool_EndianSwap(TrackData, TrackDecodedBuffer.size() * 4);
        TrackBuffers[Track] = std::string((char*)TrackData, TrackDecodedBuffer.size() * 4);
    }

    CapsUnlockAllTracks(CapsID);
    CapsUnlockImage(CapsID);
    CapsRemImage(CapsID);

    return TrackBuffers;
}
#include<iomanip>
void ExtractFiles(const std::vector<std::string> pTrackBuffers, const std::string& pTargetFolder) {

    // Track3 is the directory listing
    byte* TrackPtr = (byte*)pTrackBuffers[2].data();
    UDWORD FileCount = readDWord(TrackPtr + 0x18);

    // Move to first file entry
    TrackPtr += 0x20;
    for (UDWORD Count = 0; Count < FileCount; ++Count) {
        std::string Filename((char*)TrackPtr);
        UDWORD Filesize = readDWord(TrackPtr + 0x1C);

        UBYTE FileTrack = *(TrackPtr + 0x1A);
        UBYTE FileBlock = *(TrackPtr + 0x1B);

        std::string FileData;

        std::cout << std::setw(15) << Filename << "  Track: ";
        std::cout << std::setw(4) << ((size_t)FileTrack) << "  Block: ";
        std::cout << std::setw(3) << ((size_t)FileBlock) << "  Size: ";
        std::cout << std::setw(6) << ((size_t)Filesize) << " bytes\n";

        SDWORD FileRemaining = Filesize;
        while (FileRemaining > 0) {
            byte* FileTrackPtr = (byte*)pTrackBuffers[FileTrack].data();
            byte *FileTrackPtrEnd = (byte*)pTrackBuffers[FileTrack].data() + pTrackBuffers[FileTrack].size();
            FileTrackPtr += (FileBlock * 512);

            if (FileTrackPtr > FileTrackPtrEnd || FileTrackPtr + 510 >= FileTrackPtrEnd) {
                std::cout << "  Track: " << ((size_t)FileTrack) << ": Read past end of available track data.\n\n";
                break;
            }
            FileData.append((char*)FileTrackPtr, 510);
            FileRemaining -= 510;
            FileTrackPtr += 510;

            FileTrack = *FileTrackPtr++;
            FileBlock = *FileTrackPtr++;
        }

        // Truncate to actual file size
        FileData.resize(Filesize);

        std::ofstream outfile;
        outfile.open(pTargetFolder + "//" + Filename, std::ios_base::out | std::ios::binary);
        outfile.write((char*)FileData.data(), FileData.size());
        outfile.close();

        // Next File
        TrackPtr += 0x20;
    }
}

int main(int argc, char* argv[]) {
    std::cout << "SOS Unpacker\n";
    std::cout << "https://github.com/OpenFodder\n\n";

    if (CapsInit("CAPSImg.dll")) {
        std::cout << "CAPSImg.dll not found\n";
    } else {

        std::cout << "Finding .raw files\n";
        auto Files = local_DirectoryList(".", ".raw");

        if (!Files.size())
            std::cout << "No .raw files found\n";

        system("mkdir out");

        //local_DirectoryList
        for (auto& File : Files) {
            std::cout << "\nLoading " << File << "\n";
            auto Tracks = GetSOSTracks(File);

            if (!Tracks.size()) {
                std::cout << "No Tracks found in file: " << File << "\n";
                continue;
            }


            std::cout << "Extracting to .\\out\\...\n";
            ExtractFiles(Tracks, "out");
        }
    }

    std::cout << "Press enter to finish\n";
    std::cin.get();

    CapsExit();
	return 0;
}
