#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <linux/if.h>

#include "socket_opr.h"
#include "trace_base.h"


CSocketOpr CSocketOpr::_instance;


CSocketOpr *CSocketOpr::instance()
{
	return &_instance;
}


long CSocketOpr::getLocalHostIp()
{
    int  MAXINTERFACES=16;
    long ip;
    int fd, intrface;
    struct ifreq buf[MAXINTERFACES]; ///if.h
    struct ifconf ifc; ///if.h
    ip = -1;
    if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0) //socket.h
    {
        ifc.ifc_len = sizeof buf;
        ifc.ifc_buf = (caddr_t) buf;
        if (!ioctl (fd, SIOCGIFCONF, (char *) &ifc)) //ioctl.h
        {
            intrface = ifc.ifc_len / sizeof (struct ifreq);
            while (intrface-- > 0)
            {
                if (!(ioctl (fd, SIOCGIFADDR, (char *) &buf[intrface])))
                {
                    ip=inet_addr( inet_ntoa( ((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr) );//types
                    break;
                }

            }
        }
        close (fd);
    }
    return ip;
}

std::string CSocketOpr::localHostIp()
{
    union ipu{
    long ip;
    unsigned char ipchar[4];
    };
    
    union ipu ipChar;
    ipChar.ip = getLocalHostIp();
    
    char ipStr[64];
    CBase::snprintf(ipStr, sizeof(ipStr), "%u.%u.%u.%u", ipChar.ipchar[0],ipChar.ipchar[1],ipChar.ipchar[2],ipChar.ipchar[3]);
    return ipStr;
}



