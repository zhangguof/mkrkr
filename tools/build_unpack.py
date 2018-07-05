import os,sys
from itertools import chain

# sys.path.append("../")
# import scons_base

arch_target = "x86"
target = 'unpack'

root_path  = "../"

#using in mac
frameworks = []

#path

# cpp_path = scons_base.cpp_path
# cpp_path.append("tools/")

# refuse_files = scons_base.refuse_files
# refuse_files.add("src/core/main.c")
# refuse_files.add("main.cpp")
# refuse_files.add("un_pack_xp3.cpp")


cpp_path = []
#src files.
# cpp_src = scons_base.get_cpp_src(root_path)
cpp_src = [
	"unpack.cpp",
	"Archive.cpp",
	"XP3Archive.cpp",
	"localstream.cpp",
	"TextStream.cpp",
	# "../src/base/BinaryStream.cpp",
	# "../src/base/XP3Archive.cpp",
	# "../src/base/StorageIntf.cpp",
	# "../src/base/StorageImpl.cpp"
	]

# include_path = scons_base.get_include_paths(root_path)
include_path = ["./",
				"../src/tjs2",
				"../src/tjs2/msg",
				"../src/tjs2/base",
				"../../boost_1_67_0/",
				]



env = Environment(TARGET_ARCH=arch_target,CPPDEFINES=[])

env.Append( CPPPATH=list(chain(include_path,cpp_path)) )

platform = env['PLATFORM']
print "build in platform:",platform



def build_main_in_darwin():
	# link_flags = " ".join(["-framework %s"%s for s in frameworks])
	link_flags = " "
	cc_flags = " -std=c++0x -g -DTVP_LOG_TO_COMMANDLINE_CONSOLE -DTVP_NO_CHECK_WIDE_CHAR_SIZE"
	libs = ['tjs2','onig','z',
	'boost_filesystem','boost_system'
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