# encoding:utf8
import ctypes
import sys
traceWorkerDll = ctypes.CDLL('../TraceWorker/build/libTraceWorker.so')

class trace_worker(object):
    num_count = 0 # 
    def __init__(self, level = 100):
        line = sys._getframe().f_back.f_lineno
        file_name = sys._getframe().f_back.f_code.co_filename
        func_name = sys._getframe().f_back.f_code.co_name
        self.handle = traceWorkerDll.createCandy(line, file_name, func_name, level)

    def __del__(self):
        traceWorkerDll.destroyCandy(self.handle)

def trace_start(sip, sport, fileName):
    traceWorkerDll.startServer(sip, sport, fileName)

def trace_printf(fmt, *arg):  
    line = sys._getframe().f_back.f_lineno
    file_name = sys._getframe().f_back.f_code.co_filename
    traceWorkerDll.InsertTrace(line, file_name, "%s", fmt % arg) 


def test1():
    trace=trace_worker()
    trace_printf("%d", 100);
    trace_printf("How do you do? %s, %d", "123", 3)

if __name__ == '__main__': 
    trace_start("127.0.0.1", 8889, "./Debug.cpp")
    test1()


