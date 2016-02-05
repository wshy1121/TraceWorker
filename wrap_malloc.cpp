#include <stdio.h>
#include <sys/types.h>
#include <string.h>

#include "thread_queue.h"

extern "C" void *__wrap_malloc(size_t c)
{
	if (ThreadQueue::getEnable())
	{
		void* p = NULL;//CMemCheck::malloc(c);
		ThreadQueue::instance()->wrapMalloc(p, c);
		return p;
	}

	return base::malloc(c);
}
extern "C" void* __wrap_realloc(void *p, size_t c)
{
	if (p && ThreadQueue::getEnable())
	{
		p = NULL;//CMemCheck::realloc(p, c);
		ThreadQueue::instance()->wrapMalloc(p, c);
		return p;
	}

	return base::realloc(p, c);
}
extern "C" void* __wrap_calloc(size_t nmemb, size_t size)
{
	if (ThreadQueue::getEnable())
	{
		void *p = NULL;//CMemCheck::calloc(nmemb, size); 
		ThreadQueue::instance()->wrapMalloc(p, nmemb * size);
		return p;
	}

	return base::calloc(nmemb, size);
}
extern "C" void __wrap_free(void*p)
{
	if (p && ThreadQueue::getEnable())
	{
		ThreadQueue::instance()->wrapFree(p);
		//CMemCheck::free(p);
		return ;
	}

	base::free(p);
}




