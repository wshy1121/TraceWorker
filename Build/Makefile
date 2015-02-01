#CROSS  = sh4-linux-uclibc-
#CROSS  = arm-none-linux-gnueabi-
#CROSS  = arm-linux-gnueabihf-

DEP_INC += -I ../LogCore -I ../CrossPlat -I ../NetApp -I ../OpenSrc 

DEP_LIBS += -L ./Libs -lLogCore -lNetApp -lCrossPlat -lOpenSrc 
LIB_SRC += ../main.cpp 
CFLAGS += -Wl,--whole-archive -lpthread -Wl,--no-whole-archive -lc -static
CFLAGS += -Wl,-wrap,malloc -Wl,-wrap,realloc -Wl,-wrap,calloc -Wl,-wrap,free

CPP	=	@echo " g++ $@"; $(CROSS)g++
CC	=	@echo " gcc $@"; $(CROSS)gcc
LD	=	@echo " ld  $@"; $(CROSS)ld
AR  = @echo " ar  $@"; $(CROSS)ar
STRIP	=	@echo " strip $@"; $(CROSS)strip
RANLIB = @echo " ranlib  $@"; $(CROSS)ranlib

CP	= cp -rf
RM	= rm

AFLAGS	+= -r   

LIB_TARGET=libLogCore.a

$(LIB_TARGET): $(LIB_SRC)
	$(CPP)  -o ../Bin/LogClient -g $(CFLAGS) $(DEP_INC) $(LIB_SRC) $(DEP_LIBS)

clean:
	$(RM) $(LIB_OBJS) ../Bin/LogClient 



