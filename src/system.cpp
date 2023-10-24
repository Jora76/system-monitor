#include "header.h"
#include <cstdlib>
#include <chrono>
#include <thread>

float uSys = 0.f;
// get cpu id and information, you can use `proc/cpuinfo`

string CPUinfo()
{
    char CPUBrandString[0x40];
    unsigned int CPUInfo[4] = {0, 0, 0, 0};

    // unix system
    // for windoes maybe we must add the following
    // __cpuid(regs, 0);
    // regs is the array of 4 positions
    __cpuid(0x80000000, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
    unsigned int nExIds = CPUInfo[0];

    memset(CPUBrandString, 0, sizeof(CPUBrandString));

    for (unsigned int i = 0x80000000; i <= nExIds; ++i)
    {
        __cpuid(i, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);

        if (i == 0x80000002)
            memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
        else if (i == 0x80000003)
            memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
        else if (i == 0x80000004)
            memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
    }
    string str(CPUBrandString);
    return str;
}

// getOsName, this will get the OS of the current computer
const char *getOsName()
{
#ifdef _WIN32
    return "Windows 32-bit";
#elif _WIN64
    return "Windows 64-bit";
#elif __APPLE__ || __MACH__
    return "Mac OSX";
#elif __linux__
    return "Linux";
#elif __FreeBSD__
    return "FreeBSD";
#elif __unix || __unix__
    return "Unix";
#else
    return "Other";
#endif
}

void getProcesses(vector<Process> &tabProcess)
{
    tabProcess.clear();
    Process cur_proc;
    DIR *repo = opendir("/proc");
    string result;
    char *path = new(char);
    char *end = " KB";
    while (auto reader = readdir(repo))
    {
        if (atoi(reader->d_name) > 0)
        {
            snprintf(path, 100, "/proc/%s/status", reader->d_name);
            ifstream p_file(path);
            while (getline(p_file, result))
            {
                if (result.find("Name:") != string::npos)
                {
                    cur_proc.name = result.substr(6);
                }
                else if (result.find("State:") != string::npos)
                {
                    cur_proc.state = result.substr(9);
                }
                else if (result.find("Pid:") != string::npos && result.find("TracerPid:") == string::npos && result.find("PPid:") == string::npos)
                {
                    cur_proc.PID = atoi(result.substr(5).c_str());
                }
                else if (result.find("VmRSS:") != string::npos)
                {
                    cur_proc.MEM = strtof(result.substr(9).c_str(), &end);
                }
            }
            p_file.close();

            snprintf(path, 100, "/proc/%s/stat", reader->d_name);
            ifstream CPU_file(path);
            getline(CPU_file, result);
            int space_counter = 0;
            string user;
            string system;
            for (auto i = 0; i < result.length(); i++)
            {
                if (result[i] == ' ' && space_counter != 13)
                {
                    space_counter++;
                }
                else if (space_counter == 13)
                {
                    for (auto j = i; j < result.length(); j++)
                    {
                        if (result[j] == ' ')
                        {
                            i = j;
                            break;
                        }
                        user += result[j];
                    }
                    space_counter++;
                }
                else if (space_counter == 14)
                {
                    for (auto j = i; j < result.length(); j++)
                    {
                        if (result[j] == ' ')
                        {
                            i = j;
                            break;
                        }
                        system += result[j];
                    }
                    break;
                }
            }
            if (uSys <= 0.f)
                cur_proc.CPU = 0.f;
            else
                cur_proc.CPU = 100 * (stof(system) + stof(user)) / uSys;
            tabProcess.push_back(cur_proc);
            system.clear();
            user.clear();
        }
    }
    closedir(repo);
}

void getCPUPercentage(float &CPUPercentage, int fps)
{
    std::ifstream CPUData("/proc/stat");
    string result;
    float CPU_PrevUseData[9];
    float CPU_UseData[9];
    if (!CPUData)
    {
        std::cout << "File not found." << std::endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        getline(CPUData, result);
        CPUData.close();
        string temp;
        int counter = 0;
        for (long unsigned int i = 5; i < result.length(); i++)
        {
            if (result[i] == ' ')
            {
                CPU_PrevUseData[counter] = stof(temp);
                counter++;
                temp = "";
            }
            else
            {
                temp += result[i];
            }
        }
        counter = 0;
        this_thread::sleep_for(chrono::milliseconds(1000));

        CPUData.open("/proc/stat");
        getline(CPUData, result);
        CPUData.close();
        for (long unsigned int i = 5; i < result.length(); i++)
        {
            if (result[i] == ' ')
            {
                CPU_UseData[counter] = stof(temp) - CPU_PrevUseData[counter];
                counter++;
                temp = "";
            }
            else
            {
                temp += result[i];
            }
        }

        CPUPercentage = 100 * ((CPU_UseData[0] + CPU_UseData[1] + CPU_UseData[2]) / ((CPU_UseData[0] + CPU_UseData[1] + CPU_UseData[2] + CPU_UseData[3])));
        if (CPUPercentage > 100)
        {
            CPUPercentage = 100;
        }
        uSys = CPU_PrevUseData[0] + CPU_PrevUseData[2];
    }
}

void getFanRPM(float &rpm)
{
    system("sensors > faninfo.txt");
    std::ifstream fanData("faninfo.txt");
    if (!fanData)
    {
        std::cout << "File not found." << std::endl;
        exit(EXIT_FAILURE);
    }
    string result;
    while (getline(fanData, result))
    {
        if (result.find("cpu_fan:") != string::npos)
            break;
    }
    string temp;
    for (long unsigned int i = 13; i < result.length(); i++)
    {
        temp += result[i];
    }
    rpm = stof(temp);
    remove("faninfo.txt");
}

void getDeviceThermalState(float &thermal_state)
{
    cout << "j't'écoute tkt pas mon pote" << flush;
    ofstream file("therminfo.txt");
    if(!file)
        cout << "g pas reussi gros" << endl;
    file.close();
    system("sensors > therminfo.txt");
    std::ifstream fanData("therminfo.txt");
    if (!fanData)
    {
        std::cout << "File not found." << std::endl;
        exit(EXIT_FAILURE);
    }
    string result;
    while (getline(fanData, result))
    {
        if (result.find("temp1:") != string::npos)
            break;
    }
    string temp;
    for (long unsigned int i = 15; i < result.length(); i++)
    {
        if (!atoi(&result[i]) && result[i] != '0')
        {
            break;
        }
        temp += result[i];
    }
    thermal_state = stof(temp);
    remove("therminfo.txt");
}

void getFanInfos(string &level)
{
    ifstream fanLevel("/sys/class/hwmon/hwmon5/power/control");
    if (!fanLevel)
    {
        std::cout << "File not found." << std::endl;
        exit(EXIT_FAILURE);
    }
    // getline(fanLevel, temp);
    fanLevel >> level;
    fanLevel.close();
}
