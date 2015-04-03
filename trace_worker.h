#ifndef _TRACE_WORK_H_
#define _TRACE_WORK_H_

#ifdef WIN32
#include <windows.h>
#include <process.h>
#include <assert.h>
#else
#include <execinfo.h>
#include <stdarg.h>
#include <pthread.h>
typedef int SOCKET;
#endif

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <string>

class CCandy
{
public:
	CCandy(int line=__LINE__, char *file_name=(char *)__FILE__, char *func_name=(char *)"__FUNCTION__", int display_level=100);
	~CCandy();
};


class CBugKiller
{
public:
	static void InsertTrace(int line, char *file_name, const char* fmt, ...);
	static void InsertHex(int line, char *file_name, char *psBuf, int nBufLen);
	static void DispAll();
	static void InsertTag(int line, char *file_name, const char* fmt, ...);
	static void printfMemInfMap();
	static void openFile(const char *fileName);
	static void printfStackInfo(int line, char *file_name);
	static bool startServer(const char *ip);
private:
	CBugKiller();
};



#if !defined(NO_CTIME_CALC)
#define trace_level(level)  CCandy candy(__LINE__, (char *)__FILE__, (char *)__FUNCTION__, level)
#define trace_printf(format, ...)    CBugKiller::InsertTrace(__LINE__, (char *)__FILE__, format, ## __VA_ARGS__)
#define trace_all()    CBugKiller::DispAll()
#define trace_tag(format, ...)  CBugKiller::InsertTag(__LINE__, (char *)__FILE__, format, ## __VA_ARGS__)
#define trace_str(str, len)    CBugKiller::InsertHex(__LINE__, (char *)__FILE__, str, len)
#define trace_mem()   CBugKiller::printfMemInfMap()
#define trace_stack()	CBugKiller::printfStackInfo(__LINE__, (char *)__FILE__)
#define trace_num(num)	 trace_printf("num:%d    %d", num, __LINE__)
#define trace_err(num)       trace_printf("ERRERRERRERRERRERRERR:%d    %d    %s", (num), __LINE__, __FILE__)
#define trace_unworker()  trace_level(0)
#define trace_worker()   trace_level(100)

#else
#define trace_level(level)    
#define trace_printf(format, ...)      
#define trace_all()    
#define trace_tag(format, ...)   
#define trace_str(str, len)    
#define trace_mem()
#define trace_stack()	  
#define trace_num(num)	   
#define trace_err(num)        
#define trace_unworker()    
#define trace_worker()   
#endif



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
private:
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
	bool startServer(const char *ip);
	bool receiveInfData(CLogDataInf *pDataInf);
	int receive(char *szText,int iLen);
	int send(char *szText,int len);
	void InsertHex(char *psBuf, int nBufLen, char *str, int strLen);
	std::string &getBackTrace(std::string &backTrace);
	int reConnect();
private:
	CTraceWorkManager();	
	SOCKET connect(const char *sip, int port);
	int disConnect(SOCKET socket);
private:	
	const char *m_sip;
	int m_port;
	SOCKET m_socketClient;
	CBase::pthread_mutex_t socketMutex;
};





#endif




