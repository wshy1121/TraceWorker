#include "stdafx.h"
#include "trace_worke.h"
#include <signal.h>


//≤‚ ‘CTimeCalc π”√

void fun1()
{	trace_worker();
	trace_printf("NULL");	
	{
		trace_printf("NULL");
		trace_level(4);
		{
			trace_printf("NULL");
			trace_level(3);
			{
				trace_printf("NULL");
				trace_level(2);
				{
					trace_printf("NULL");
					trace_level(1);
					{
						trace_printf("NULL");
						trace_level(0);
					}

				}

			}

		}
	}
}
void fun0(int count)
{	trace_worker();
	if (count == 0)
	{
		return ;
	}
	trace_printf("1234");
	--count;
	fun0(count);
	fun1();
}

void* test1(void *pArg)
{	
	while (1)
	{
		trace_worker();
		fun0(100);
		CBase::usleep(10*1000*1000);
	}
	return NULL;
}

int main()
{
	if (!CBugKiller::startServer("127.0.0.1"))
	{
		printf("startServer failed\n");
		return 0;
	}

	const int threadNum = 16;
	CBase::pthread_t thread_id[threadNum];
	for (int i=0; i<threadNum; ++i)
	{
		CBase::pthread_create(&thread_id[i], NULL,test1,NULL);
	}

	while (true)
	{
		CBase::usleep(1000*1000);
	}

	return 0;
}



