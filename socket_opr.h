#ifndef _SOCKET_OPR_H
#define _SOCKET_OPR_H
#include <string>


class CSocketOpr
{
public:
    static CSocketOpr *instance();
    std::string localHostIp();
private:
    long getLocalHostIp();
private:
    static CSocketOpr _instance;
};

#endif

