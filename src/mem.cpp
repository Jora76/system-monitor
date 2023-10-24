#include "header.h"
#include <sys/statvfs.h>

string result;
struct statvfs fsInfo;

void getMemState(float &memState, float &memTotal, float &swapState, float &swapTotal)
{
    float memTaken;
    float swapTaken;
    ifstream memData("/proc/meminfo");
    if (!memData)
    {
        std::cout << "File not found." << std::endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        while (getline(memData, result))
        {
            string test;
            if (result.find("MemTotal:") != string::npos)
            {
                test = result.substr(17, 7);
                memTotal = stof(test);
            }
            else if (result.find("MemAvailable:") != string::npos)
            {
                test = result.substr(17, 7);
                memTaken = stof(test);
            }
            else if (result.find("SwapTotal:") != string::npos)
            {
                test = result.substr(17, 7);
                swapTotal = stof(test);
            }
            else if (result.find("SwapFree:") != string::npos)
            {
                test = result.substr(17, 7);
                swapTaken = stof(test);
            }
        }
        memData.close();
        memState = (memTotal - memTaken) / 1000000.f;
        swapState = (swapTotal - swapTaken) / 1000000.f;
    }
}

void getHardDiskSpace(float &totalSpace, float &freeSpace)
{
    if (statvfs("/", &fsInfo) != -1)
    {
        totalSpace = fsInfo.f_frsize * fsInfo.f_blocks / 1000000000;
        freeSpace = fsInfo.f_frsize * fsInfo.f_bavail / 1000000000;
    } 
}