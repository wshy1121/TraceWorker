#ifndef _TRACE_WORK_H_
#define _TRACE_WORK_H_

#ifdef WIN32
#include <assert.h>
#include <process.h>
#else
#include <execinfo.h>
#include <stdarg.h>
#include <pthread.h>
#endif

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
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
	static int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
	static int usleep(int micro_second);
	static int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
	static int pthread_mutex_lock(pthread_mutex_t *mutex);
	static int pthread_mutex_unlock(pthread_mutex_t *mutex);
};

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


#endif




