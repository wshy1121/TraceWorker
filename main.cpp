#include "trace_worke.h"

int main()
{
	CBugKiller::startServer("10.6.5.63");
	//trace_printf("NULL");
	trace_worker();
}

