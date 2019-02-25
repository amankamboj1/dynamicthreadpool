cmake_minimum_required (VERSION 3.12)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(__CurTargetName__ dynamicthreadpool)

set(SOURCE_FILES 
  dynamic_thread_pool.h
  dynamic_thread_pool.cpp
)

add_library(${__CurTargetName__} STATIC
	${SOURCE_FILES}
)