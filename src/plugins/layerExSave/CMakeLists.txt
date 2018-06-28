
cmake_minimum_required(VERSION 3.4.1)

# set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/version_script.map -Wl,--strip-debug -Wl,--discard-all" )
set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}-Wl" )
add_definitions("-Wall -g")
add_definitions("-Wall -Wignored-attributes")

include_directories( ./
			./LodePNG
			../
             ../win32/
             ../../tjs2/
             ../../utils/
             ../../ext_libs_src/
             ../ncbind/
             )

# add_library( kagparser

#              SHARED

#              Main.cpp
#              KAGParser.cpp
#              ../win32/tp_stub.cpp)

add_library( layerexsave

             STATIC

             Main.cpp
             savepng.cpp
             # savetlg5.cpp
             utils.cpp
             ./LodePNG/lodepng.cpp
             ../win32/tp_stub.cpp)

target_link_libraries( layerexsave )

