#ifndef __LINK_TOOL_H
#define __LINK_TOOL_H
#include <map>
#include <stddef.h>
#include <string>
#include <stdlib.h>
#include "trace_base.h"
#include "mem_base.h"

#define INF_SIZE 16
#define TQueueContain(x) container_of((x), ThreadNode, node)
#define each_link_node(head, node) for ((node)=(head)->next; (head) != (node); (node)=(node)->next)
#define TRACE_INF_LEN  512

typedef struct node
{
    struct node *next;
	struct node *pre;
}node;

void init_node(struct node *node);
void insert_node(struct node *node, struct node *inser_node);
void remov_node(struct node *node);
namespace base
{

class  CList
{
public:
	static CList *createCList();
	static void destroyClist(CList *pCList);
public:
	int push_back(node *pNode);
	node *begin();
	node *back();
	void pop_front();
	void pop_back();
	bool empty();
	void clear();
	size_t size();
	node *getHead();
	node *find(node *cmpNode, bool (*cmpFuc)(node *node1, node *node2));
	node *erase(node *pNode);
private:
	void init();
	void exit();
public:
	node head_node;
	node *tail;
	int node_num;
};


typedef struct CStrNode
{
public:
	static CStrNode *createCStrNode(size_t maxStrLen);
	static void destroyCStrNode(CStrNode *pNode);
public:
	node *getNode();
	size_t size();
	void setStr(char *str, size_t strLen = 0);
	char *getStr();
	size_t writeStr(char *str);
private:
	void init(size_t maxStrLen);
	void exit();
public:
	struct node m_node;
	char *m_str;
	size_t m_strLen;
	size_t m_remainMem;
}CStrNode;
#define TStrNodeContain(x) container_of((x), CStrNode, m_node)

struct CString
{
public:
	static CString* createCString();
	static void  destroyCString(CString *pCString);
public:	
	void append(char *str);
	void append(const char *str);
	char *c_str();
	size_t size();
	void clear();
private:
	void init();
	void exit();
private:
	CList *m_pStrList;
	CStrNode *m_lastStrNode;
	size_t m_strLen;
	size_t m_maxStrNodeLen;
};


class CPthreadMutex
{
public:
	///\brief 构造函数，默认为互斥锁
	CPthreadMutex()
	{
		m_mutex = (CBase::pthread_mutex_t *)base::malloc(sizeof(CBase::pthread_mutex_t));
		CBase::pthread_mutex_init(m_mutex, NULL);
	}

	///\brief 析构函数
	~CPthreadMutex()
	{
		base::free(m_mutex);
	}

	///\brief 占用锁
	bool Enter()
	{
		CBase::pthread_mutex_lock(m_mutex);
		return true;
	}

	///\brief 释放锁
	bool Leave()
	{
		CBase::pthread_mutex_unlock(m_mutex);
		return true;
	}

private:
	CBase::pthread_mutex_t  *m_mutex;
};

class CGuardMutex
{
public:
	///\brief 构造函数
	inline CGuardMutex(CPthreadMutex& mutex)
		:m_mutex(mutex)
	{
		m_mutex.Enter();
	};

	///\brief 析构函数
	inline ~CGuardMutex()
	{
		m_mutex.Leave();
	};
private:
	CPthreadMutex &m_mutex;
};


typedef enum
{
	e_Mem, 
	e_TimeCalc, 
	e_ThreadEnableNum,
}E_ENABLE_TYPE;

typedef struct 
{
	CBase::pthread_t thread_id;
	bool enable[e_ThreadEnableNum];
	struct node node;
}ThreadNode;

}




#endif


