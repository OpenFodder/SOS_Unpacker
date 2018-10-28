
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <stdint.h>

#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <vector>

#ifdef WIN32
#include "Windows.h"
#include <io.h>
#include <direct.h>
#include "capsimg/Compatibility/dirent.h"
#else
#include <dirent.h>
#include <stddef.h>			// offsetof
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>			// localtime
#define MAX_PATH ( 260 )
#define _access access
#ifndef __MINGW32__
#define _mkdir(x) mkdir(x,0)
#else
#define _mkdir(x) mkdir(x)
#endif
#ifndef __cdecl
#define __cdecl
#endif
#define _lrotl(x,n) (((x) << (n)) | ((x) >> (sizeof(x)*8-(n))))
#define _lrotr(x,n) (((x) >> (n)) | ((x) << (sizeof(x)*8-(n))))
typedef const char *LPCSTR;
typedef const char *LPCTSTR;
#define d_namlen d_reclen
#define __assume(cond) do { if (!(cond)) __builtin_unreachable(); } while (0)

typedef struct _SYSTEMTIME {
    uint16_t wYear;
    uint16_t wMonth;
    uint16_t wDayOfWeek;
    uint16_t wDay;
    uint16_t wHour;
    uint16_t wMinute;
    uint16_t wSecond;
    uint16_t wMilliseconds;
} SYSTEMTIME, *LPSYSTEMTIME;
extern "C" void GetLocalTime(LPSYSTEMTIME lpSystemTime);
#endif

std::vector<std::string> local_DirectoryList(const std::string& pPath, const std::string& pExtension);

#define INTEL
#define MAX_FILENAMELEN (MAX_PATH*2)

// external definitions
#include "capsimg/Core/CommonTypes.h"

// Core components
#include "capsimg/Core/BaseFile.h"
#include "capsimg/Core/DiskFile.h"
#include "capsimg/Core/MemoryFile.h"
#include "capsimg/Core/CRC.h"
#include "capsimg/Core/BitBuffer.h"

// IPF library public definitions
#include "capsimg/LibIPF/CapsLibAll.h"

// CODECs
#include "capsimg/Codec/DiskEncoding.h"
#include "capsimg/Codec/CapsDefinitions.h"
#include "capsimg/Codec/CTRawCodec.h"

// file support
#include "capsimg/CAPSImg/CapsFile.h"
#include "capsimg/CAPSImg/DiskImage.h"
#include "capsimg/CAPSImg/CapsLoader.h"
#include "capsimg/CAPSImg/CapsImageStd.h"
#include "capsimg/CAPSImg/CapsImage.h"
#include "capsimg/CAPSImg/StreamImage.h"
#include "capsimg/CAPSImg/StreamCueImage.h"
#include "capsimg/CAPSImg/DiskImageFactory.h"

// Device access
#include "capsimg/Device/C2Comm.h"

// system
#include "capsimg/CAPSImg/CapsCore.h"
#include "capsimg/CAPSImg/CapsFDCEmulator.h"
#include "capsimg/CAPSImg/CapsFormatMFM.h"
