#include <stdio.h>
#include <SOIL.h>
#include "imgbuf.h"
#include "gltexture.hpp"

ImgBuf::ImgBuf(void *b,int _w,int _h,bool _rgba, int _size)
{
		buf = b;
		w = _w, h = _h;
		is_rgba = _rgba;
		format_size = _size;
	}
ImgBuf::~ImgBuf()
{
	if(buf)
	{
		SOIL_free_image_data((unsigned char *)buf);
	}
}


int load_bmp(GLTexture *tex,char* filename)
{
	int w,h;
    unsigned char* buf = SOIL_load_image(filename,&w,&h,0,SOIL_LOAD_RGBA);
    if(buf)
    {
    	printf("load bmp success!w:%d,h:%d\n",w,h);
    	// ImgBuf* img = new ImgBuf(buf, w, h, false, sizeof(unsigned char));
    	tex->set_buf(buf,w,h);
    	// SOIL_free_image_data(buf);
    	return 0;
    }
    printf("load bmp fail!!!\n");
    return -1;
}

unsigned char* fill_color(unsigned int col,int w, int h)
{
	int size = w * h * sizeof(unsigned char) * 4;
	unsigned int * buf = (unsigned int *)malloc(size);
	// unsigned char r,g,b,a;
	// r = (col & 0xFF);
	// g = ((col>>8) & 0xFF);
	// b = ((col >> 16) & 0xFF);
	// a = (col >> 24);
	for(int i=0;i< w*h;++i)
	{
		buf[i] = col;
	}
	return (unsigned char *) buf;

}