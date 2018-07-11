#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <locale>

#include "shift_jis_tables.h"
#include "tjsCommHead.h"
#include "CharacterSet.h"

// extern uint16_t get_shiftjis_code(uint8_t* &in);
void test_str()
{
	uint8_t jis_src[] = {
		'a','b','b',0x8E,0xF1,0x97,0xD6,0x0
	};
	ttstr s;
	tjs_char buf[sizeof(jis_src)/sizeof(uint8_t)];
	const char* p = (const char*)jis_src;
	int r = TVPShiftJISoWideCharString(p,
	sizeof(jis_src)/sizeof(uint8_t)-1,buf
	);
	if(r>0)
	{
		buf[r] = 0;
		s = buf;
		wprintf(TJS_W("get len:%d,%ls"),r,s.c_str());
	}

}

int main(int argc, char* argv[])
{
	 setlocale(LC_CTYPE,"UTF-8");
	 // std::locale loc("zh.jip");
	 // std::cout<<"local name:"<<loc.name()<<" "<<loc.ctype<<std::endl;
	// printf("====test!!\n");
	 uint8_t jis_src[] = {0x20,0x7E,
	 0x81,0x40,0x81,0x7E,
	 0x81,0x80,0x81,0xFC,

	 0x9F,0x40,0x9F,0x7E,
	 0x9F,0x80,0x9F,0xFC,

	 0xA1,0xDF,

	 0xE0,0x40,0xE0,0x7E,
	 0xE0,0x80,0xE0,0xFC,

	 0xEA,0x40,0xEA,0x7E,
	 0xEA,0x80,0xEA,0xFC,
	 };
	 uint16_t dst[] = {0x20,0x203E,
	 0x3000,0x00D7,0x00F7,0x25EF,
	 0x6A97,0x6BEF,0x9EBE,0x6ECC,
	 0xFF61,0xFF9F,
	 0x6F3E,0x70DD,0x70D9,0x73F1,
	 0x9D5D,0x9EEF,0x9EF4,0,
	 };
	 int n = sizeof(jis_src);
	 printf("dst:\n");
	 for(int i=0;i<sizeof(dst)/sizeof(uint16_t);++i)
	 {
	 	printf("0x%0x,",dst[i]);
	 }
	 printf("\n");
	 const char* p = (const char*)jis_src;
	 printf("get:\n");
	 wchar_t c;
	 bool r = get_shiftjis_code(p,&c);
	 while(r)
	 {
	 	wchar_t c;
	 	printf("0x%0x,",c);
	 	r = get_shiftjis_code(p,&c);
	 }
	 printf("\n");
	 test_str();
	
	return 0;
}