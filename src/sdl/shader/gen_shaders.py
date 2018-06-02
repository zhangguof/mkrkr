#-*- coding:utf-8 -*-

vs_shader = "simple_vs.glsl"
fs_shader = "simple_fs.glsl"
out_hpp = "../shaders.hpp"

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
	vs_str = read_shader_file(vs_shader)
	fs_str = read_shader_file(fs_shader)
	vs = 'const char* simple_vs_shader="%s";'%(vs_str)
	fs = 'const char* simple_fs_shader="%s";'%(fs_str)
	var_s = "\n".join([vs,fs])
	with open(out_hpp,"wb") as f:
		f.write(template%var_s)
	

if __name__ == '__main__':
	main()