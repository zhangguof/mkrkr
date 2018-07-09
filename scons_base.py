#-*- coding:utf-8 -*-
import os
frameworks = ['CoreAudio','OpenGL',
			  'AudioToolbox','ForceFeedback','Carbon',
			  'CoreFoundation','Cocoa','IOKit','CoreVideo',
			  'Metal','AVFoundation','CoreMedia','Security',
			  'VideoToolbox',
			  ]

cpp_path = ['./',
			'src',
			'src/objects','src/utils',
			'src/base','src/visual',
			'src/visual/sdl',
			'src/sound',
			'src/environ',
			'src/extension',
			'src/sdl',
			'src/movie',
			'src/core',
]

refuse_files = set([
	"test.cpp",
	# "un_pack_xp3.cpp",
	"src/sdl/test.cpp",
	'src/sound/WaveImpl.cpp',
	'src/core/test.cpp'
	])

# include_path = list(cpp_path)
include_path = list([
	'src/tjs2',
	'src/tjs2/base',
	'src/tjs2/msg',
	'src/tjs2/utils',
	'../boost_1_67_0/',
	'src/ext_libs_src/',
	'../sdl/SDL2-2.0.8/include',
	'../ffmpeg-4.0/',
	'src/ext_libs_src/freetype-2.9/include',
	'src/ext_libs_src/libpng-1.6.34',
	'src/ext_libs_src/openal-soft-1.18.2/include',
	'src/ext_libs_src/freealut-1.1.0/include',
	"src/plugins/KAGParser",
	])

plugin_infos = {
	"base":{"path":['src/plugins/'],},
	"layerexsave":{
		"path":['src/plugins/layerexsave',
				'src/plugins/layerexsave/LodePNG'],
		"include":['src/plugins','src/plugins/ncbind'],
		'refuse_files':['src/plugins/layerexsave/savetlg5.cpp',]
	},
	"kagparser":{
		"path":['src/plugins/KAGParser'],
	},
	"menu":{
		"path":['src/plugins/menu'],

	},
}

def search_cpp_src(paths,root_path=""):
	cpp_srcs = []
	for cpp_path in paths:
		if(root_path):
			cpp_path = os.path.join(root_path,cpp_path)
		for root,_,files in os.walk(cpp_path):
			for fname in files:
				if fname.endswith(".cpp") or fname.endswith(".c"):
					fpath = os.path.join(root,fname)
					base_path = os.path.relpath(fpath,root_path)
					if base_path in refuse_files:
						continue
					cpp_srcs.append(fpath)
			break
	return cpp_srcs

#add plugins
def search_plugins(infos,out_cpp_path,out_include_path,out_refuse_files):
	for plugin_name,item in infos.items():
		if item.get("path"):
			out_cpp_path.extend(item['path'])
		if item.get('include'):
			out_include_path.extend(item['include'])
		if item.get('refuse_files'):
			for f in item['refuse_files']:
				out_refuse_files.add(f);



def get_cpp_src(root_path=""):
	search_plugins(plugin_infos,cpp_path,include_path,refuse_files)
	cpp_src = search_cpp_src(cpp_path,root_path)
	return cpp_src

def get_include_paths(root_path=""):
	include_path.extend(cpp_path)
	paths = include_path
	if(root_path):
		paths = [os.path.join(root_path,p) for p in include_path]
	print "include----",paths
	return paths

