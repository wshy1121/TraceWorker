#CROSS  = sh4-linux-uclibc-
#CROSS  = arm-none-linux-gnueabi-
#CROSS  = arm-linux-gnueabihf-


LIB_SRC += main.cpp 
LIB_SRC += trace_worker.cpp 

CFLAGS += -Wl,--whole-archive -lpthread -Wl,--no-whole-archive -lc -static


CPP	=	@echo " g++ $@"; $(CROSS)g++
CC	=	@echo " gcc $@"; $(CROSS)gcc
LD	=	@echo " ld  $@"; $(CROSS)ld
AR  = @echo " ar  $@"; $(CROSS)ar
STRIP	=	@echo " strip $@"; $(CROSS)strip
RANLIB = @echo " ranlib  $@"; $(CROSS)ranlib

CP	= cp -rf
RM	= rm

AFLAGS	+= -r   

TARGET=client

$(TARGET): $(LIB_SRC)
	$(CPP)  -o $(TARGET) -g $(CFLAGS) $(DEP_INC) $(LIB_SRC) $(DEP_LIBS)

clean:
	$(RM) $(LIB_OBJS) client 



