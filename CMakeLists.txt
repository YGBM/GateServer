cmake_minimum_required(VERSION 3.14)
project(GateServer)

set(CMAKE_CXX_STANDARD 17)
# 2.下载的外部库
## 外部库的头文件
set(ZLIB_INCLUDE ./include)
## 外部库的lib文件（静态编译）
set(ZLIB_LIBRARIES ./lib)
add_subdirectory(fmt)
add_subdirectory(hiredis)

include_directories(${ZLIB_INCLUDE})

link_directories(${ZLIB_LIBRARIES})

add_executable(GateServer main.cpp redisMgr.cpp redisMgr.hpp
        mysqlMgr.hpp mysqlMgr.cpp mysqlDao.hpp mysqlDao.cpp
        )

target_link_libraries(GateServer libhv.so fmt hiredis libmysqlclient.so)
target_include_directories(GateServer PUBLIC rapidjson)