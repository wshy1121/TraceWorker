#ifndef __THREAD_QUEUE_H__
#define __THREAD_QUEUE_H__
#include "link_tool.h"


class  ThreadQueue
{
public:
	ThreadQueue();
public:
	void initThreadNode(base::ThreadNode *queue_node);
	void initQueue();
	int insertQueue(base::ThreadNode *queue_node);
	int removeQueue(CBase::pthread_t thread_id);
	int removeNode(base::ThreadNode *queueNode, base::E_ENABLE_TYPE m_type);
	int getQueue(CBase::pthread_t thread_id, base::ThreadNode **ret_queue_node);
	void clearQueue();
	void dispQueue();
	void setEnable(bool enable);
	static bool getEnable();
	static void start();
	static void stop();
	void wrapMalloc(void* addr, size_t c);
	void wrapFree(void* addr);
	base::ThreadNode *getQueueNode(CBase::pthread_t thread_id);
	static ThreadQueue *instance();
private:
	static bool m_enable;
	static ThreadQueue *_instance;
private:
	base::ThreadNode head_node;
	base::ThreadNode *tail;
	int node_num;
	base::CPthreadMutex  m_mutex;
};

class CGuardEnable
{
public:
	///\brief 构造函数
	CGuardEnable(base::E_ENABLE_TYPE type);

	///\brief 析构函数
	~CGuardEnable();

	bool needReturn();
private:
	base::E_ENABLE_TYPE m_type;
	base::ThreadNode *m_queueNode;
	bool m_needReturn;
};


//防止嵌套调用处理
#define threadQueueEnable(type)  \
	CGuardEnable guard(type);  \
	if (guard.needReturn())  \
	{  \
		return ;  \
	}

#endif

