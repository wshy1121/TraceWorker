#include <string.h>
#include "trace_worker.h"
#include "trace_base.h"

char testData[16*1024];
//����CTimeCalcʹ��
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
		//CBase::usleep(1000*1000);
	}
	return NULL;
}

int main1()
{
	trace_start("127.0.0.1", 880110, "/root/nfs/Log/TraceWorkerDebug.cpp");
	{
		//trace_worker();
		fun0(5);
		
	}
	//return 0;
	for (int i=0; i<10000; ++i)
	{
		{
			trace_worker();
			fun0(5);
		}
	}
	return 0;
}




int main()
{
	int dataLen = sizeof(testData);
	memset(testData, 'A', dataLen);
	testData[dataLen-1] = '\n';
	
	trace_start("127.0.0.1", 880110, "/root/nfs/Log/TraceWorkerDebug.cpp");
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

