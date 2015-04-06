#include "stdafx.h"
#include "trace_worker.h"

char testData[16*1024];
//²âÊÔCTimeCalcÊ¹ÓÃ
void fun1()
{	trace_worker();
	trace_stack();
	trace_printf(testData);	
	trace_printf("get_stack  %s", "stack.c_str()");
	trace_str(testData, sizeof(testData));
	trace_tag(testData);
	{
		trace_printf(testData);
		trace_level(4);
		{
			trace_printf(testData);
			trace_level(3);
			{
				trace_printf(testData);
				trace_level(2);
				{
					trace_printf(testData);
					trace_level(1);
					{
						trace_printf(testData);
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
		CBase::usleep(1000*1000);
	}
	return NULL;
}

int main()
{
	CBugKiller::startServer("127.0.0.1");
	{
		trace_worker();
	}

	while (1)
	{
		{
			trace_worker();
			fun0(5);
			//CBase::usleep(100);
		}
		CTraceWorkManager::instance()->reConnect();
	}
	return 0;
}




int testThreads()
{
	char str[4096];
	CTraceWorkManager::instance()->InsertHex(testData, sizeof(testData), str, sizeof(str));

	int dataLen = sizeof(testData);
	memset(testData, 'A', dataLen);
	testData[dataLen-1] = '\n';
	
	CBugKiller::startServer("127.0.0.1");
	const int threadNum = 10;
	CBase::pthread_t thread_id[threadNum];	

	for (int i=0; i<threadNum; ++i)
	{
		CBase::pthread_create(&thread_id[i], NULL,test1,NULL);
	}
	while (1)
	{
		CBase::usleep(1000*1000);
	}
	return 0;
}

