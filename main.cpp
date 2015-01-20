#include "trace_worke.h"
#include <unistd.h>

char testData[16*1024];
//²âÊÔCTimeCalcÊ¹ÓÃ
void fun1()
{	trace_worker();
	trace_printf(testData);	
	trace_printf("get_stack  %s", "stack.c_str()");
	trace_str(testData, sizeof(testData));
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
	trace_worker();
	fun0(100);
	return NULL;
}

int main()
{
	char str[4096];
	CTraceWorkManager::instance()->InsertHex(testData, sizeof(testData), str, sizeof(str));

	return 0;
	int dataLen = sizeof(testData);
	memset(testData, 'A', dataLen);
	testData[dataLen-1] = '\n';
	
	CBugKiller::startServer("10.6.5.63");
	const int threadNum = 10;
	CBase::pthread_t thread_id[threadNum];	
	while (1)
	{
		for (int i=0; i<threadNum; ++i)
		{
			::pthread_create(&thread_id[i], NULL,test1,NULL);
		}
		//-------------------------------------------------------------
		for (int i=0; i<threadNum; ++i)
		{
			::pthread_join(thread_id[i], NULL);
		}
		CBase::usleep(1000*1000);
	}
	return 0;
}

