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
#include "trace_worke.h"



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
	char str[4096];
	CTraceWorkManager::instance()->InsertHex(psBuf, nBufLen, str, sizeof(str));

	CLogDataInf dataInf;
	dataInf.putInf((char *)"insertTrace");
	dataInf.putInf(CBase::pthread_self());
	dataInf.putInf(line);
	dataInf.putInf(file_name);
	dataInf.putInf("");
	dataInf.putInf(0);
	dataInf.putInf(str);

	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	CTraceWorkManager::instance()->send(packet, packetLen);
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
	CBase::pthread_mutex_init(&socketMutex, NULL);
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
		CBase::pthread_mutex_lock(&socketMutex);
		rc=::send(m_socketClient,szText,cnt,0);
		CBase::pthread_mutex_unlock(&socketMutex);
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


void CTraceWorkManager::InsertHex(char *psBuf, int nBufLen, char *str, int strLen)
{
	/* save log msg in file */
	CBase::snprintf(str, strLen, "hex%s:len=%4d\n", __FUNCTION__, nBufLen);

	/* save log msg in file */
	int j = 0;
	char sLine[100], sTemp[12];
	for	(int i=0; i<nBufLen; i++)
	{
		
		/* initialize a new line */
		if (j==0)
		{
			memset (sLine,	' ', sizeof(sLine));
			CBase::snprintf (sTemp,	sizeof(sTemp), "%04d:", i );
			memcpy (sLine, sTemp, 5);
			CBase::snprintf (sTemp, sizeof(sTemp), ":%04d", i+15 );
			memcpy (sLine+72, sTemp, 5);
		}

		/* output psBuf value in hex */
		CBase::snprintf(sTemp, sizeof(sTemp), "%02X ", (unsigned	char)psBuf[i]);
		memcpy( &sLine[j*3+5+(j>7)], sTemp, 3);

		/* output psBuf in ascii */
		if ( isprint (psBuf[i]))
		{
			sLine[j+55+(j>7)]=psBuf[i];
		}
		else
		{
			sLine[j+55+(j>7)]='.';
		}
		j++;

		/* output the line to file */
		if (j==16)
		{
			sLine[77]=0;
			CBase::snprintf(str+strlen(str), sizeof(str)-strlen(str), "%s\n", sLine);
			j=0;
		}
	}

	/* last line */
	if (j)
	{
		sLine[77]=0;
		CBase::snprintf(str+strlen(str), sizeof(str)-strlen(str), "%s\n",	sLine);
	}

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

char *CBase::strcpy(char *dest, const char *src)
{
#ifdef WIN32
	if (dest != NULL)
	{
		strcpy_s(dest, strlen(src)+1, src);
	}
	return dest;
#else
	return ::strcpy(dest, src);
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

int CBase::pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
#ifdef WIN32
	InitializeCriticalSection(mutex);
	return 0;
#else
	return ::pthread_mutex_init(mutex, attr);
#endif

}

int CBase::pthread_mutex_lock(pthread_mutex_t *mutex)
{
#ifdef WIN32
	EnterCriticalSection(mutex);
	return 0;
#else
	return ::pthread_mutex_lock(mutex);
#endif

}

int CBase::pthread_mutex_unlock(pthread_mutex_t *mutex)
{
#ifdef WIN32
	LeaveCriticalSection(mutex);
	return 0;
#else
	return ::pthread_mutex_unlock(mutex);
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
			CLen[CLenSize-1] = i + 1;
			break;
		}
	}
	return ;
}

void CLogDataInf::C2ILen(char *CLen, int CLenSize, int &iLen)
{
	iLen = 0;
	int x256 = 1;
	for (int i=0; i<CLen[CLenSize-1]; ++i)
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


