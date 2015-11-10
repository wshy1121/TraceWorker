# encoding:utf8

class NewClass(object):
    num_count = 0 # 
    def __init__(self,name):
        self.name = name
        self.__class__.num_count += 1
        print name,NewClass.num_count
    def __del__(self):
        self.__class__.num_count -= 1
        print "Del",self.name,self.__class__.num_count
    def test():
        print "aa"
def test1():
    aa = NewClass("Hello")
    bb = NewClass("World")
    cc = NewClass("aaaa")
test1()
print "Over"
