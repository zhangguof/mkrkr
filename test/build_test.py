import os,sys
from itertools import chain

sys.path.append("../")
import scons_base

arch_target = "x86"
target = 'test'

root_path  = "../"

#using in mac
frameworks = scons_base.frameworks
frameworks.append("opencv2");

#path

cpp_path = scons_base.cpp_path
cpp_path.append("test")

refuse_files = scons_base.refuse_files
refuse_files.add("src/core/main.c")
refuse_files.add("main.cpp")
refuse_files.add("un_pack_xp3.cpp")
refuse_files.add("test/test_thread.cpp");
refuse_files.add("test/test_timer.cpp");
refuse_files.add("test/test_tjs_script.cpp");

print "====",cpp_path

# cpp_path = []
#src files.
cpp_src = scons_base.get_cpp_src(root_path)
# cpp_src = [
# 	"unpack.cpp",
# 	"Archive.cpp",
# 	"XP3Archive.cpp",
# 	"localstream.cpp",
# 	"TextStream.cpp",
# 	# "../src/base/BinaryStream.cpp",
# 	# "../src/base/XP3Archive.cpp",
# 	# "../src/base/StorageIntf.cpp",
# 	# "../src/base/StorageImpl.cpp"
# 	]

include_path = scons_base.get_include_paths(root_path)
# include_path = ["./",
# 				"../src/tjs2",
# 				"../src/tjs2/msg",
# 				"../src/tjs2/base",
# 				"../../boost_1_67_0/",
# 				]
include_path.append("../thirdparty/include");
print "==src==",cpp_src



env = Environment(TARGET_ARCH=arch_target,CPPDEFINES=[])

env.Append( CPPPATH=include_path )

platform = env['PLATFORM']
print "build in platform:",platform



def build_main_in_darwin():
	link_flags = " ".join(["-framework %s"%s for s in frameworks])
	link_flags += " -F ../libs"
	cc_flags = " -std=c++0x -g -DTVP_LOG_TO_COMMANDLINE_CONSOLE -DTVP_NO_CHECK_WIDE_CHAR_SIZE"
	libs = ['tjs2','onig','z',
	'boost_filesystem','boost_system',
	'SDL2','GLEW','soil',
	'freetype','bz2',
	'png16','iconv','lzma',
	'avformat','avcodec','avutil','swscale','swresample',
	'openal',
	]
	lib_path = ['../libs',]
	env.Program(target=target, source=cpp_src,
		CC='c++', 
		CXX='clang++',
		CCFLAGS=cc_flags, LINKFLAGS=link_flags,
		LIBS = libs, LIBPATH = lib_path)


def build_main():
	build_main_in_darwin()


build_main()