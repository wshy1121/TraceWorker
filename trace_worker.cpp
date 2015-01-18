//#include "stdafx.h"
#ifdef WIN32

#include<ws2tcpip.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <string>
typedef int SOCKET;
#endif
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "trace_worke.h"

static SOCKET m_socketClient;
const char *dataFormat = "{\"opr\" : \"%s\", \"threadId\" : %d, \"line\" : %d, \"fileName\" : \"%s\", \"funcName\" : \"%s\", \"displayLevel\" : %d, \"content\" : \"%s\"}";

class CBase
{
public:
#ifdef WIN32
	typedef unsigned long pthread_t;
	typedef int  pthread_attr_t;
#else
	typedef ::pthread_t  pthread_t;
	typedef ::pthread_attr_t  pthread_attr_t;
#endif
public:
	static int snprintf(char *str, size_t size, const char *format, ...);
	static pthread_t pthread_self(void);
	static int vsnprintf(char *str, size_t size, const char *format, va_list ap);
	static int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
	static int usleep(int micro_second);
};


class CTraceWorkManager
{
public:
	static CTraceWorkManager *instance();
	bool startServer(const char *ip);
	int send(char *szText,int len);
private:
	CTraceWorkManager();
};


#define INF_SIZE 16 
class CLogDataInf
{
public:
	CLogDataInf();
	~CLogDataInf();
	void putInf(char *strdata);
	void putInf(const char *strdata);
	void putInf(int intData);
	int packet(char *&packet);
	int unPacket(char *infs[]);
	int unPacket(char *packet, char *infs[]);
private:
	void I2CLen(int iLen, char *CLen, int CLenSize);
	void C2ILen(char *CLen, int CLenSize, int &iLen);
private:
	int m_lenSize;	
	char *m_infs[INF_SIZE];
	char *m_packet;
	int m_packetLen;
	int m_infsNum;
};

	
CCandy::CCandy(int line, char *file_name, char *func_name, int display_level)
{
	CLogDataInf dataInf;
	dataInf.putInf((char *)"createCandy");
	dataInf.putInf(CBase::pthread_self());
	dataInf.putInf(line);
	dataInf.putInf(file_name);
	dataInf.putInf(func_name);
	dataInf.putInf(display_level);
	dataInf.putInf("NULL");
	
	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	
	CTraceWorkManager::instance()->send(packet, packetLen);
	return ;
}

CCandy::~CCandy()
{
	CLogDataInf dataInf;
	dataInf.putInf((char *)"destroyCandy");
	dataInf.putInf(CBase::pthread_self());
	dataInf.putInf(0);
	dataInf.putInf("");
	dataInf.putInf("");
	dataInf.putInf(0);
	dataInf.putInf("");
	
	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	CTraceWorkManager::instance()->send(packet, packetLen);
	return ;
}



void CBugKiller::InsertTrace(int line, char *file_name, const char* fmt, ...)
{
	char content[4096];
	va_list ap;
	va_start(ap,fmt);
	CBase::vsnprintf(content,sizeof(content), fmt, ap);
	va_end(ap);

	CLogDataInf dataInf;
	dataInf.putInf((char *)"insertTrace");
	dataInf.putInf(CBase::pthread_self());
	dataInf.putInf(line);
	dataInf.putInf(file_name);
	dataInf.putInf("");
	dataInf.putInf(0);
	dataInf.putInf(content);
	
	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	CTraceWorkManager::instance()->send(packet, packetLen);
	return ;
}

void CBugKiller::InsertHex(int line, char *file_name, char *psBuf, int nBufLen)
{
#if 0
	RECV_DATA *pRecvData = CTimeCalcInfManager::instance()->createRecvData(nBufLen);
	TimeCalcInf *pCalcInf = &pRecvData->calcInf;

	pCalcInf->m_opr = TimeCalcInf::e_insertHex;
	pCalcInf->m_threadId = base::pthread_self();
	pCalcInf->m_line = line;
	pCalcInf->m_fileName = file_name;
	memcpy(pCalcInf->m_pContent, psBuf, nBufLen);

	CTimeCalcInfManager::instance()->pushRecvData(pRecvData);
#endif	
	return ;
}

void CBugKiller::DispAll()
{
	CLogDataInf dataInf;
	dataInf.putInf((char *)"dispAll");
	dataInf.putInf(CBase::pthread_self());
	dataInf.putInf(0);
	dataInf.putInf("");
	dataInf.putInf("");
	dataInf.putInf(0);
	dataInf.putInf("backtrace");

	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	CTraceWorkManager::instance()->send(packet, packetLen);
}


void CBugKiller::InsertTag(int line, char *file_name, const char* fmt, ...)
{
	char content[4096];
	va_list ap;
	va_start(ap,fmt);
	CBase::vsnprintf(content,sizeof(content), fmt, ap);
	va_end(ap);

	CLogDataInf dataInf;
	dataInf.putInf((char *)"insertTag");
	dataInf.putInf(CBase::pthread_self());
	dataInf.putInf(line);
	dataInf.putInf(file_name);
	dataInf.putInf("");
	dataInf.putInf(0);
	dataInf.putInf(content);

	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	CTraceWorkManager::instance()->send(packet, packetLen);
	return ;
}

void CBugKiller::printfMemInfMap()
{
	CLogDataInf dataInf;
	dataInf.putInf((char *)"printfMemInfMap");
	dataInf.putInf(CBase::pthread_self());
	dataInf.putInf(0);
	dataInf.putInf("");
	dataInf.putInf("");
	dataInf.putInf(0);
	dataInf.putInf("");

	
	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	CTraceWorkManager::instance()->send(packet, packetLen);
}

void CBugKiller::printfStackInfo(int line, char *file_name)
{
	InsertTrace(line, file_name, "backTrace youself");
}

bool CBugKiller::startServer(const char *sip)
{
	return CTraceWorkManager::instance()->startServer(sip);
}


CTraceWorkManager::CTraceWorkManager()
{
#ifdef WIN32	
		WSADATA wsa={0};
		WSAStartup(MAKEWORD(2,2),&wsa);
#endif
}

CTraceWorkManager *CTraceWorkManager::instance()
{
	static CTraceWorkManager _instance;
	return &_instance;
}

bool CTraceWorkManager::startServer(const char *sip)
{
	m_socketClient = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == m_socketClient)
	{
		return false;
	}

	struct sockaddr_in addr;
	addr.sin_family 		= AF_INET;
	addr.sin_addr.s_addr	= inet_addr(sip);
	addr.sin_port			= htons(8889);
	
	int ret = ::connect(m_socketClient, (struct sockaddr *) & addr, sizeof(sockaddr_in));
	if(-1 == ret)
	{
		return false;
	}
	return true;
}

int CTraceWorkManager::send(char *szText,int len)
{
	int cnt;
	int rc;
	cnt=len;
	while(cnt>0)
	{
		rc=::send(m_socketClient,szText,cnt,0);
		if(rc==-1)
		{
			return -1;
		}
		if(rc==0)
		{
			return len-cnt;
		}
		szText+=rc;
		cnt-=rc;
	}
	return len;
}



int CBase::snprintf(char *str, size_t size, const char *format, ...)
{
	va_list ap;
	va_start(ap,format);
#ifdef WIN32
	int ret = vsnprintf_s(str, size, _TRUNCATE, format, ap);
#else
	int ret = ::vsnprintf(str, size, format, ap);
#endif
	va_end(ap);
	return ret;
}

CBase::pthread_t CBase::pthread_self(void)
{
#ifdef WIN32
		return GetCurrentThreadId();
#else
		return ::pthread_self();
#endif

}

int CBase::vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
#ifdef WIN32
	return vsnprintf_s(str, size, _TRUNCATE, format, ap);
#else
	return ::vsnprintf(str, size, format, ap);
#endif
}

int CBase::pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg)
{
#ifdef WIN32
	void (*routine)(void *) = (void (*) (void *))start_routine;
	return (int)_beginthread(routine, 0, arg);
#else
	return ::pthread_create(thread, attr, start_routine, arg);
#endif
}

int CBase::usleep(int micro_second)
{
#ifdef WIN32
		if (micro_second < 1000)
		{
			micro_second = 1000;
		}
		
		Sleep(micro_second / 1000);
		return 0;
#else
		return ::usleep(micro_second);
#endif

}


CLogDataInf::CLogDataInf() : m_lenSize(4), m_packet(NULL),m_infsNum(0)
{
	m_packet = (char *)malloc(32*1024);
	m_packetLen = m_lenSize;
}


CLogDataInf::~CLogDataInf()
{
	if (m_packet)
	{
		free(m_packet);
		m_packet = NULL;
	}
}

void CLogDataInf::I2CLen(int iLen, char *CLen, int CLenSize)
{
	memset(CLen, 0, CLenSize);
	for (int i=0; i<CLenSize; ++i)
	{
		CLen[i] = iLen % 256;
		iLen = iLen >> 8;
		if (iLen == 0)
		{
			break;
		}
	}
	return ;
}

void CLogDataInf::C2ILen(char *CLen, int CLenSize, int &iLen)
{
	iLen = 0;
	int x256 = 1;
	for (int i=0; i<CLenSize; ++i)
	{
		iLen += (unsigned char)(CLen[i])*x256;
		x256 = x256 << 8;
	}
	return ;
}



void CLogDataInf::putInf(char *strdata)
{
	int dataLen = strlen(strdata) + 1;

	I2CLen(dataLen + m_lenSize, m_packet+m_packetLen, m_lenSize);
	m_packetLen += m_lenSize;

	memcpy(m_packet+m_packetLen, strdata, dataLen);
	m_packetLen += dataLen;

}

void CLogDataInf::putInf(const char *strdata)
{
	putInf((char *)strdata);
}


void CLogDataInf::putInf(int intData)
{
	char strData[64];
	snprintf(strData, sizeof(strData), "%d", intData);
	putInf(strData);
}

int CLogDataInf::unPacket(char *infs[])
{
	return unPacket(m_packet, infs);
}
int CLogDataInf::unPacket(char *packet, char *infs[])
{
	int totalLen = 0;
	char *inf = NULL;
	int infLen = 0;
	
	C2ILen(packet, m_lenSize, totalLen);

	int infsNum = 0;
	int i=m_lenSize;
	for (; i<totalLen-m_lenSize; )
	{
		C2ILen(packet+i,m_lenSize,infLen);
		inf = packet + i + m_lenSize;
		i += infLen;

		infs[infsNum++] = inf;
	}
	
	C2ILen(packet+i,m_lenSize,infLen);
	if (infLen != totalLen)
	{
		infs[0] = NULL;
		return 0;
	}
	infs[infsNum] = NULL;
	return totalLen;
}

int CLogDataInf::packet(char *&packet)
{
	m_packetLen += m_lenSize;

	I2CLen(m_packetLen, m_packet, m_lenSize);
	I2CLen(m_packetLen, m_packet+m_packetLen-m_lenSize, m_lenSize);
	
	packet = m_packet;
	return m_packetLen;
}


