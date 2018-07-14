#-*- coding:utf-8 -*-
import chardet
src_path = "data/"

import codecs
import os

def decode(src_path):
	for root,_,files in os.walk(src_path):
		for fname in files:
			fpath = os.path.join(root,fname)
			new_s = ""
			if not (fname.endswith(".hpp") or fname.endswith(".cpp") or
			   fname.endswith(".tjs") or fname.endswith(".txt") or 
			   fname.endswith(".ks") or fname.endswith(".ttd") or
			   fname.endswith(".h")):
				continue
			with open(fpath,"rb") as f:
				s = f.read()
				en = chardet.detect(s);
				print "fname:%s"%fname,en
				if en['encoding'] == 'SHIFT_JIS':
					new_s = s.decode("SHIFT_JIS").encode("utf-8")
			if new_s:
				with open(fpath,"wb") as f:
					f.write(new_s)
					print "do decode:%s:(%s->%s)"%(fpath,en['encoding'],'utf-8')
				new_s = ""


def change(src_path):
	for root,_,files in os.walk(src_path):
		for fname in files:
			fpath = os.path.join(root,fname)
			new_s = ""
			with open(fpath,"rb") as f:
				s = f.read()
				if s.startswith(codecs.BOM_UTF8):
					print "has with bom:%s"%fpath
					continue
				new_s = codecs.BOM_UTF8 + s
			if new_s:
				with open(fpath,"wb") as f:
					f.write(new_s)
					print "change to bom:%s"%fpath
				new_s = ""
#add 
def add_end(src_path):
	for root,_,files in os.walk(src_path):
		for fname in files:
			fpath = os.path.join(root,fname)
			new_s = ""
			if not (fname.endswith(".ks") or fname.endswith(".tjs")):
				continue
			with open(fpath,"rb") as f:
				s = f.read()
				en = chardet.detect(s);
				print "fname:%s"%fname,en
				if en['encoding'] == 'UTF-16':
					# new_s = s.decode("SHIFT_JIS").encode("utf-8")
					new_s = s + ' \x00'
			if new_s:
				with open(fpath,"wb") as f:
					f.write(new_s)
					print "do add_end:%s:(%s)"%(fpath,en['encoding'])
				new_s = ""


if __name__=="__main__":
	# change(src_path)
	import sys
	argv = sys.argv
	src = src_path
	if len(argv) > 1:
		src = argv[1]
	print "decode:",src
	# decode(src)
	add_end(src)
