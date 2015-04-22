#ifdef WIN32
#include <windows.h>
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

#define INF_SIZE 16 
class CBase
{
public:
#ifdef WIN32
	typedef unsigned long pthread_t;
	typedef int  pthread_attr_t;
	typedef CRITICAL_SECTION  pthread_mutex_t;
	typedef int  pthread_mutexattr_t;
#else
	typedef ::pthread_t  pthread_t;
	typedef ::pthread_attr_t  pthread_attr_t;
	typedef ::pthread_mutex_t  pthread_mutex_t;
	typedef ::pthread_mutexattr_t  pthread_mutexattr_t;
#endif
public:
	static int snprintf(char *str, size_t size, const char *format, ...);
	static pthread_t pthread_self(void);
	static int vsnprintf(char *str, size_t size, const char *format, va_list ap);
	static char *strcpy(char *dest, const char *src);
	static int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
	static int usleep(int micro_second);
	static int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
	static int pthread_mutex_lock(pthread_mutex_t *mutex);
	static int pthread_mutex_unlock(pthread_mutex_t *mutex);
	static int backtrace(void **buffer, int size);
	static int close(int fd);
};

class CLogDataInf
{
public:
	CLogDataInf();
	~CLogDataInf();
	void putInf(char *strdata);
	void putInf(const char *strdata);
	int packet(char *&packet);
	int unPacket(char *packet);
	int unPacket(char *packet, char *infs[], int infLens[]);
	void C2ILen(char *CLen, int CLenSize, int &iLen);
private:
	void I2CLen(int iLen, char *CLen, int CLenSize);
public:
	int m_lenSize;	
	char *m_infs[INF_SIZE];
	int m_infLens[INF_SIZE];
	char *m_packet;
	int m_packetLen;
	int m_infsNum;
};

class CTraceWorkManager
{
public:
	static CTraceWorkManager *instance();
	bool startServer(const char *sip, int sport, const char *fileName);
	bool receiveInfData(CLogDataInf *pDataInf);
	int receive(char *szText,int iLen);
	int send(char *szText,int len);
	void InsertHex(char *psBuf, int nBufLen, char *str, int strLen);
	std::string &getBackTrace(std::string &backTrace);
	int reStart();
	int getSessionId();	
private:
	CTraceWorkManager();	
	void openFile(const char *fileName);
	SOCKET connect(const char *sip, int port);
	int disConnect(SOCKET socket);	
	int reConnect();
private:	
	const char *m_sip;
	int m_port;
	std::string m_fileName;
	SOCKET m_socketClient;
	CBase::pthread_mutex_t socketMutex;
	int m_sessionId;
	const int m_maxSessionId;
};

static CTraceWorkManager *g_trace = CTraceWorkManager::instance();

CCandy::CCandy(int line, char *file_name, char *func_name, int display_level)
{
	char sSid[16];
	char sTid[16];
	char sLine[8];
	char sLevel[8];
	CBase::snprintf(sSid, sizeof(sSid), "%d", g_trace->getSessionId());
	CBase::snprintf(sTid, sizeof(sTid), "%d", CBase::pthread_self());
	CBase::snprintf(sLine, sizeof(sLine), "%d", line);
	CBase::snprintf(sLevel, sizeof(sLevel), "%d", display_level);
	
	CLogDataInf dataInf;
	dataInf.putInf((char *)"createCandy");
	dataInf.putInf(sSid);
	dataInf.putInf(sTid);
	dataInf.putInf(sLine);
	dataInf.putInf(file_name);
	dataInf.putInf(func_name);
	dataInf.putInf(sLevel);
	dataInf.putInf("NULL");
	
	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	g_trace->send(packet, packetLen);
	
	g_trace->receiveInfData(&dataInf);
	return ;
}

CCandy::~CCandy()
{
	char sSid[16];
	char sTid[16];	
	CBase::snprintf(sSid, sizeof(sSid), "%d", g_trace->getSessionId());
	CBase::snprintf(sTid, sizeof(sTid), "%d", CBase::pthread_self());
	CLogDataInf dataInf;
	dataInf.putInf((char *)"destroyCandy");
	dataInf.putInf(sSid);
	dataInf.putInf(sTid);
	dataInf.putInf("0");
	dataInf.putInf("");
	dataInf.putInf("");
	dataInf.putInf("0");
	dataInf.putInf("");
	
	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	g_trace->send(packet, packetLen);
	g_trace->receiveInfData(&dataInf);
	return ;
}



void CBugKiller::InsertTrace(int line, char *file_name, const char* fmt, ...)
{
	char content[4096];
	va_list ap;
	va_start(ap,fmt);
	CBase::vsnprintf(content,sizeof(content), fmt, ap);
	va_end(ap);

	char sSid[16];
	char sTid[16];
	char sLine[8];	
	CBase::snprintf(sSid, sizeof(sSid), "%d", g_trace->getSessionId());
	CBase::snprintf(sTid, sizeof(sTid), "%d", CBase::pthread_self());
	CBase::snprintf(sLine, sizeof(sLine), "%d", line);
	
	CLogDataInf dataInf;
	dataInf.putInf((char *)"insertTrace");
	dataInf.putInf(sSid);
	dataInf.putInf(sTid);
	dataInf.putInf(sLine);
	dataInf.putInf(file_name);
	dataInf.putInf("");
	dataInf.putInf("0");
	dataInf.putInf(content);
	
	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	g_trace->send(packet, packetLen);
	g_trace->receiveInfData(&dataInf);
	return ;
}

void CBugKiller::InsertHex(int line, char *file_name, char *psBuf, int nBufLen)
{
	char str[4096];
	g_trace->InsertHex(psBuf, nBufLen, str, sizeof(str));

	char sSid[16];
	char sTid[16];
	char sLine[8];	
	CBase::snprintf(sSid, sizeof(sSid), "%d", g_trace->getSessionId());
	CBase::snprintf(sTid, sizeof(sTid), "%d", CBase::pthread_self());
	CBase::snprintf(sLine, sizeof(sLine), "%d", line);

	CLogDataInf dataInf;
	dataInf.putInf((char *)"insertTrace");
	dataInf.putInf(sSid);
	dataInf.putInf(sTid);
	dataInf.putInf(sLine);
	dataInf.putInf(file_name);
	dataInf.putInf("");
	dataInf.putInf("0");
	dataInf.putInf(str);

	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	g_trace->send(packet, packetLen);
	g_trace->receiveInfData(&dataInf);	
	return ;
}

void CBugKiller::DispAll()
{
	char sSid[16];
	char sTid[16];	
	CBase::snprintf(sSid, sizeof(sSid), "%d", g_trace->getSessionId());
	CBase::snprintf(sTid, sizeof(sTid), "%d", CBase::pthread_self());
	CLogDataInf dataInf;
	dataInf.putInf((char *)"dispAll");
	dataInf.putInf(sSid);
	dataInf.putInf(sTid);
	dataInf.putInf("0");
	dataInf.putInf("");
	dataInf.putInf("");
	dataInf.putInf("0");
	dataInf.putInf("backtrace");

	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	g_trace->send(packet, packetLen);
	g_trace->receiveInfData(&dataInf);
	return ;	
}


void CBugKiller::InsertTag(int line, char *file_name, const char* fmt, ...)
{
	char content[4096];
	va_list ap;
	va_start(ap,fmt);
	CBase::vsnprintf(content,sizeof(content), fmt, ap);
	va_end(ap);

	char sSid[16];
	char sTid[16];
	char sLine[8];	
	CBase::snprintf(sSid, sizeof(sSid), "%d", g_trace->getSessionId());
	CBase::snprintf(sTid, sizeof(sTid), "%d", CBase::pthread_self());
	CBase::snprintf(sLine, sizeof(sLine), "%d", line);

	CLogDataInf dataInf;
	dataInf.putInf((char *)"insertTag");
	dataInf.putInf(sSid);
	dataInf.putInf(sTid);
	dataInf.putInf(sLine);
	dataInf.putInf(file_name);
	dataInf.putInf("");
	dataInf.putInf("0");
	dataInf.putInf(content);

	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	g_trace->send(packet, packetLen);
	g_trace->receiveInfData(&dataInf);	
	return ;
}

void CBugKiller::printfMemInfMap()
{
	char sSid[16];
	char sTid[16];	
	CBase::snprintf(sSid, sizeof(sSid), "%d", g_trace->getSessionId());
	CBase::snprintf(sTid, sizeof(sTid), "%d", CBase::pthread_self());
	CLogDataInf dataInf;
	dataInf.putInf((char *)"printfMemInfMap");
	dataInf.putInf(sSid);
	dataInf.putInf(sTid);
	dataInf.putInf("0");
	dataInf.putInf("");
	dataInf.putInf("");
	dataInf.putInf("0");
	dataInf.putInf("");

	
	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	g_trace->send(packet, packetLen);
	g_trace->receiveInfData(&dataInf);
	return ;	
}

void CBugKiller::printfStackInfo(int line, char *file_name)
{
	std::string backTrace;
	g_trace->getBackTrace(backTrace);
	InsertTrace(line, file_name, backTrace.c_str());
}

bool CBugKiller::startServer(const char *sip, int sport, const char *fileName)
{
	return g_trace->startServer(sip, sport, fileName);
}

int CBugKiller::reStart()
{
	return CTraceWorkManager::instance()->reStart();
}

CTraceWorkManager::CTraceWorkManager():m_sessionId(1), m_maxSessionId(1024*1024)
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

bool CTraceWorkManager::startServer(const char *sip, int sport, const char *fileName)
{
	int serverPort = sport;
	CBase::pthread_mutex_init(&socketMutex, NULL);
	m_socketClient = connect(sip, serverPort);
	if(-1 == m_socketClient)
	{
		return false;
	}
	openFile(fileName);
	return true;
}
void CTraceWorkManager::openFile(const char *fileName)
{
	char sSid[16];
	char sTid[16];
	CBase::snprintf(sSid, sizeof(sSid), "%d", g_trace->getSessionId());
	CBase::snprintf(sTid, sizeof(sTid), "%d", CBase::pthread_self());
	CLogDataInf dataInf;
	dataInf.putInf((char *)"openFile");
	dataInf.putInf(sSid);
	dataInf.putInf(sTid);
	dataInf.putInf("0");
	dataInf.putInf("");
	dataInf.putInf("");
	dataInf.putInf("0");
	dataInf.putInf(fileName);

	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	send(packet, packetLen);
	receiveInfData(&dataInf);

	m_fileName = fileName;
	return ;	
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
	if (m_socketClient <= 0)
	{
		return -1;
	}
	return 0;
}

int CTraceWorkManager::reStart()
{
	int iRet = 0;
	iRet = reConnect();
	if (iRet != 0)
	{
		return iRet;
	}
	openFile(m_fileName.c_str());
	return 0;
}

int CTraceWorkManager::getSessionId()
{
	return 0;
	if (m_sessionId > m_maxSessionId)
	{
		m_sessionId = 0;
		
	}
	return ++m_sessionId;
}

bool CTraceWorkManager::receiveInfData(CLogDataInf *pDataInf)
{
	int sessionId = atoi(pDataInf->m_infs[1]);
	if (sessionId <= 0)
	{
		return true;
	}
	
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

	m_infs[m_infsNum] = strdata;
	m_infLens[m_infsNum++] = dataLen;	
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


