
#ifdef WIN32
extern "C" __declspec(dllimport) SDWORD __cdecl CAPSInit();
extern "C" __declspec(dllimport) SDWORD __cdecl CAPSExit();
extern "C" __declspec(dllimport) SDWORD __cdecl CAPSAddImage();
extern "C" __declspec(dllimport) SDWORD __cdecl CAPSRemImage(SDWORD id);
extern "C" __declspec(dllimport) SDWORD __cdecl CAPSLockImage(SDWORD id, PCHAR name);
extern "C" __declspec(dllimport) SDWORD __cdecl CAPSLockImageMemory(SDWORD id, PUBYTE buffer, UDWORD length, UDWORD flag);
extern "C" __declspec(dllimport) SDWORD __cdecl CAPSUnlockImage(SDWORD id);
extern "C" __declspec(dllimport) SDWORD __cdecl CAPSLoadImage(SDWORD id, UDWORD flag);
extern "C" __declspec(dllimport) SDWORD __cdecl CAPSGetImageInfo(PCAPSIMAGEINFO pi, SDWORD id);
extern "C" __declspec(dllimport) SDWORD __cdecl CAPSLockTrack(PVOID ptrackinfo, SDWORD id, UDWORD cylinder, UDWORD head, UDWORD flag);
extern "C" __declspec(dllimport) SDWORD __cdecl CAPSUnlockTrack(SDWORD id, UDWORD cylinder, UDWORD head);
extern "C" __declspec(dllimport) SDWORD __cdecl CAPSUnlockAllTracks(SDWORD id);
extern "C" __declspec(dllimport) PCHAR  __cdecl CAPSGetPlatformName(UDWORD pid);
extern "C" __declspec(dllimport) SDWORD __cdecl CAPSGetVersionInfo(PVOID pversioninfo, UDWORD flag);
extern "C" __declspec(dllimport) UDWORD __cdecl CAPSFdcGetInfo(SDWORD iid, PCAPSFDC pc, SDWORD ext);
extern "C" __declspec(dllimport) SDWORD __cdecl CAPSFdcInit(PCAPSFDC pc);
extern "C" __declspec(dllimport) void   __cdecl CAPSFdcReset(PCAPSFDC pc);
extern "C" __declspec(dllimport) void   __cdecl CAPSFdcEmulate(PCAPSFDC pc, UDWORD cyclecnt);
extern "C" __declspec(dllimport) UDWORD __cdecl CAPSFdcRead(PCAPSFDC pc, UDWORD address);
extern "C" __declspec(dllimport) void   __cdecl CAPSFdcWrite(PCAPSFDC pc, UDWORD address, UDWORD data);
extern "C" __declspec(dllimport) SDWORD __cdecl CAPSFdcInvalidateTrack(PCAPSFDC pc, SDWORD drive);
extern "C" __declspec(dllimport) SDWORD __cdecl CAPSFormatDataToMFM(PVOID pformattrack, UDWORD flag);
extern "C" __declspec(dllimport) SDWORD __cdecl CAPSGetInfo(PVOID pinfo, SDWORD id, UDWORD cylinder, UDWORD head, UDWORD inftype, UDWORD infid);
extern "C" __declspec(dllimport) SDWORD __cdecl CAPSSetRevolution(SDWORD id, UDWORD value);
extern "C" __declspec(dllimport) SDWORD __cdecl CAPSGetImageType(PCHAR name);
extern "C" __declspec(dllimport) SDWORD __cdecl CAPSGetImageTypeMemory(PUBYTE buffer, UDWORD length);
extern "C" __declspec(dllimport) SDWORD __cdecl CAPSGetDebugRequest();
#else
	extern "C" {
	SDWORD CAPSInit(void);
	SDWORD CAPSExit(void);
	SDWORD CAPSAddImage(void);
	SDWORD CAPSRemImage(SDWORD id);
	SDWORD CAPSLockImage(SDWORD id, char *name);
	SDWORD CAPSLockImageMemory(SDWORD id, PUBYTE *buffer, UDWORD length, UDWORD flag);
	SDWORD CAPSUnlockImage(SDWORD id);
	SDWORD CAPSLoadImage(SDWORD id, UDWORD flag);
	SDWORD CAPSGetImageInfo(struct CapsImageInfo *pi, SDWORD id);
	SDWORD CAPSLockTrack(void *ptrackinfo, SDWORD id, UDWORD cylinder, UDWORD head, UDWORD flag);
	SDWORD CAPSUnlockTrack(SDWORD id, UDWORD cylinder, UDWORD head);
	SDWORD CAPSUnlockAllTracks(SDWORD id);
	char *CAPSGetPlatformName(UDWORD pid);
	SDWORD CAPSGetVersionInfo(void *pversioninfo, UDWORD flag);
	SDWORD CAPSGetInfo(void *pinfo, SDWORD id, UDWORD cylinder, UDWORD head, UDWORD inftype, UDWORD infid);
	}
#endif