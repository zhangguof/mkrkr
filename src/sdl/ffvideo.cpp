#include "ffvideo.hpp"
#include "GLDevice.hpp"



VideoPlayer::VideoPlayer()
{
	p_ffstream = nullptr;
	next_update_tick = 0;
	fps = 0.0f;
	width = heigh = 0;
	fill_cb = nullptr;
	status = Init;
}

VideoPlayer::open(std::string s)
{
	fname = s;
	p_ffstream = std::make_shared<ffStream>(s);
	// p_ffstream->open_
	fps = p_ffstream->get_fps();
	width = p_ffstream->get_width();
	heigh = p_ffstream->get_height();
}

void VideoPlayer::play()
{

	// if(next_update_tick == 0)
	// 	next_update_tick = time_now();
	next_update_tick = time_now();
	status = Playing;
}

void VideoPlayer::update()
{
	if(status != Playing)
		return;
	uint32_t cur = time_now();
	if(cur >= next_update_tick)
	{
		uint32_t* buf = nullptr;
		int size = pbf->read(&buf,width*heigh*format_size);
		if(size > 0)
			fill_cb(buf,size);

		next_update_tick = time_now() + (uint32_t)(1000.0/fps);
	}
}






