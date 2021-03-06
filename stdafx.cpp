// stdafx.cpp : source file that includes just the standard includes
//	CAPSImg.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#ifdef WIN32
#include <direct.h>

std::vector<std::string> local_DirectoryList(const std::string& pPath, const std::string& pExtension) {
    WIN32_FIND_DATA fdata;
    HANDLE dhandle;
    std::vector<std::string> results;

    char path[2000];
    _getcwd(path, 2000);

    // Build the file path
    std::string finalPath;

    if (pPath.size())
        finalPath += pPath;

    finalPath += "/*";
    finalPath += pExtension;

    if ((dhandle = FindFirstFile(finalPath.c_str(), &fdata)) == INVALID_HANDLE_VALUE) {
        return results;
    }
    
    results.push_back(std::string(fdata.cFileName));

    while (1) {
        if (FindNextFile(dhandle, &fdata)) {
            results.push_back(std::string(fdata.cFileName));
        }
        else {
            if (GetLastError() == ERROR_NO_MORE_FILES) {
                break;
            }
            else {
                FindClose(dhandle);
                return results;
            }
        }
    }

    FindClose(dhandle);

    return results;
}

#else
#include <dirent.h>
#include <algorithm>
std::string findType;

int file_select(const struct dirent *entry) {
    std::string name = entry->d_name;

    std::transform(name.begin(), name.end(), name.begin(), ::toupper);

    if (name.find(findType) == std::string::npos)
        return false;

    return true;
}

std::vector<std::string> local_DirectoryList(const std::string& pPath, const std::string& pExtension) {
    struct dirent		**directFiles;
    std::vector<std::string>		  results;

    // Build the file path

    std::string finalPath;

    finalPath += pPath;
    finalPath += "/";

    findType = pExtension;

    transform(findType.begin(), findType.end(), findType.begin(), ::toupper);

    int count = scandir(finalPath.c_str(), (dirent***)&directFiles, file_select, 0);

    for (int i = 0; i < count; ++i) {

        results.push_back(std::string(directFiles[i]->d_name));
    }

    transform(findType.begin(), findType.end(), findType.begin(), ::tolower);

    count = scandir(finalPath.c_str(), (dirent***)&directFiles, file_select, 0);

    for (int i = 0; i < count; ++i) {

        results.push_back(std::string(directFiles[i]->d_name));
    }

    return results;
}


void GetLocalTime(LPSYSTEMTIME lpSystemTime)
{
    time_t t = time(NULL);
    struct tm *tp = localtime(&t);

    lpSystemTime->wYear = tp->tm_year + 1900;
    lpSystemTime->wMonth = tp->tm_mon + 1;
    lpSystemTime->wDayOfWeek = tp->tm_wday;
    lpSystemTime->wDay = tp->tm_mday;
    lpSystemTime->wHour = tp->tm_hour;
    lpSystemTime->wMinute = tp->tm_min;
    lpSystemTime->wSecond = tp->tm_sec;
    // we don't have milliseconds in struct tm
    lpSystemTime->wMilliseconds = 0;
}

#endif
