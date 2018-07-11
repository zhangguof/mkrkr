#-*- coding:utf-8 -*-

map_file = "SHIFTJIS.TXT"

c_template = '''
//gen by build_shift_jis_decode.py
#include "shift_jis_tables.h"
#include <unordered_map>
std::unordered_map<uint16_t,uint16_t> shift_jis_tables = {
	%s
};

//# const uint8_t shift_jis_g1_range[] = {0x20,0x7E,0x81,0x9F,0xA1,0xDF,0xE0,0xEA};
//# const uint8_t shift_jis_g2_range[] = {0x40,0x7E,0x80,0xFC};
//# const int shift_jis_g2_size = (0x7E - 0x40) + (0xFC - 0x80) + 2;
//# const int jis_p1_start = 0;
//# const int jis_p2_start = jis_p1_start + 0x7E - 0x20 + 1;
//# const int jis_p3_start = jis_p2_start + 0x9F - 0x81 + 1;
//# const int jis_p4_start = jis_p3_start + 0xDF - 0xA1 + 1;

//#const uint16_t shift_jis_tables[]={};


'''


def build_map(filename):
	# byte1 0x20~0x7F 0x81~0x9F 0xA1~0xDF 0xE0~0xEA
	# byte2 0x40~0x7E,0x80-0xFC
	g1 = [0x20,]
	g2 = [0x81,[0x40,],[0x80,]]
	g3 = [0xA1,]
	g4 = [0xE0,[0x40,],[0x80,]]
	g = []
	cnt1 = 0
	with open(filename,"rb") as f:
		lines = f.readlines()
		for l in lines:
			l = l.strip()
			if(l.startswith("#")):
				continue
			shift_bytes, code, _ = l.split("\t")
			c = int(code,16);
			b = int(shift_bytes,16)
			cnt1+=1
			b1 = b2 = 0
			if b > 0xff:
				b1 = (b>>8) #low word
				b2 = (b&0xff)
			else:
				b1 = b&0xff
			if(b1<0x80):
				#group1
				g1.append(c)
				g.append((b,c))
			elif (b1>=0x81 and b1<=0x9F):
				if (b2>=0x40 and b2<=0x7E):
					g2[1].append(c)
					g.append((b,c))
				elif (b2>=0x80 and b2<=0xFC):
					g2[2].append(c)
					g.append((b,c))
			elif(b1>=0xA1 and b1<= 0xDF):
				g3.append(c)
				g.append((b,c))
			elif (b1>=0xE0 and b1<=0xEA):
				if (b2>=0x40 and b2<=0x7E):
					g4[1].append(c)
					g.append((b,c))
				elif (b2>=0x80 and b2<=0xFC):
					g4[2].append(c)
					g.append((b,c))
	cnt_g1 = len(g1) -1
	cnt_g2 = len(g2[1]) + len(g2[2]) - 2
	cnt_g3 = len(g3) - 1
	cnt_g4 = len(g4[1]) + len(g4[2]) - 2
	print"cnt = %d,g1=%d,g2=%d,g3=%d,g4=%d"%(cnt1,cnt_g1,cnt_g2,cnt_g3,cnt_g4)
	print "all = %d,%d"%(cnt_g1+cnt_g2+cnt_g3+cnt_g4,len(g))
	return g

def write_file(fname,codes):
	# print len(codes),codes[1],type(codes[1])
	with open(fname,"wb") as f:
		codes = ["{%s,%s},"%(hex(x[0]),hex(x[1])) for x in codes]
		i = 0
		while(i<len(codes)):
			i+=1
			if i%20 == 0:
				codes[i] = codes[i]+'\n'
		codes_str = "".join(codes)
		s = c_template%(codes_str)
		f.write(s)


def build_it():
	codes = build_map(map_file)
	write_file("shift_jis_tables.cpp",codes)


if __name__ == '__main__':
	build_it()
	