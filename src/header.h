#ifndef header_H
#define header_H

#include "../imgui/lib/imgui.h"
#include "../imgui/lib/backend/imgui_impl_sdl.h"
#include "../imgui/lib/backend/imgui_impl_opengl3.h"
#include <stdio.h>
#include <dirent.h>
#include <vector>
#include <iostream>
#include <cmath>
// lib to read from file
#include <fstream>
// for the name of the computer and the logged in user
#include <unistd.h>
#include <limits.h>
// this is for us to get the cpu information
// mostly in unix system
// not sure if it will work in windows
#include <cpuid.h>
// this is for the memory usage and other memory visualization
// for linux gotta find a way for windows
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
// for time and date
#include <ctime>
// ifconfig ip addresses
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>

using namespace std;

struct CPUStats
{
    long long int user;
    long long int nice;
    long long int system;
    long long int idle;
    long long int iowait;
    long long int irq;
    long long int softirq;
    long long int steal;
    long long int guest;
    long long int guestNice;
};

struct RX
{
    int bytes;
    int packets;
    int errs;
    int drop;
    int fifo;
    int frame;
    int compressed;
    int multicast;
};

struct TX
{
    int bytes;
    int packets;
    int errs;
    int drop;
    int fifo;
    int colls;
    int carrier;
    int compressed;
};

struct IP4
{
    char *name;
    char addressBuffer[INET_ADDRSTRLEN];
    TX net_out;
    RX net_in;
};

struct Networks
{
    vector<IP4> ip4s;
};

struct Process 
{
    int PID;
    string name;
    string state;
    float CPU;
    float MEM = 0.f;
};

// system stats
string CPUinfo();
const char *getOsName();
void getProcesses(vector<Process> &tabProcess);
void getCPUPercentage(float &CPUPercentage, int fps);
void getFanRPM(float &rpm);
void getFanInfos(string &level);
void getDeviceThermalState(float &thermal_state);

// memory and processes
void getMemState(float &memState, float &memTotal, float &swapState, float &swapTotal);
void getHardDiskSpace(float &totalSpace, float &freeSpace);

// network
Networks getIps();
void setNet_attr(IP4 &net, int attr_code, string data);
void getNetworks(IP4 &net);

#endif
