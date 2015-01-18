#include "trace_worke.h"
#include <unistd.h>
int main()
{
	CBugKiller::startServer("10.6.5.63");
	trace_worker();
	sleep(1);
}

