// Simple caps plug-in support example on Win32, should fit most users
//
// www.caps-project.org

#include "stdafx.h"

#include "ComType.h"
#include "CapsAPI.h"

#include "windows.h"
#include "CapsPlug.h"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>

static int caps_flags = DI_LOCK_DENVAR | DI_LOCK_DENNOISE | DI_LOCK_NOISE | DI_LOCK_UPDATEFD | DI_LOCK_TYPE | DI_LOCK_OVLBIT | DI_LOCK_TRKBIT;

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

UDWORD	swapDWord(UDWORD buffer) {
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

    d0 = 0x4489;
    while (d0 == readLEWord(*pBuffer))
        *pBuffer += 2;
}

int sectorFindSync(byte **pBuffer, byte *pBufferEnd) {
    const	UWORD syncValue = 0x4489;

    UDWORD	d0 = 0, d7 = 0;
    bool	found = false;

    for (;;) {
        d0 = swapDWord(readLEDWord(*pBuffer));

        for (d7 = 0; d7 != 0x10; ++d7) {

            if (syncValue == (UWORD)d0) {
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

static void mfmcopy(UINT16 *mfm, UINT8 *data, int len)
{
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

int main(int argc, char* argv[])
{
	CapsInit("CAPSImg.dll");

	int img=CapsAddImage();

	// lock this image into the container
	auto ret = CapsLockImage(img, argv[1]);
    if (ret != imgeOk) {
        return -1;
    }

	// get some information about the image
	CapsImageInfo ci;
	CapsGetImageInfo(&ci, img);
    auto MaxTracks = (ci.maxcylinder - ci.mincylinder + 1) * (ci.maxhead - ci.minhead + 1);

	CapsLoadImage(img, caps_flags);

    CapsTrackInfoT2 ti;

    std::vector<std::string> TrackBuffers;
    TrackBuffers.resize(MaxTracks);

    // Track 2
    for (size_t Track = 2; Track < MaxTracks; ++Track) {
        std::string Full;

        ti.type = ctitNoise;

        size_t Side = 1;
        if ((Track & 1))
            Side = 0;

        auto Data = CapsLockTrack((PCAPSTRACKINFO)&ti, img, Track / 2, Side, caps_flags);

        Full.resize(ti.tracklen / 8);
        mfmcopy((UINT16*) Full.data(), ti.trackbuf, ti.tracklen);

        byte* Ptr = (byte*) Full.data();
        byte* PtrEnd = (byte*) Full.data() + Full.size();

        auto c = sectorFindSync(&Ptr, Ptr + Full.size());
        trackbuffer_shift_bits(&Ptr, c, (PtrEnd - Ptr) / 2 - 8);

        byte* PtrOdd = Ptr + 0x180c;
        if (PtrOdd > PtrEnd)
            continue;

        SDWORD Checksum = getlw((UDWORD*) Ptr, (UDWORD*) PtrOdd);
        Ptr += 4; PtrOdd += 4;
        // Check for Sensible Software marker
        if (swapDWord(Checksum) != 'SOS6') {
            continue;
        }
        
        Checksum = -swapDWord(Checksum);
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

        //if (Checksum)
        //    return -1;

        // Track CRC Matches
        byte* TrackData = (byte*) TrackDecodedBuffer.data();
        tool_EndianSwap(TrackData, TrackDecodedBuffer.size() * 4);
        TrackBuffers[Track] = std::string((char*)TrackData, TrackDecodedBuffer.size() * 4);
    }

	CapsUnlockAllTracks(img);
	CapsUnlockImage(img);
	CapsRemImage(img);
	CapsExit();

    byte* TrackPtr = (byte*)TrackBuffers[2].data();

    UDWORD FileCount = readDWord(TrackPtr + 0x18);


    // Move to first file entry
    TrackPtr += 0x20;
    for (UDWORD Count = 0; Count < FileCount; ++Count) {
        std::string Filename((char*)TrackPtr);
        UDWORD Filesize = readDWord(TrackPtr + 0x1C);

        UBYTE FileTrack = *(TrackPtr + 0x1A);
        UBYTE FileBlock = *(TrackPtr + 0x1B);

        std::string FileData;

        SDWORD FileRemaining = Filesize;
        while (FileRemaining > 0) {
            byte* FileTrackPtr = (byte*)TrackBuffers[FileTrack].data();
            FileTrackPtr += (FileBlock * 512);

            FileData.append((char*)FileTrackPtr, 510);
            FileRemaining -= 510;
            FileTrackPtr += 510;

            FileTrack = *FileTrackPtr++;
            FileBlock = *FileTrackPtr++;
        }

        FileData.resize(Filesize);
        std::ofstream outfile;
        outfile.open(Filename, std::ios_base::out | std::ios::binary);
        outfile.write( (char*)FileData.data(), FileData.size());
        outfile.close();

        // Next File
        TrackPtr += 0x20;
    }

	return 0;
}
