#include "header.h"

// /proc/net/dev pour le reste

Networks getIps()
{
    Networks ip_addr;
    ifaddrs *network;
    getifaddrs(&network);
    for (auto addr = network; addr != NULL; addr = addr->ifa_next)
    {
        if (!ip_addr.ip4s.empty())
        {
            if (addr->ifa_name == ip_addr.ip4s[0].name)
                break;
        }

        if (addr->ifa_addr != NULL && addr->ifa_addr->sa_family == AF_INET)
        {
            IP4 temp;
            inet_ntop(AF_INET, network->ifa_addr, temp.addressBuffer, INET_ADDRSTRLEN);
            temp.name = addr->ifa_name;
            getNetworks(temp);
            ip_addr.ip4s.push_back(temp);
        }
    }
    freeifaddrs(network);
    return ip_addr;
}

//test

void getNetworks(IP4 &net)
{
    ifstream netFile("/proc/net/dev");
    if (!netFile)
    {
        std::cout << "File not found." << std::endl;
        exit(EXIT_FAILURE);
    }

    string result;
    while (getline(netFile, result))
    {
        char path[100];
        snprintf(path, sizeof(path + 20), "%s:", net.name);
        if (result.find(path) != string::npos)
        {
            int count = 0;
            string temp;
            for (auto i = 0; i < result.substr(8).length(); i++)
            {
                if (result.substr(8)[i] == ' ' && !temp.empty())
                {
                    setNet_attr(net, count, temp);
                    count++;
                    temp.clear();
                }
                else if (result.substr(8)[i] >= 48 && result.substr(8)[i] <= 57)
                {
                    temp += result.substr(8)[i];
                }
            }
            break;
            // récupérer toutes les données qui sont en bordel
        }
    }
}

void setNet_attr(IP4 &net, int attr_code, string data)
{
    switch (attr_code)
    {
    case 0:
        net.net_in.bytes = atoi(data.c_str());
        break;
    case 1:
        net.net_in.packets = atoi(data.c_str());
        break;
    case 2:
        net.net_in.errs = atoi(data.c_str());
        break;
    case 3:
        net.net_in.drop = atoi(data.c_str());
        break;
    case 4:
        net.net_in.fifo = atoi(data.c_str());
        break;
    case 5:
        net.net_in.frame = atoi(data.c_str());
        break;
    case 6:
        net.net_in.compressed = atoi(data.c_str());
        break;
    case 7:
        net.net_in.multicast = atoi(data.c_str());
        break;
    case 8:
        net.net_out.bytes = atoi(data.c_str());
        break;
    case 9:
        net.net_out.packets = atoi(data.c_str());
        break;
    case 10:
        net.net_out.errs = atoi(data.c_str());
        break;
    case 11:
        net.net_out.drop = atoi(data.c_str());
        break;
    case 12:
        net.net_out.fifo = atoi(data.c_str());
        break;
    case 13:
        net.net_out.colls = atoi(data.c_str());
        break;
    case 14:
        net.net_out.carrier = atoi(data.c_str());
        break;
    case 15:
        net.net_out.compressed = atoi(data.c_str());
        break;
    }
}