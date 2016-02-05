#include "mem_base.h"
extern "C" void __real_free(void* p);
extern "C" void* __real_malloc(size_t);
extern "C" void *__real_realloc(void* c, int size);
extern "C" void* __real_calloc(size_t nmemb, size_t size);

namespace base
{

#ifdef WIN32
void* malloc(size_t size)
{
	return ::malloc(size);
}


void free(void* p)
{
	::free(p);
}

void *realloc(void* c, size_t size)
{
	return ::realloc(c, (int)size);
}

void* calloc(size_t nmemb, size_t size)
{
	return ::calloc((int)nmemb, (int)size);
}
#else
void* malloc(size_t size)
{
	return ::malloc(size);
}


void free(void* p)
{
	::free(p);
}

void *realloc(void* c, size_t size)
{
	return ::realloc(c, (int)size);
}

void* calloc(size_t nmemb, size_t size)
{
	return ::calloc((int)nmemb, (int)size);
}
#endif
}//base


