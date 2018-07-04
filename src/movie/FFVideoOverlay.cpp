#include "FFVideoOverlay.hpp"

//read call back frome stream.

static int stream_read_packet_cb(void* opaque, uint8_t* buf, int buf_size)
{
	tTJSBinaryStream* p_stream  = static_cast<tTJSBinaryStream* >(opaque);
	buf_size = p_stream->Read(buf, buf_size);
	// printf("read cb:%d\n",buf_size);
	if(!buf_size)
		return AVERROR_EOF;
	return buf_size;
}

//FFVideoOverlay
FFVideoOverlay::FFVideoOverlay()
{
	p_ffstream = nullptr;
	p_vplayer = nullptr;
	event_queue = nullptr;
	ev_events = 0;
	ev_event_map[EC_COMPLETE] = 0;
	ev_event_map[EC_UPDATE] = 0;

}

void FFVideoOverlay::open(tTJSBinaryStream *stream, 
		const wchar_t * streamname,
		const wchar_t *type, uint64_t size)
{
	p_ffstream = std::make_shared<ffStream>();
	p_ffstream->open_audio_stream_cb((void*)stream, 
				stream_read_packet_cb);
	p_vplayer = std::make_shared<VideoPlayer>();
	p_vplayer->open(p_ffstream);
	stream_name = streamname;
	SDL_Log("===FFVideoOverlay::open:%ls\n",stream_name.c_str());

}

void FFVideoOverlay::Play()
{
	SDL_Log("FFVideoOverlay:Play:%ls",stream_name.c_str());
	if(p_vplayer)
	{
		p_vplayer->play();
		// push_graphnotify_event();
		// update(0);
	}
}
void FFVideoOverlay::Stop()
{
	if(p_vplayer)
	{
		p_vplayer->stop();
	}
}
void FFVideoOverlay::Pause()
{
	if(p_vplayer)
	{
		p_vplayer->pause();
	}
}

void FFVideoOverlay::SetPosition(uint64_t tick) {

}
void FFVideoOverlay::GetPosition(uint64_t *tick) {

}
void FFVideoOverlay::GetStatus(tTVPVideoStatus *_status) {
	*_status = status;

}


void FFVideoOverlay::Rewind() 
{
	if(p_vplayer)
	{
		p_vplayer->rewind();
	}
}
void FFVideoOverlay::SetFrame( int f ) 
{
	p_vplayer->SetFrame(f);
}
void FFVideoOverlay::GetFrame( int *f ) 
{
	*f = p_vplayer->GetFrame();
}
void FFVideoOverlay::GetFPS( double *f ) 
{
	*f = p_vplayer->fps;
}
void FFVideoOverlay::GetNumberOfFrame( int *f ) 
{
	*f = (int) p_vplayer->total_frames;
}
void FFVideoOverlay::GetTotalTime( int64_t *t ) 
{
	*t = (int64_t) p_vplayer->total_time;
}


void FFVideoOverlay::GetVideoSize( long *width, long *height ) 
{
	*width = p_vplayer->width;
	*height = p_vplayer->height;
}
void FFVideoOverlay::GetFrontBuffer( uint8_t **buff ) 
{
	*buff = p_vplayer->front_buffer;
}
void FFVideoOverlay::SetVideoBuffer( uint8_t *buff1, uint8_t *buff2, long size ) 
{
	SDL_Log("SetVideoBuffer!!:size:%d",size);
	p_vplayer->set_video_buffer(buff1,buff2,size);

}


void FFVideoOverlay::SetStopFrame( int frame ) 
{
	StopFrame = frame;
}
void FFVideoOverlay::GetStopFrame( int *frame ) 
{
	*frame = StopFrame;
}
void FFVideoOverlay::SetDefaultStopFrame() 
{
	// StopFrame = -1;
}


void FFVideoOverlay::SetPlayRate( double rate ) 
{

}
void FFVideoOverlay::GetPlayRate( double *rate ) 
{

}

void FFVideoOverlay::GetNumberOfVideoStream( unsigned long *streamCount )
{

}
void FFVideoOverlay::SelectVideoStream( unsigned long num )
{

}
void FFVideoOverlay::GetEnableVideoStreamNum( long *num )
{

}

void FFVideoOverlay::PresentVideoImage()
{
	
}

void FFVideoOverlay::GetEvent(long *evcode, void **param1,void **param2, bool *got)
{
	// if(ev_events<=0)
	// {
	// 	*got = false;
	// 	return;
	// }
	*got = false;

	if(ev_event_map[EC_UPDATE]>0 && p_vplayer->status == Playing)
	{
		*evcode = EC_UPDATE;
		// *p1 = 
		GetFrame(&_cur_frame);
		*param1 = &_cur_frame;
		*got = true;
		// ev_events--;
		ev_event_map[EC_UPDATE]--;
	}
	else if(ev_event_map[EC_COMPLETE]>0 && p_vplayer->status == Complete) //read end.
	{
		SDL_Log("on Complete!!!!");
		*evcode = EC_COMPLETE;
		
		*got = true;
		// ev_events--;
		ev_event_map[EC_COMPLETE]--;
	}

}

void FFVideoOverlay::FreeEventParams(long evcode, void* param1, void* param2)
{

}

void FFVideoOverlay::push_graphnotify_event(int code)
{
	// SDL_Log("push_graphnotify_event!!:%d",ev_events);
	NativeEvent ev;
	// ev.code = WM_GRAPHNOTIFY;
	ev.set_code(WM_GRAPHNOTIFY);
	event_queue->PostEvent(ev);
	// ev_events++;
	ev_event_map[code]++;
}

void FFVideoOverlay::push_statechange_event(tTVPVideoStatus vs_s)
{
	NativeEvent ev;
	ev.set_code(WM_STATE_CHANGE);
	ev.param = reinterpret_cast<void*>(vs_s);
	event_queue->PostEvent(ev);
}


void FFVideoOverlay::update(uint32_t cur_tick)
{
	// printf("===FFVideoOverlay::update\n");
	int ret  =  p_vplayer->update(cur_tick);
	// printf("===FFVideoOverlay:update:%d\n",ret);
	if(ret)
	{
		push_graphnotify_event(EC_UPDATE);
		if(p_vplayer->status == Complete)
		{	
			push_graphnotify_event(EC_COMPLETE);
			push_statechange_event(vsEnded);
		}
	}


			

	// if(ret!=0)
	// 	push_graphnotify_event();
	// if(p_vplayer->status == Complete)
	// {
	// 	push_statechange_event(vsEnded);
	// }

}


//----------------------------------------------------------------------------
// typedef void (*UpdateFunc)(unsigned int interval);

iTVPVideoOverlay* g_curVideoOverlay = nullptr;

void VideoPlayer_Loop(uint32_t cur_tick)
{
	if(g_curVideoOverlay)
		((FFVideoOverlay*)g_curVideoOverlay)->update(cur_tick);
}

// static bool regist_vidoe_update  = false;

void GetVideoLayerObject(
	void* event_queue, tTJSBinaryStream *stream, const wchar_t * streamname,
	const wchar_t *type, uint64_t size, iTVPVideoOverlay **out)
{
	auto p = new FFVideoOverlay;
	p->set_owner(event_queue);
	*out = p;
	

	if( *out )
		static_cast<FFVideoOverlay*>(*out)->open(stream, streamname, type, size );
	//regist update
	// if(!regist_vidoe_update)
	// {
	// 	regist_update(VideoPlayer_Loop);
	// 	regist_vidoe_update = true;
	// }
		
		// regist_objupdate((UpdateObj*)p);
	g_curVideoOverlay = p;

}
