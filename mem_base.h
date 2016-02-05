#ifndef __MEM_BASE_H
#define __MEM_BASE_H
#include <stdlib.h>


#ifdef WIN32
#define container_of(ptr, type, member)  (type *)((char *)ptr - offsetof(type,member))
#else
#define container_of(ptr, type, member) ({  \
const decltype( ((type *)0)->member ) *__mptr = (ptr);   \
(type *)( (char *)__mptr - offsetof(type,member) );})
#endif

namespace base
{
void* malloc(size_t size);
void free(void* p);
void *realloc(void* c, size_t size);
void* calloc(size_t nmemb, size_t size);
}//base



#endif


