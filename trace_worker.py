# encoding:utf8
import ctypes
import sys
traceWorkerDll = ctypes.CDLL('build/libTraceWorker.so')

class NewClass(object):
    num_count = 0 # 
    def __init__(self, level = 100):
        line = sys._getframe().f_back.f_lineno
        file_name = sys._getframe().f_back.f_code.co_filename
        func_name = sys._getframe().f_back.f_code.co_name
        self.handle = traceWorkerDll.createCandy(line, file_name, func_name, level)

    def __del__(self):
        traceWorkerDll.destroyCandy(self.handle)

def printf(fmt, *arg):  
    print fmt % arg  

printf("How do you do? %s, %d", "123", 3)

import time


def test1():
    aa = NewClass()
    
if __name__ == '__main__': 
    traceWorkerDll.startServer("127.0.0.1", 8889, "./Debug.cpp")
    
    test1()
    time.sleep(1)

