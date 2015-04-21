#ifndef _TRACE_WORK_H_
#define _TRACE_WORK_H_

#ifdef WIN32
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
	static void printfStackInfo(int line, char *file_name);
	static bool startServer(const char *sip, int sport, const char *fileName = "Debug.cpp");
	static int reStart();
private:
	CBugKiller();
};


#if !defined(_DEBUG)
#define NO_CTIME_CALC
#endif
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
#define trace_start(sip, sport, fileName)  CBugKiller::startServer(sip, sport, fileName)
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
#define trace_start(sip, sport, fileName)  

#endif









#endif




