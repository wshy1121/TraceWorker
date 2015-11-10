#@note   Hikvision Digital Technology Co., Ltd. All Rights Reserved.
#@brief  CMake子目录配置文件
#@author Yaopei
#@date   2015/7/31
#@note   the CMakeLists.txt for src. directory


PROJECT(HMSProxy)
cmake_minimum_required(VERSION 2.8)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -g -Wall -fPIC -O0")
else()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -O2 -Wall -fPIC")
endif()

SET(SRC_SRCS
  trace_worker.cpp
)

# shared library
ADD_LIBRARY(TraceWorker SHARED ${SRC_SRCS} )

#static library
#ADD_LIBRARY(TraceWorker STATIC ${SRC_SRCS})

# link library
#TARGET_LINK_LIBRARIES(TraceWorker)

# clean
SET_TARGET_PROPERTIES(TraceWorker PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(Decoder_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
