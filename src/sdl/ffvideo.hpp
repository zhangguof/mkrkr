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
typedef void (*FILL_BUFFER_FUNC)(uint8_t* buf,int size);

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
	int heigh;
	VideoStatus status;


	VideoPlayer();
	void play();
	void open(std::string s);
	void update();
	void set_cb(FILL_BUFFER_FUNC cb)
	{
		fill_cb = cb;
	}


};







#endif