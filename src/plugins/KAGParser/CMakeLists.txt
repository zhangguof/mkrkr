
cmake_minimum_required(VERSION 3.4.1)

# set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/version_script.map -Wl,--strip-debug -Wl,--discard-all" )
set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}-Wl" )

include_directories( ./
             ../win32/
             ../../tjs2/
             ../../utils/
             )

# add_library( kagparser

#              SHARED

#              Main.cpp
#              KAGParser.cpp
#              ../win32/tp_stub.cpp)

add_library( kagparser

             STATIC

             Main.cpp
             KAGParser.cpp
             ../win32/tp_stub.cpp)

target_link_libraries( kagparser )

