#include "trace_base.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <execinfo.h>

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



