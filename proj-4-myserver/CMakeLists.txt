cmake_minimum_required(VERSION 2.6)

project(main)


set(CXX_FLAGS -g -std=c++11 -Wall -pthread)
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")

set(CMAKE_SUPPRESS_REGENERATION 1)
set(CMAKE_USE_RELATIVE_PATHS ON)

set(SRC
    src/main.cpp
    src/server.cpp
    src/client.cpp
    src/log.cpp
)

add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ./include
        ../network/include
        ../depends/libevent/include
)

find_library(netbase_lib libnetbase.a ./lib)
find_library(event_lib libevent.so ../depends/libevent/build/lib)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE
        ${netbase_lib}
        ${event_lib}
)

# output all the linked libraries
get_target_property(OUT ${PROJECT_NAME} LINK_LIBRARIES)
    message("[info] all the linked libraries are: ${OUT}")
