import os
from itertools import chain

import scons_base

arch_target = "x86"
target = 'unpack'

src_path  = "src"




#using in mac
frameworks = scons_base.frameworks

#path

cpp_path = scons_base.cpp_path

refuse_files = scons_base.refuse_files

include_path = scons_base.include_path

#src files.

cpp_src = scons_base.cpp_src


env = Environment(TARGET_ARCH=arch_target,CPPDEFINES=[])

env.Append( CPPPATH=list(chain(include_path,cpp_path)) )

platform = env['PLATFORM']
print "build in platform:",platform



def build_main_in_darwin():
	link_flags = " ".join(["-framework %s"%s for s in frameworks])
	cc_flags = " -std=c++0x -g -DTVP_LOG_TO_COMMANDLINE_CONSOLE -DTVP_NO_CHECK_WIDE_CHAR_SIZE"
	libs = ['tjs2','onig','z',
	'boost_filesystem','boost_system',
	'SDL2','GLEW','soil',
	'freetype','bz2','png16','iconv','lzma',
	'avformat','avcodec','avutil','swscale','swresample',
	'openal',
	]
	lib_path = ['./libs',]
	env.Program(target=target, source=cpp_src,
		CC='c++', 
		CXX='clang++',
		CCFLAGS=cc_flags, LINKFLAGS=link_flags,
		LIBS = libs, LIBPATH = lib_path)



def build_main():
	build_main_in_darwin()


build_main()