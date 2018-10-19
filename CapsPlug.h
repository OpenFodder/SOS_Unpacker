
// library function pointers
struct CapsProc {
	LPCSTR name;
	FARPROC proc;
};

typedef SDWORD (__cdecl *CAPSHOOKN)(...);
typedef PCHAR  (__cdecl *CAPSHOOKS)(...);

SDWORD CapsInit(LPCTSTR lib);
SDWORD CapsExit();
SDWORD CapsAddImage();
SDWORD CapsRemImage(SDWORD id);
SDWORD CapsLockImage(SDWORD id, PCHAR name);
SDWORD CapsLockImageMemory(SDWORD id, PUBYTE buffer, UDWORD length, UDWORD flag);
SDWORD CapsUnlockImage(SDWORD id);
SDWORD CapsLoadImage(SDWORD id, UDWORD flag);
SDWORD CapsGetImageInfo(PCAPSIMAGEINFO pi, SDWORD id);
SDWORD CapsLockTrack(PCAPSTRACKINFO pi, SDWORD id, UDWORD cylinder, UDWORD head, UDWORD flag);
SDWORD CapsUnlockTrack(SDWORD id, UDWORD cylinder, UDWORD head);
SDWORD CapsUnlockAllTracks(SDWORD id);
PCHAR  CapsGetPlatformName(UDWORD pid);

SDWORD CAPSGetInfo(PVOID pinfo, SDWORD id, UDWORD cylinder, UDWORD head, UDWORD inftype, UDWORD infid);
SDWORD CAPSSetRevolution(SDWORD id, UDWORD value);
SDWORD CAPSGetImageType(PCHAR name);
SDWORD CAPSGetImageTypeMemory(PUBYTE buffer, UDWORD length);
SDWORD CAPSGetDebugRequest();
