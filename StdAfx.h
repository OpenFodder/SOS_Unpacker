
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <stdint.h>

#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <map>

#include "CapsLibAll.h"

#ifdef WIN32
#include "Windows.h"
#endif

std::vector<std::string> local_DirectoryList(const std::string& pPath, const std::string& pExtension);
