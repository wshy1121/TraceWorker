#ifdef WIN32
#include <windows.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
typedef int SOCKET;
#endif
#include <map>
#include "trace_worker.h"
#include "trace_base.h"
#include "trace_packet.h"
#include "socket_opr.h"

class CTraceWorkManager
{
public:
	static CTraceWorkManager *instance();
	bool startServer(const char *sip, int sport, const char *fileName);
	bool isStarted();
	int dealPacket(char *packet, int packetLen, CLogDataInf &dataInf);
	void InsertHex(int line, char *file_name, char *psBuf, int nBufLen);
	void packetHex(char *psBuf, int nBufLen, char *str, int strLen);
	void InsertTrace(int line, char *file_name, const char *format, va_list ap);
	std::string &getBackTrace(std::string &backTrace);
	int reStart();
	int getSessionId(bool enabl = false);	
	static void ctrl_c_func(int signo);
private:
	CTraceWorkManager(); 
	void openFile(const char *fileName);
	int connect(const char *sip, int port);
	int disConnect(int socket);	
	int reConnect();
	bool receiveInfData(CLogDataInf *pDataInf);
	int receive(char *szText,int iLen);
	int send(char *szText,int len);	
private:	
	const char *m_sip;
	int m_port;
	std::string m_fileName;
	int m_socketClient;
	CBase::pthread_mutex_t socketMutex;
	int m_sessionId;
	const int m_maxSessionId;
	static CTraceWorkManager _instance;
};

static CTraceWorkManager *g_trace = CTraceWorkManager::instance();

CCandy::CCandy(int line, char *file_name, char *func_name, int pre_line, char *pre_file_name, char *pre_func_name, int display_level)
{
	if (!g_trace->isStarted())
	{
		return ;
	}
	m_line = line;
	m_fileName = file_name;
	m_funcName = func_name;
	
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

	char preLine[8];
	CBase::snprintf(preLine, sizeof(preLine), "%d", pre_line);
	dataInf.putInf(preLine);
	dataInf.putInf(pre_file_name);
	dataInf.putInf(pre_func_name);
	
	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	g_trace->dealPacket(packet, packetLen, dataInf);
	return ;
}

CCandy::~CCandy()
{
	if (!g_trace->isStarted())
	{
		return ;
	}

	char sSid[16];
	char sTid[16];	
	char sLine[8];
	CBase::snprintf(sSid, sizeof(sSid), "%d", g_trace->getSessionId());
	CBase::snprintf(sTid, sizeof(sTid), "%d", CBase::pthread_self());
	CBase::snprintf(sLine, sizeof(sLine), "%d", m_line);
	CLogDataInf dataInf;
	dataInf.putInf((char *)"destroyCandy");
	dataInf.putInf(sSid);
	dataInf.putInf(sTid);
	dataInf.putInf(sLine);
	dataInf.putInf(m_fileName.c_str());
	dataInf.putInf(m_funcName.c_str());
	dataInf.putInf("0");
	dataInf.putInf("");
	
	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	g_trace->dealPacket(packet, packetLen, dataInf);
	return ;
}



void CBugKiller::InsertTrace(int line, char *file_name, const char* fmt, ...)
{
	va_list ap;
	va_start(ap,fmt);
	g_trace->InsertTrace(line, file_name, fmt, ap);
	va_end(ap);
}

void CBugKiller::InsertHex(int line, char *file_name, char *psBuf, int nBufLen)
{
	g_trace->InsertHex(line, file_name, psBuf, nBufLen);
	return ;
}

void CBugKiller::DispAll()
{
	if (!g_trace->isStarted())
	{
		return ;
	}

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
	g_trace->dealPacket(packet, packetLen, dataInf);
	return ;	
}


void CBugKiller::InsertTag(int line, char *file_name, const char* fmt, ...)
{
	if (!g_trace->isStarted())
	{
		return ;
	}

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
	g_trace->dealPacket(packet, packetLen, dataInf);
	return ;
}

void CBugKiller::printfMemInfMap()
{
	if (!g_trace->isStarted())
	{
		return ;
	}

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
	g_trace->dealPacket(packet, packetLen, dataInf);
	return ;	
}

void CBugKiller::printfStackInfo(int line, char *file_name)
{
	std::string backTrace;
	g_trace->getBackTrace(backTrace);
	InsertTrace(line, file_name, backTrace.c_str());
}



void CBugKiller::traceSignal(int signo)
{
#if 0
	struct sigaction act;

	act.sa_handler = CTraceWorkManager::ctrl_c_func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	sigaction(signo, &act, NULL);	
#endif
}


bool CBugKiller::startServer(const char *sip, int sport, const char *fileName)
{
	return g_trace->startServer(sip, sport, fileName);
}

int CBugKiller::reStart()
{
	return CTraceWorkManager::instance()->reStart();
}

CTraceWorkManager CTraceWorkManager::_instance;
CTraceWorkManager::CTraceWorkManager():m_socketClient(-1), m_sessionId(1), m_maxSessionId(1024*1024)
{
#ifdef WIN32	
		WSADATA wsa={0};
		WSAStartup(MAKEWORD(2,2),&wsa);
#endif
}

CTraceWorkManager *CTraceWorkManager::instance()
{
	return &_instance;
}

bool CTraceWorkManager::startServer(const char *sip, int sport, const char *fileName)
{
	if (isStarted())
	{
		return true;
	}

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

bool CTraceWorkManager::isStarted()
{
	if(m_socketClient <= 0)
	{
		return false;
	}
	return true;
}
void CTraceWorkManager::ctrl_c_func(int signo)
{
	CBugKiller::InsertTrace(__LINE__, (char *)__FILE__, "signal  %d", signo);
	CBugKiller::printfStackInfo(__LINE__, (char *)__FILE__);
	exit(0);
}

void CTraceWorkManager::openFile(const char *fileName)
{
	if (!g_trace->isStarted())
	{
		return ;
	}

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
	dataInf.putInf((char *)fileName);
    

	char *packet = NULL;
	int packetLen = dataInf.packet(packet);
	dealPacket(packet, packetLen, dataInf);
	m_fileName = fileName;
	return ;	
}

int CTraceWorkManager::connect(const char *sip, int port)
{
	int socketClient = socket(AF_INET, SOCK_STREAM, 0);
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

int CTraceWorkManager::disConnect(int socket)
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

int CTraceWorkManager::getSessionId(bool enabl)
{
	if (!enabl)
	{
		return 0;
	}
	if (m_sessionId > m_maxSessionId)
	{
		m_sessionId = 0;
		
	}
	return ++m_sessionId;
}

int CTraceWorkManager::dealPacket(char *packet, int packetLen, CLogDataInf &dataInf)
{
	char sendData[16];
	int sendDataLen = 0;
	unsigned int strLenNum = packetLen + 12;
	
	memcpy(sendData+sendDataLen, (char *)"\x7B\x7B\x7B\x7B", 4);
	sendDataLen += 4;
	memcpy(sendData+sendDataLen, &strLenNum, sizeof(int));
	sendDataLen += sizeof(int);
	
	CBase::pthread_mutex_lock(&socketMutex);
	send(sendData, sendDataLen);
	send(packet, packetLen);
	send((char *)"\x7D\x7D\x7D\x7D", 4);
	
	receiveInfData(&dataInf);	
	CBase::pthread_mutex_unlock(&socketMutex);
	return 0;
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
	if(-1 == m_socketClient)
	{
		return -1;
	}

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
	if(-1 == m_socketClient)
	{
		return -1;
	}

	int cnt;
	int rc;
	cnt=len;
	while(cnt>0)
	{	
		rc=::send(m_socketClient,szText,cnt,0);
		if(rc==-1)
		{
			m_socketClient = -1;
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

void CTraceWorkManager::InsertHex(int line, char *file_name, char *psBuf, int nBufLen)
{
	if (!g_trace->isStarted())
	{
		return ;
	}

	char str[4096];
	g_trace->packetHex(psBuf, nBufLen, str, sizeof(str));

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
	dealPacket(packet, packetLen, dataInf);
	return ;
}

void CTraceWorkManager::packetHex(char *psBuf, int nBufLen, char *str, int strLen)
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
		if ( isprint ((unsigned char)psBuf[i]))
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

void CTraceWorkManager::InsertTrace(int line, char *file_name, const char *format, va_list ap)
{
	if (!g_trace->isStarted())
	{
		return ;
	}

	char content[4096];
	CBase::vsnprintf(content,sizeof(content), format, ap);
	
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
	dealPacket(packet, packetLen, dataInf);
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


extern "C"
{
static CBase::pthread_mutex_t candyMapMutex;
static std::map<int, CCandy *> candyMap;

int createCandy(int line, char *fileName, char *funcName, int preLine, char *preFileName, char *preFuncName, int displayLevel)
{
	CCandy *pCandy = new CCandy(line, fileName, funcName, preLine, preFileName, preFuncName, displayLevel);
	int sessionId = g_trace->getSessionId(true);

	CBase::pthread_mutex_lock(&candyMapMutex);
	candyMap.insert(std::make_pair(sessionId, pCandy));
	CBase::pthread_mutex_unlock(&candyMapMutex);
	return sessionId;
}

void destroyCandy(int sessionId)
{
	CBase::pthread_mutex_lock(&candyMapMutex);
	std::map<int, CCandy *>::iterator iter = candyMap.find(sessionId);
	if (iter == candyMap.end())
	{	CBase::pthread_mutex_unlock(&candyMapMutex);
		return ;
	}
	CCandy *pCandy = iter->second;	
	candyMap.erase(iter);
	CBase::pthread_mutex_unlock(&candyMapMutex);
	delete pCandy;
}
void InsertTrace(int line, char *file_name, const char* fmt, ...)
{
	va_list ap;
	va_start(ap,fmt);
	g_trace->InsertTrace(line, file_name, fmt, ap);
	va_end(ap);
}

void InsertHex(int line, char *file_name, char *psBuf, int nBufLen)
{
	g_trace->InsertHex(line, file_name, psBuf, nBufLen);
}

//#define trace_all()    CBugKiller::DispAll()
//#define trace_tag(format, ...)  CBugKiller::InsertTag(__LINE__, (char *)__FILE__, format, ## __VA_ARGS__)
//#define trace_str(str, len)    CBugKiller::InsertHex(__LINE__, (char *)__FILE__, str, len)
//#define trace_mem()   CBugKiller::printfMemInfMap()
//#define trace_stack()	CBugKiller::printfStackInfo(__LINE__, (char *)__FILE__)
//#define trace_num(num)	 trace_printf("num:%d    %d", num, __LINE__)
//#define trace_err(num)       trace_printf("ERRERRERRERRERRERRERR:%d    %d    %s", (num), __LINE__, __FILE__)
//#define trace_unworker()  trace_level(0)
//#define trace_worker()   trace_level(100)
bool startServer(const char *sip, int sport, const char *fileName)
{
	return CBugKiller::startServer(sip, sport, fileName);
}
//#define trace_signal(signo)  CBugKiller::traceSignal(signo)

}

