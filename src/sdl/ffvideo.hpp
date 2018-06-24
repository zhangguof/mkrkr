#ifndef _FF_VIDEO_H_
#define _FF_VIDEO_H_
#include <string>

#include "SDL.h"
#include <string>
#include <safeQueue.hpp>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/avcodec.h>  
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>


#ifdef __cplusplus
}
#endif
#include "ffStream.hpp"
extern uint32_t time_now();
typedef void (*FILL_BUFFER_FUNC)(uint8_t* buf,int w,int h,int size);

enum VideoStatus
{
	Init,
	Playing,
	Stoped,
	Paused
};

class VideoPlayer
{
public:
	std::shared_ptr<ffStream> p_ffstream;
	uint32_t next_update_tick;
	float fps;
	std::string fname;
	FILL_BUFFER_FUNC fill_cb;
	const int format_size = 4;
	int width;
	int height;
	VideoStatus status;
	// int cur_pos;
	bool is_loop;


	uint8_t* buf1;
	uint8_t* buf2;
	uint8_t* front_buffer;
	int buf_size;


	VideoPlayer();
	void play();
	void open(std::string s);
	void open(std::shared_ptr<ffStream>& fs);
	void update();

	void swap_buf(uint8_t* buf,int size);
	void set_video_buffer(uint8_t* buff1,uint8_t* buff2,int size)
	{
		buf1 = buff1;
		buf2 = buff2;
		buf_size = size;
	}

	void set_cb(FILL_BUFFER_FUNC cb)
	{
		fill_cb = cb;
	}
	void on_stop()
	{
		assert(status == Playing);
		status = Stoped;
		// cur_pos = 0;
		if(is_loop)
		{
			p_ffstream->SetFrame(0);
			status = Playing;
		}
	}
	void set_loop(bool s)
	{
		is_loop = s;
	}

	int GetFrame()
	{
		return p_ffstream->GetFrame();
	}
	void SetFrame(int f)
	{
		p_ffstream->SetFrame(f);
	}


};







#endif