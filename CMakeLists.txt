#@note   Hikvision Digital Technology Co., Ltd. All Rights Reserved.
#@brief  CMake子目录配置文件
#@author Yaopei
#@date   2015/7/31
#@note   the CMakeLists.txt for src. directory


PROJECT(HMSProxy)
cmake_minimum_required(VERSION 2.8)

IF (WIN32)
    ADD_DEFINITIONS(-DWIN32)
ELSEIF (UNIX)
    #do something related to UNIX
ELSEIF(APPLE)
    #do something related to APPLE
ENDIF (WIN32)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -g -Wall -fPIC -O0")

SET(SRC_SRCS
  trace_worker.cpp
  trace_packet.cpp
  trace_base.cpp
  thread_queue.cpp
  link_tool.cpp
  wrap_malloc.cpp
  mem_base.cpp
  socket_opr.cpp
  ConvertUTF.cpp
)

# shared library
#ADD_LIBRARY(TraceWorker SHARED ${SRC_SRCS} )

#static library
ADD_LIBRARY(TraceWorker STATIC ${SRC_SRCS})

# link library
IF (WIN32)
    TARGET_LINK_LIBRARIES(TraceWorker wsock32)
ELSEIF (UNIX)
    TARGET_LINK_LIBRARIES(TraceWorker)
ENDIF (WIN32)

ADD_EXECUTABLE(test main.cpp)
# link library
TARGET_LINK_LIBRARIES(test  TraceWorker pthread)

# clean
SET_TARGET_PROPERTIES(TraceWorker PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(Decoder_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
