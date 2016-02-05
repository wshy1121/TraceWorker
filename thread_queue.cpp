#include "thread_queue.h"

using namespace base;

CPthreadMutex g_insMutexCalc;

///\brief 构造函数
CGuardEnable::CGuardEnable(E_ENABLE_TYPE type)
	:m_type(type)
{
	if (!ThreadQueue::getEnable())
	{
		return ;
	}
	m_queueNode = ThreadQueue::instance()->getQueueNode(CBase::pthread_self());
	m_needReturn = !m_queueNode->enable[m_type];
}

CGuardEnable::~CGuardEnable()
{
	if (!ThreadQueue::getEnable())
	{
		return ;
	}
	
	if (m_needReturn) 
	{
		return ;
	}
	
	ThreadQueue::instance()->removeNode(m_queueNode, m_type);
}

bool CGuardEnable::needReturn()
{
	if (!ThreadQueue::getEnable())
	{
		return false;
	}
	
	if (m_needReturn) 
	{
		return true;
	}

	m_queueNode->enable[m_type] = false;
	return false;
}


bool ThreadQueue::m_enable = false;
ThreadQueue *ThreadQueue::_instance = NULL;

ThreadQueue::ThreadQueue()
{
	initQueue();
	return ;
}

ThreadQueue *ThreadQueue::instance()
{
	if (NULL == _instance)
	{
		CGuardMutex guardMutex(g_insMutexCalc);
		if (NULL == _instance)
		{
			_instance = new ThreadQueue;
		}
	}
	return _instance;
}

void ThreadQueue::start()
{
	m_enable = true;
}

void ThreadQueue::stop()
{
	m_enable = false;
}

void ThreadQueue::setEnable(bool enable)
{
	m_enable = enable;
}

bool ThreadQueue::getEnable()
{
	return m_enable;
}


void ThreadQueue::initQueue()
{
	CGuardMutex guardMutex(m_mutex);
	init_node(&head_node.node);
	tail = &head_node;
	node_num = 0;
	
	return ;
}
void ThreadQueue::initThreadNode(ThreadNode *queue_node)
{
	for (int i=0; i<e_ThreadEnableNum; ++i)
	{
		queue_node->enable[i] = true;
	}
	queue_node->thread_id = CBase::pthread_self();
	return ;
}

int ThreadQueue::insertQueue(ThreadNode *queue_node)
{
	CGuardMutex guardMutex(m_mutex);
	insert_node(head_node.node.pre, &queue_node->node); 
	tail = queue_node;
	++(node_num);
	return 0;
}
int ThreadQueue::removeNode(ThreadNode *queueNode, E_ENABLE_TYPE type)
{
	CGuardMutex guardMutex(m_mutex);
	queueNode->enable[type] = true;
	
	for (int i=0; i<e_ThreadEnableNum; ++i)
	{
		if (queueNode->enable[i] == false)
		{
			return -1;
		}
	}
	if (queueNode->thread_id == CBase::pthread_self())
	{
		remov_node(&queueNode->node);
		tail = TQueueContain(head_node.node.pre);
		--(node_num);
		base::free(queueNode);
		return 0;	
	}

	return -1;
}

int ThreadQueue::removeQueue(CBase::pthread_t thread_id)
{
	CGuardMutex guardMutex(m_mutex);
	struct node *node = NULL;
	struct node *head = &head_node.node;
	ThreadNode *queue_node = NULL;

	if (node_num < 1) //无结点
	{
		return -1;
	}
	if (&head_node == tail) //无结点
	{
		return -1;
	}

	each_link_node(head, node)
	{
		queue_node = TQueueContain(node);
		if (queue_node->thread_id == thread_id)
		{
			remov_node(&queue_node->node);
			tail = TQueueContain(head_node.node.pre);
			--(node_num);
			return 0;	
		}
	}

	return -1;
}
int ThreadQueue::getQueue(CBase::pthread_t thread_id, ThreadNode **ret_queue_node)
{
	CGuardMutex guardMutex(m_mutex);
	struct node *node = NULL;
	struct node *head = &head_node.node;
	ThreadNode *queue_node = NULL;

	if (node_num < 1) //无结点
	{
		*ret_queue_node = NULL;
		return -1;
	}
	if (&head_node == tail) //无结点
	{
		*ret_queue_node = NULL;
		return -1;
	}

	each_link_node(head, node)
	{
		queue_node = TQueueContain(node);
		if (queue_node->thread_id == thread_id)
		{
			*ret_queue_node = queue_node;
			tail = TQueueContain(head_node.node.pre);
			return 0;	
		}
	}

	*ret_queue_node = NULL;
	return -1;
}


void ThreadQueue::clearQueue()
{
	CGuardMutex guardMutex(m_mutex);
	struct node *node = NULL;
	struct node *head = &head_node.node;
	ThreadNode *queue_node = NULL;

	if (node_num < 1) //无结点
	{
		return ;
	}
	if (&head_node == tail) //无结点
	{
		return ;
	}

	for ((node)=(head)->next; (head) != (node); )
	{
		queue_node = TQueueContain(node);
		(node)=(node)->next;
		//printf("queue_node->thread_id  %ld\n", queue_node->thread_id);

		base::free(queue_node);

	}


	init_node(&head_node.node);
	tail = &head_node;
	node_num = 0;

	return ;
}
void ThreadQueue::dispQueue()
{
	CGuardMutex guardMutex(m_mutex);
	struct node *node = NULL;
	struct node *head = &head_node.node;
	ThreadNode *queue_node = NULL;

	if (node_num < 1) //无结点
	{
		return ;
	}
	if (&head_node == tail) //无结点
	{
		return ;
	}

	each_link_node(head, node)
	{
		queue_node = TQueueContain(node);
		printf("queue_node->thread_id  %ld\n", queue_node->thread_id);

	}

	return ;

}
ThreadNode *ThreadQueue::getQueueNode(CBase::pthread_t thread_id)
{
	ThreadNode *queue_node = NULL;
	getQueue(CBase::pthread_self(), &queue_node);
	if (!queue_node)
	{
		queue_node = (ThreadNode *)base::malloc(sizeof(ThreadNode));
		initThreadNode(queue_node);
		insertQueue(queue_node);
	}
	return queue_node;
}
void ThreadQueue::wrapMalloc(void* addr, size_t c)
{
	threadQueueEnable(e_Mem);
#if 0
	std::string backtrace;
	//CalcMem::instance()->getBackTrace(backtrace);
	CMemCheck::instance()->addMemInfo(addr, (int)c, backtrace);
	
	MEM_DATA *pMemData =  CalcMem::instance()->createMemData((int)backtrace.size());
	CalcMemInf *pCalcMemInf = &pMemData->calcMemInf;
		
	pCalcMemInf->m_opr = CalcMemInf::e_wrapMalloc;
	pCalcMemInf->m_traceInfoId.threadId = CBase::pthread_self();
	pCalcMemInf->m_traceInfoId.clientId = -1;
	pCalcMemInf->m_memAddr = addr;
	pCalcMemInf->m_memSize= c;
	base::strcpy(pCalcMemInf->m_backTrace, backtrace.c_str());

	CalcMemManager::instance()->pushMemData(pMemData);
#endif
	return ;
}

void ThreadQueue::wrapFree(void* addr)
{
	threadQueueEnable(e_Mem);
#if 0
	MemNodeInf nodeInf;
	if (CMemCheck::instance()->getMemNodeInf(addr, nodeInf) == false)
	{
		return ;
	}
	
	MEM_DATA *pMemData =  CalcMem::instance()->createMemData((int)nodeInf.path.size());
	CalcMemInf *pCalcMemInf = &pMemData->calcMemInf;

	pCalcMemInf->m_opr = CalcMemInf::e_wrapFree;
	pCalcMemInf->m_traceInfoId.threadId = CBase::pthread_self();
	pCalcMemInf->m_traceInfoId.clientId = -1;
	pCalcMemInf->m_memAddr = addr;
	pCalcMemInf->m_memSize= nodeInf.memSize;
	base::strcpy(pCalcMemInf->m_backTrace, nodeInf.path.c_str());
	
	CalcMemManager::instance()->pushMemData(pMemData);
#endif
	return ;
}

