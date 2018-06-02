#ifndef _IMG_BUF_H_
#define _IMG_BUF_H_

class ImgBuf
{
public:
	int w,h;
	void* buf;
	bool is_rgba;
	int format_size;
	ImgBuf(){}
	ImgBuf(void *b,int _w,int _h,bool _rgba, int _size);
	~ImgBuf();

};


#endif