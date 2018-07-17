#-*- coding:utf-8 -*-

vs_shader = "simple_vs.glsl"
fs_shader = "simple_fs.glsl"
out_hpp = "../shaders.hpp"

shader_infos = [
	("simple_vs.glsl","simple_vs_shader"),
	("simple_fs.glsl","simple_fs_shader"),

	("simple_es_vs.glsl","simple_es_vs_shader"),
	("simple_es_fs.glsl","simple_es_fs_shader"),

	("object2d_vs.glsl","object2d_vs_sharder"),
	("object2d_fs.glsl","object2d_fs_sharder"),
]

template = '''
//gen by gen_shaders.py
#ifndef _SHADERS_H_
#define _SHADERS_H_
%s
#endif
'''
def read_shader_file(filename):
	lines = []
	with open(filename,"rb") as f:
		lines = f.readlines()
	lines = [line.strip() for line in lines]
	s = "\\n\\\n".join(lines)
	return s

def main():
	shaders = []
	for info in shader_infos:
		file_name, var_name = info
		read_s = read_shader_file(file_name)
		new_s = 'const char* const %s="%s";'%(var_name,read_s)
		shaders.append(new_s)

	# vs_str = read_shader_file(vs_shader)
	# fs_str = read_shader_file(fs_shader)
	# vs = 'const char* simple_vs_shader="%s";'%(vs_str)
	# fs = 'const char* simple_fs_shader="%s";'%(fs_str)
	var_s = "\n".join(shaders)
	with open(out_hpp,"wb") as f:
		f.write(template%var_s)
	

if __name__ == '__main__':
	main()