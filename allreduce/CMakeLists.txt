cmake_minimum_required (VERSION 3.5)
project (allreduce)

set (CMAKE_CXX_STANDARD 11)

include_directories(${vwcommon_INCLUDE_DIRS})

add_library(${PROJECT_NAME} allreduce_sockets.cc allreduce_threads.cc)

if(WIN32)
	set(ADDITIONAL_LIBRARY Ws2_32.lib)	
endif()

target_link_libraries(allreduce PRIVATE vwcommon ${ADDITIONAL_LIBRARY})

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
