#ifndef CAPSLIB_H
#define CAPSLIB_H

#undef LIB_USER
#ifdef CAPS_USER
#define LIB_USER
#endif

SDWORD __cdecl CAPSInit();
SDWORD __cdecl CAPSExit();
SDWORD __cdecl CAPSAddImage();
SDWORD __cdecl CAPSRemImage(SDWORD id);
SDWORD __cdecl CAPSLockImage(SDWORD id, PCHAR name);
SDWORD __cdecl CAPSLockImageMemory(SDWORD id, PUBYTE buffer, UDWORD length, UDWORD flag);
SDWORD __cdecl CAPSUnlockImage(SDWORD id);
SDWORD __cdecl CAPSLoadImage(SDWORD id, UDWORD flag);
SDWORD __cdecl CAPSGetImageInfo(PCAPSIMAGEINFO pi, SDWORD id);
SDWORD __cdecl CAPSLockTrack(PVOID ptrackinfo, SDWORD id, UDWORD cylinder, UDWORD head, UDWORD flag);
SDWORD __cdecl CAPSUnlockTrack(SDWORD id, UDWORD cylinder, UDWORD head);
SDWORD __cdecl CAPSUnlockAllTracks(SDWORD id);
PCHAR  __cdecl CAPSGetPlatformName(UDWORD pid);
SDWORD __cdecl CAPSGetVersionInfo(PVOID pversioninfo, UDWORD flag);
UDWORD __cdecl CAPSFdcGetInfo(SDWORD iid, PCAPSFDC pc, SDWORD ext);
SDWORD __cdecl CAPSFdcInit(PCAPSFDC pc);
void   __cdecl CAPSFdcReset(PCAPSFDC pc);
void   __cdecl CAPSFdcEmulate(PCAPSFDC pc, UDWORD cyclecnt);
UDWORD __cdecl CAPSFdcRead(PCAPSFDC pc, UDWORD address);
void   __cdecl CAPSFdcWrite(PCAPSFDC pc, UDWORD address, UDWORD data);
SDWORD __cdecl CAPSFdcInvalidateTrack(PCAPSFDC pc, SDWORD drive);
SDWORD __cdecl CAPSFormatDataToMFM(PVOID pformattrack, UDWORD flag);
SDWORD __cdecl CAPSGetInfo(PVOID pinfo, SDWORD id, UDWORD cylinder, UDWORD head, UDWORD inftype, UDWORD infid);
SDWORD __cdecl CAPSSetRevolution(SDWORD id, UDWORD value);
SDWORD __cdecl CAPSGetImageType(PCHAR name);
SDWORD __cdecl CAPSGetImageTypeMemory(PUBYTE buffer, UDWORD length);
SDWORD __cdecl CAPSGetDebugRequest();

#endif
