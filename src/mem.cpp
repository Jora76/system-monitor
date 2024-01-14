#include "header.h"
#include <sys/statvfs.h>

string result;
struct statvfs fsInfo;

// Get virtual and physic memory usage
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
            string temp;
            if (result.find("MemTotal:") != string::npos)
            {
                temp = result.substr(17, 7);
                memTotal = stof(temp);
            }
            else if (result.find("MemAvailable:") != string::npos)
            {
                temp = result.substr(17, 7);
                memTaken = stof(temp);
            }
            else if (result.find("SwapTotal:") != string::npos)
            {
                temp = result.substr(17, 7);
                swapTotal = stof(temp);
            }
            else if (result.find("SwapFree:") != string::npos)
            {
                temp = result.substr(17, 7);
                swapTaken = stof(temp);
            }
        }
        memData.close();
        memState = (memTotal - memTaken) / 1000000.f;
        swapState = (swapTotal - swapTaken) / 1000000.f;
    }
}

// Get hard disk space taken
void getHardDiskSpace(float &totalSpace, float &freeSpace)
{
    if (statvfs("/", &fsInfo) != -1)
    {
        totalSpace = fsInfo.f_frsize * fsInfo.f_blocks / 1000000000;
        freeSpace = fsInfo.f_frsize * fsInfo.f_bavail / 1000000000;
    } 
}