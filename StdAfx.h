// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//


#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxtempl.h>

#include "ComType.h"
#include "CapsAPI.h"
#include "CapsLib.h"

#include <vector>
std::vector<std::string> local_DirectoryList(const std::string& pPath, const std::string& pExtension);
