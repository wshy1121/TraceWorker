#ifdef WIN32
#include <windows.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include<linux/tcp.h>
typedef int SOCKET;
#endif
#include <map>
#include <sys/ioctl.h>
#include <assert.h>
#include "trace_worker.h"
#include "trace_base.h"
#include "trace_packet.h"
#include "socket_opr.h"
#include "SimpleIni.h"
#include "rb_tree.h"

class CTraceWorkManager;
class TraceDeep
{
public:
    friend class CTraceWorkManager;
    TraceDeep(){}
public:
    bool operator > (const TraceDeep &key)
    {
        return m_tid > key.m_tid;
    }

    bool operator == (const TraceDeep &key)
    {
        return m_tid == key.m_tid;
    }
private:
    CBase::pthread_t m_tid;
    int m_deep;
    bool m_isCon;
};

class CTraceWorkManager
{
public:
	static CTraceWorkManager *instance();
	bool startServer(const char *sip, int sport, const char *fileName);
	bool isStarted();
	int dealPacket(CLogDataInf &sendDataInf, CLogDataInf &recvDataInf);
	void InsertHex(int line, char *file_name, char *psBuf, int nBufLen);
	void packetHex(char *psBuf, int nBufLen, char *str, int strLen);
	void InsertTrace(int line, char *file_name, const char *format, va_list ap);
	std::string &getBackTrace(std::string &backTrace);
	int reStart();
	int getSessionId(bool enabl = false);	
	static void ctrl_c_func(int signo);
    bool createCandy();
    bool destroyCandy();
private:
	CTraceWorkManager(); 
	~CTraceWorkManager(); 
	void openFile(const char *fileName);
	int connect(const char *sip, int port);
	int disConnect(int socket);	
	int reConnect();
	bool receiveInfData(CLogDataInf *pDataInf);
	int receive(char *szText,int iLen);
	int send(char *szText,int len);	
    bool getServerInf(std::string &ip, int &port);
    
    void removeTraceDeep(TraceDeep &traceDeep);
    TraceDeep *getTraceDeep(CBase::pthread_t tid);
    TraceDeep *creatTraceDeep(CBase::pthread_t tid);
    static void* reConThread(void *pArg);
    void __reConThread(void *pArg);
    int connectServer(const char *sip, int serverPort);
private:	
	std::string m_sip;
	int m_port;
	std::string m_fileName;
	int m_socketClient;
	CBase::pthread_mutex_t socketMutex;
	int m_sessionId;
	const int m_maxSessionId;
    CRbTree<TraceDeep> m_threadTree;
    CBase::pthread_mutex_t m_threadTreeMutex;
    CBase::pthread_t m_reConnectThread;
	static CTraceWorkManager _instance;
};

static CTraceWorkManager *g_trace = CTraceWorkManager::instance();

static void handlePipe(int sig)
{
}

CCandy::CCandy(int line, char *file_name, char *func_name, int pre_line, char *pre_file_name, char *pre_func_name, int display_level)
{
	if (!g_trace->createCandy())
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
	
	CLogDataInf dataInf(false);
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
	
	g_trace->dealPacket(dataInf, dataInf);
	return ;
}

CCandy::~CCandy()
{
	if (!g_trace->destroyCandy())
	{
		return ;
	}

	char sSid[16];
	char sTid[16];	
	char sLine[8];
	CBase::snprintf(sSid, sizeof(sSid), "%d", g_trace->getSessionId());
	CBase::snprintf(sTid, sizeof(sTid), "%d", CBase::pthread_self());
	CBase::snprintf(sLine, sizeof(sLine), "%d", m_line);
	CLogDataInf dataInf(false);
	dataInf.putInf((char *)"destroyCandy");
	dataInf.putInf(sSid);
	dataInf.putInf(sTid);
	dataInf.putInf(sLine);
	dataInf.putInf(m_fileName.c_str());
	dataInf.putInf(m_funcName.c_str());
	dataInf.putInf("0");
	dataInf.putInf("");
	
	g_trace->dealPacket(dataInf, dataInf);
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
	CLogDataInf dataInf(false);
	dataInf.putInf((char *)"dispAll");
	dataInf.putInf(sSid);
	dataInf.putInf(sTid);
	dataInf.putInf("0");
	dataInf.putInf("");
	dataInf.putInf("");
	dataInf.putInf("0");
	dataInf.putInf("backtrace");

	g_trace->dealPacket(dataInf, dataInf);
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

	CLogDataInf dataInf(false);
	dataInf.putInf((char *)"insertTag");
	dataInf.putInf(sSid);
	dataInf.putInf(sTid);
	dataInf.putInf(sLine);
	dataInf.putInf(file_name);
	dataInf.putInf("");
	dataInf.putInf("0");
	dataInf.putInf(content);

	g_trace->dealPacket(dataInf, dataInf);
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
	CLogDataInf dataInf(false);
	dataInf.putInf((char *)"printfMemInfMap");
	dataInf.putInf(sSid);
	dataInf.putInf(sTid);
	dataInf.putInf("0");
	dataInf.putInf("");
	dataInf.putInf("");
	dataInf.putInf("0");
	dataInf.putInf("");

	
	g_trace->dealPacket(dataInf, dataInf);
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
CTraceWorkManager *CTraceWorkManager::instance()
{
	return &_instance;
}

CTraceWorkManager::CTraceWorkManager()
:m_socketClient(-1)
,m_sessionId(1)
,m_maxSessionId(1024*1024)
{
    CBase::pthread_mutex_init(&socketMutex, NULL);
    CBase::pthread_mutex_init(&m_threadTreeMutex, NULL);
#ifdef WIN32	
	WSADATA wsa={0};
	WSAStartup(MAKEWORD(2,2),&wsa);
#endif

    struct sigaction action;
    action.sa_handler = handlePipe;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGPIPE, &action, NULL);
}

CTraceWorkManager::~CTraceWorkManager()
{
}

bool CTraceWorkManager::startServer(const char *sip, int sport, const char *fileName)
{
	if (isStarted())
	{
		return true;
	}
    
	m_sip = sip;
	m_port = sport;
    m_fileName = fileName;
    
    m_socketClient = connectServer(sip, sport);
    if (m_socketClient == -1)
    {
        return false;
    }
    
	openFile(fileName);
    CBase::pthread_create(&m_reConnectThread, NULL, reConThread, NULL);
	return true;
}

int CTraceWorkManager::connectServer(const char *sip, int serverPort)
{
    int socketClient = -1;
    std::string serIp;
    int serPort = 0;
    bool bRet = getServerInf(serIp, serPort);
    if (bRet == true)
    {
        socketClient = connect(serIp.c_str(), serPort);
        if(-1 == socketClient)
        {
            socketClient = connect(sip, serverPort);
            if(-1 == socketClient)
            {
                return -1;
            }
        }   
    }
    else
    {
        socketClient = connect(sip, serverPort);
        if(-1 == socketClient)
        {
            return -1;
        }
    }
    return socketClient;
}

bool CTraceWorkManager::getServerInf(std::string &ip, int &port)
{
    CSimpleIniA ini;  
    ini.SetUnicode();  
    SI_Error ret = ini.LoadFile("TraceCfg.ini");
    if (ret != SI_OK)
    {
        return false;
    }
    
    const char *netCliIp = ini.GetValue("NetConfig", "SerIp", "192.168.0.1");  
    int netCliPort = (int)ini.GetLongValue("NetConfig", "SerPort", 0);
    ip = netCliIp;
    port = netCliPort;
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
	CLogDataInf dataInf(false);
	dataInf.putInf((char *)"openFile");
	dataInf.putInf(sSid);
	dataInf.putInf(sTid);
	dataInf.putInf("0");
	dataInf.putInf("");
	dataInf.putInf("");
	dataInf.putInf("0");
	dataInf.putInf((char *)fileName);
    

	dealPacket(dataInf, dataInf);
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

	unsigned long block = 1;
	ioctl(socketClient,FIONBIO,&block);

	struct sockaddr_in addr;
	addr.sin_family 		= AF_INET;
	addr.sin_addr.s_addr	= inet_addr(sip);
	addr.sin_port			= htons(port);
	
	int ret = ::connect(socketClient, (struct sockaddr *) & addr, sizeof(sockaddr_in));
	if(-1 == ret)
	{
		int maxFd = socketClient + 1;
		struct timeval timeOut;
		timeOut.tv_sec = 0;
		timeOut.tv_usec = 10000;
		fd_set writeFds;
		FD_ZERO(&writeFds);
		FD_SET(socketClient,&writeFds);
		if(select(maxFd,NULL,&writeFds,NULL,&timeOut) <= 0 )
		{
			close(socketClient);
			socketClient = -1;
			return -1;
		}

		int error = -1;
		socklen_t len = sizeof(int);
		getsockopt(socketClient,SOL_SOCKET,SO_ERROR,&error,&len);
		if( 0 != error )
		{
			close(socketClient);
			socketClient = -1;
			return -1;
		}
	}

    int no_delay = 1;
    int iErr = setsockopt(socketClient, IPPROTO_TCP, TCP_NODELAY, &no_delay, sizeof(no_delay));
    if (iErr == -1)
    {
        printf("set no_delay failed\n");
        return -1;
    }
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
	m_socketClient = connect(m_sip.c_str(), m_port);
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


int CTraceWorkManager::dealPacket(CLogDataInf &sendDataInf, CLogDataInf &recvDataInf)
{
    int headLen = 4 + sizeof(int);
    int tailLen = 4;
    
    int newLen = sendDataInf.getPkLen();
    char *sendBuffer = new char[newLen + headLen + tailLen];
    int sendBufferLen = 0;
    
    char *packet = sendBuffer+headLen;
    int packetLen = sendDataInf.packet(packet);
    if (newLen != packetLen)
    {
        printf("dealPacket failed  newLen != packetLen %d  %d\n", newLen, packetLen);
        delete []sendBuffer;
        return -1;
    }

	char *headData = sendBuffer;
	int headDataLen = 0;
    
	unsigned int strLenNum = packetLen + 12;
	memcpy(headData+headDataLen, (char *)"\x7B\x7B\x7B\x7B", 4);
	headDataLen += 4;
	memcpy(headData+headDataLen, &strLenNum, sizeof(int));
	headDataLen += sizeof(int);

    char *tailData = sendBuffer + headLen + packetLen;
    int tailDataLen = 0;
    memcpy(tailData, "\x7D\x7D\x7D\x7D", 4);
    tailDataLen += 4;

    sendBufferLen = headDataLen + packetLen + tailDataLen;
    
	CBase::pthread_mutex_lock(&socketMutex);
	send(sendBuffer, sendBufferLen);
	receiveInfData(&recvDataInf);	
	CBase::pthread_mutex_unlock(&socketMutex);
    
    delete []sendBuffer;
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

	char packet[512];
    if (iLen > (int)sizeof(packet))
    {
        printf("receiveInfData iLen is too long %d\n", iLen);
        return false;
    }
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
    int tryTime = 0;
	while(cnt>0)
	{  
		rc=::send(m_socketClient,szText,cnt,0);
		if(rc <= 0)
		{
		    if (errno == EAGAIN && ++tryTime < 8)
            {
                CBase::usleep(8000);
                continue;
            }

            CBase::close(m_socketClient);
            m_socketClient = -1;
            printf("send errno is %d\n", errno);
            break;
		}
		szText += rc;
		cnt -= rc;
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

	CLogDataInf dataInf(false);
	dataInf.putInf((char *)"insertTrace");
	dataInf.putInf(sSid);
	dataInf.putInf(sTid);
	dataInf.putInf(sLine);
	dataInf.putInf(file_name);
	dataInf.putInf("");
	dataInf.putInf("0");
	dataInf.putInf(str);

	dealPacket(dataInf, dataInf);
	return ;
}

void CTraceWorkManager::packetHex(char *psBuf, int nBufLen, char *str, int strLen)
{
	/* save log msg in file */
	CBase::snprintf(str, strLen, "hex%s:len=%4d\r\n", __FUNCTION__, nBufLen);
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
			CBase::snprintf(str+strlen(str), strLen-strlen(str), "%s\r\n", sLine);
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
		CBase::snprintf(str+strlen(str), strLen-strlen(str), "%s\r\n",	sLine);
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
	
	CLogDataInf dataInf(false);
	dataInf.putInf((char *)"insertTrace");
	dataInf.putInf(sSid);
	dataInf.putInf(sTid);
	dataInf.putInf(sLine);
	dataInf.putInf(file_name);
	dataInf.putInf("");
	dataInf.putInf("0");
	dataInf.putInf(content);
	
	dealPacket(dataInf, dataInf);
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


bool CTraceWorkManager::createCandy()
{
    bool isStarted = g_trace->isStarted();
    TraceDeep *traceDeep = creatTraceDeep(CBase::pthread_self());
    traceDeep->m_deep++;

    if (isStarted)
    {
        if (traceDeep->m_deep == 1)
        {
            traceDeep->m_isCon = true;
        }
    }
    else
    {
        traceDeep->m_isCon = false;
    }
    
    return traceDeep->m_isCon;
}

bool CTraceWorkManager::destroyCandy()
{
    TraceDeep *traceDeep = getTraceDeep(CBase::pthread_self());
    if(traceDeep)
    {
        traceDeep->m_deep--;
        if (traceDeep->m_deep == 0)
        {
            removeTraceDeep(*traceDeep);
        }
        return traceDeep->m_isCon;
    }

    return false;
}

void CTraceWorkManager::removeTraceDeep(TraceDeep &traceDeep)
{
    CBase::pthread_mutex_lock(&m_threadTreeMutex);
    m_threadTree.remove(traceDeep); 
    CBase::pthread_mutex_unlock(&m_threadTreeMutex);
}



TraceDeep *CTraceWorkManager::getTraceDeep(CBase::pthread_t tid)
{
    TraceDeep traceDeep;    
    traceDeep.m_tid = tid;
    
    CBase::pthread_mutex_lock(&m_threadTreeMutex);
    TraceDeep *pTraceDeep = m_threadTree.search(traceDeep);
    CBase::pthread_mutex_unlock(&m_threadTreeMutex);
    
	return pTraceDeep;
}

TraceDeep *CTraceWorkManager::creatTraceDeep(CBase::pthread_t tid)
{
    CBase::pthread_mutex_lock(&m_threadTreeMutex);
    TraceDeep traceDeep;    
    traceDeep.m_tid = tid;
    
    TraceDeep *pTraceDeep = m_threadTree.search(traceDeep);
	if (pTraceDeep == NULL)  
	{
        traceDeep.m_deep = 0;
        traceDeep.m_tid = tid;
        traceDeep.m_isCon = false;
        pTraceDeep = m_threadTree.insert(traceDeep);
    }
    
    CBase::pthread_mutex_unlock(&m_threadTreeMutex);
    return pTraceDeep;
}

void* CTraceWorkManager::reConThread(void *pArg)
{
    g_trace->__reConThread(pArg);
    return NULL;
}
void CTraceWorkManager::__reConThread(void *pArg)
{
    int sleepTime = 3*1000*1000;
    while (1)
    {
        CBase::usleep(sleepTime);
        if (m_socketClient == -1)
        {
            m_socketClient = connectServer(m_sip.c_str(), m_port);
            if (m_socketClient > 0)
            {
                openFile(m_fileName.c_str());
            }
        }
    }
    return ;
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

