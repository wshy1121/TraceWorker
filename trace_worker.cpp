#include "stdafx.h"
#ifdef WIN32
#pragma comment(lib,"ws2_32.lib")
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <string>
typedef int SOCKET;
#endif
#include "trace_worker.h"



CCandy::CCandy(int line, char *file_name, char *func_name, int display_level)
{
	char sTid[16];
	char sLine[8];
	char sLevel[8];
	CBase::snprintf(sTid, sizeof(sTid), "%d", CBase::pthread_self());
	CBase::snprintf(sLine, sizeof(sLine), "%d", line);
	CBase::snprintf(sLevel, sizeof(sLevel), "%d", display_level);
	
	CLogDataInf dataInf;
	dataInf.putInf((char *)"createCandy");
	dataInf.putInf(sTid);
	dataInf.putInf(sLine);
	dataInf.putInf(file_name);
	dataInf.putInf(func_name);
	dataInf.putInf(sLevel);
	dataInf.putInf("NULL");
	
	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	CTraceWorkManager::instance()->send(packet, packetLen);
	
	CTraceWorkManager::instance()->receiveInfData(&dataInf);
	return ;
}

CCandy::~CCandy()
{
	char sTid[16];
	CBase::snprintf(sTid, sizeof(sTid), "%d", CBase::pthread_self());
	CLogDataInf dataInf;
	dataInf.putInf((char *)"destroyCandy");
	dataInf.putInf(sTid);
	dataInf.putInf("0");
	dataInf.putInf("");
	dataInf.putInf("");
	dataInf.putInf("0");
	dataInf.putInf("");
	
	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	CTraceWorkManager::instance()->send(packet, packetLen);
	CTraceWorkManager::instance()->receiveInfData(&dataInf);
	return ;
}



void CBugKiller::InsertTrace(int line, char *file_name, const char* fmt, ...)
{
	char content[4096];
	va_list ap;
	va_start(ap,fmt);
	CBase::vsnprintf(content,sizeof(content), fmt, ap);
	va_end(ap);

	char sTid[16];
	char sLine[8];
	CBase::snprintf(sTid, sizeof(sTid), "%d", CBase::pthread_self());
	CBase::snprintf(sLine, sizeof(sLine), "%d", line);
	
	CLogDataInf dataInf;
	dataInf.putInf((char *)"insertTrace");
	dataInf.putInf(sTid);
	dataInf.putInf(sLine);
	dataInf.putInf(file_name);
	dataInf.putInf("");
	dataInf.putInf("0");
	dataInf.putInf(content);
	
	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	CTraceWorkManager::instance()->send(packet, packetLen);
	CTraceWorkManager::instance()->receiveInfData(&dataInf);
	return ;
}

void CBugKiller::InsertHex(int line, char *file_name, char *psBuf, int nBufLen)
{
	char str[4096];
	CTraceWorkManager::instance()->InsertHex(psBuf, nBufLen, str, sizeof(str));

	char sTid[16];
	char sLine[8];
	CBase::snprintf(sTid, sizeof(sTid), "%d", CBase::pthread_self());
	CBase::snprintf(sLine, sizeof(sLine), "%d", line);

	CLogDataInf dataInf;
	dataInf.putInf((char *)"insertTrace");
	dataInf.putInf(sTid);
	dataInf.putInf(sLine);
	dataInf.putInf(file_name);
	dataInf.putInf("");
	dataInf.putInf("0");
	dataInf.putInf(str);

	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	CTraceWorkManager::instance()->send(packet, packetLen);
	CTraceWorkManager::instance()->receiveInfData(&dataInf);	
	return ;
}

void CBugKiller::DispAll()
{
	char sTid[16];
	CBase::snprintf(sTid, sizeof(sTid), "%d", CBase::pthread_self());
	CLogDataInf dataInf;
	dataInf.putInf((char *)"dispAll");
	dataInf.putInf(sTid);
	dataInf.putInf("0");
	dataInf.putInf("");
	dataInf.putInf("");
	dataInf.putInf("0");
	dataInf.putInf("backtrace");

	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	CTraceWorkManager::instance()->send(packet, packetLen);
	CTraceWorkManager::instance()->receiveInfData(&dataInf);
	return ;	
}


void CBugKiller::InsertTag(int line, char *file_name, const char* fmt, ...)
{
	char content[4096];
	va_list ap;
	va_start(ap,fmt);
	CBase::vsnprintf(content,sizeof(content), fmt, ap);
	va_end(ap);

	char sTid[16];
	char sLine[8];
	CBase::snprintf(sTid, sizeof(sTid), "%d", CBase::pthread_self());
	CBase::snprintf(sLine, sizeof(sLine), "%d", line);

	CLogDataInf dataInf;
	dataInf.putInf((char *)"insertTag");
	dataInf.putInf(sTid);
	dataInf.putInf(sLine);
	dataInf.putInf(file_name);
	dataInf.putInf("");
	dataInf.putInf("0");
	dataInf.putInf(content);

	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	CTraceWorkManager::instance()->send(packet, packetLen);
	CTraceWorkManager::instance()->receiveInfData(&dataInf);	
	return ;
}

void CBugKiller::printfMemInfMap()
{
	char sTid[16];
	CBase::snprintf(sTid, sizeof(sTid), "%d", CBase::pthread_self());
	CLogDataInf dataInf;
	dataInf.putInf((char *)"printfMemInfMap");
	dataInf.putInf(sTid);
	dataInf.putInf("0");
	dataInf.putInf("");
	dataInf.putInf("");
	dataInf.putInf("0");
	dataInf.putInf("");

	
	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	CTraceWorkManager::instance()->send(packet, packetLen);
	CTraceWorkManager::instance()->receiveInfData(&dataInf);
	return ;	
}

void CBugKiller::openFile(const char *fileName)
{
	char sTid[16];
	CBase::snprintf(sTid, sizeof(sTid), "%d", CBase::pthread_self());
	CLogDataInf dataInf;
	dataInf.putInf((char *)"openFile");
	dataInf.putInf(sTid);
	dataInf.putInf("0");
	dataInf.putInf("");
	dataInf.putInf("");
	dataInf.putInf("0");
	dataInf.putInf(fileName);

	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	CTraceWorkManager::instance()->send(packet, packetLen);
	CTraceWorkManager::instance()->receiveInfData(&dataInf);
	return ;	
}


void CBugKiller::printfStackInfo(int line, char *file_name)
{
	std::string backTrace;
	CTraceWorkManager::instance()->getBackTrace(backTrace);
	InsertTrace(line, file_name, backTrace.c_str());
}

bool CBugKiller::startServer(const char *sip)
{
	bool bRet = CTraceWorkManager::instance()->startServer(sip);
	if (!bRet)
	{
		return bRet;
	}

	openFile("Debug11.cpp");
	return bRet;
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
	m_socketClient = connect(sip, 8889);
	if(-1 == m_socketClient)
	{
		return false;
	}

	return true;
}

SOCKET CTraceWorkManager::connect(const char *sip, int port)
{
	SOCKET socketClient = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == socketClient)
	{
		return -1;
	}

	struct sockaddr_in addr;
	addr.sin_family 		= AF_INET;
	addr.sin_addr.s_addr	= inet_addr(sip);
	addr.sin_port			= htons(port);
	
	int ret = ::connect(socketClient, (struct sockaddr *) & addr, sizeof(sockaddr_in));
	if(-1 == ret)
	{
		return -1;
	}
	m_sip = sip;
	m_port = port;
	return socketClient;
}

int CTraceWorkManager::disConnect(SOCKET socket)
{
	return CBase::close(socket);
}

int CTraceWorkManager::reConnect()
{
	if (m_socketClient > 0)
	{
		disConnect(m_socketClient);
		m_socketClient = -1;
	}
	m_socketClient = connect(m_sip, m_port);
	printf("reConnect m_socketClient  %d\n", m_socketClient);
	return m_socketClient;
}

bool CTraceWorkManager::receiveInfData(CLogDataInf *pDataInf)
{
	const int ClenSize = 4;
	char CLen[ClenSize];
	if (receive(CLen, ClenSize) <= 0)
	{
		return false;
	}
	int iLen = 0;		
	pDataInf->C2ILen(CLen,ClenSize,iLen);

	char *packet = new char[iLen];
	memcpy(packet, CLen, ClenSize);
	if (receive(packet+ClenSize, iLen-ClenSize) <= 0)
	{
		return false;
	}
	pDataInf->unPacket(packet);

	return true;
}

int CTraceWorkManager::receive(char *szText,int iLen)
{
	int recvBufLen = 0;
	int totalRecvLen = 0;
	while (1)
	{
		recvBufLen = ::recv(m_socketClient, szText+totalRecvLen, iLen-totalRecvLen, 0);
		if (recvBufLen <= 0)

		{
			return -1;
		}
		totalRecvLen += recvBufLen;
		if (totalRecvLen == iLen)
		{
			break;
		}
	}
	return iLen;
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
	int curStrLen = strlen(str);

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
			CBase::snprintf(str+strlen(str), strLen-strlen(str), "%s\n", sLine);
			j=0;
			curStrLen += strlen(sLine) + 1;
			if (curStrLen > (strLen-1))
			{
				curStrLen = (strLen-1);
			}
		}
	}

	/* last line */
	if (j)
	{
		sLine[77]=0;
		CBase::snprintf(str+strlen(str), strLen-strlen(str), "%s\n",	sLine);
		curStrLen += strlen(sLine) + 1;
	}

}

std::string &CTraceWorkManager::getBackTrace(std::string &backTrace)
{
	void *stack_addr[32];
	int layer;
	int i;
	char tmp[256];
	backTrace = "addr2line -e ./Challenge_Debug -f -C  ";
	layer = CBase::backtrace(stack_addr, 32);
	for(i = 3; i < layer; i++)
	{
		CBase::snprintf(tmp, sizeof(tmp), "%p  ", stack_addr[i]);
		backTrace += tmp;
	}
	return backTrace;
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

int CBase::backtrace(void **buffer, int size)
{
#ifdef WIN32
	//assert(0);
	return 0;
#else
	return ::backtrace(buffer, size);
#endif
}

int CBase::close(int fd)
{
#if WIN32
	return closesocket(fd);
#else
	return ::close(fd);
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
		CLen[i] = iLen;
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
	for (int i=0; i<CLen[CLenSize-1]; ++i)
	{
		iLen += (unsigned char)(CLen[i]) << (i<<3);
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

int CLogDataInf::unPacket(char *packet)
{
	if (m_packet && m_packet != packet)
	{
		free(m_packet);
	}
	m_packet = packet;
	return unPacket(m_packet, m_infs, m_infLens);
}

int CLogDataInf::unPacket(char *packet, char *infs[], int infLens[])
{
	char *inf = NULL;
	int infLen = 0;
	C2ILen(packet, m_lenSize, m_packetLen);
	m_infsNum = 0;
	int i=m_lenSize;
	for (; i<m_packetLen-m_lenSize; )
	{
		C2ILen(packet+i,m_lenSize,infLen);
		inf = packet + i + m_lenSize;
		i += infLen;
		infs[m_infsNum] = inf;
		infLens[m_infsNum++] = infLen;
		
	}
	C2ILen(packet+i,m_lenSize,infLen);
	if (infLen != m_packetLen)
	{	
		m_packetLen = 0;
		m_infsNum = 0;
	}
	infs[m_infsNum] = NULL;
	infLens[m_infsNum] = NULL;
	return m_packetLen;
}

int CLogDataInf::packet(char *&packet)
{
	m_packetLen += m_lenSize;

	I2CLen(m_packetLen, m_packet, m_lenSize);
	I2CLen(m_packetLen, m_packet+m_packetLen-m_lenSize, m_lenSize);
	
	packet = m_packet;
	return m_packetLen;
}


