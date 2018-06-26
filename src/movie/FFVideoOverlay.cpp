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

}

void FFVideoOverlay::Play()
{
	SDL_Log("FFVideoOverlay:Play");
	if(p_vplayer)
	{
		p_vplayer->play();
		push_update_event();
	}
}
void FFVideoOverlay::Stop()
{

}
void FFVideoOverlay::Pause()
{
	
}

void FFVideoOverlay::SetPosition(uint64_t tick) {

}
void FFVideoOverlay::GetPosition(uint64_t *tick) {

}
void FFVideoOverlay::GetStatus(tTVPVideoStatus *status) {

}


void FFVideoOverlay::Rewind() 
{

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

}
void FFVideoOverlay::GetTotalTime( int64_t *t ) 
{

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

}
void FFVideoOverlay::GetStopFrame( int *frame ) 
{

}
void FFVideoOverlay::SetDefaultStopFrame() 
{

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
	if(ev_events<=0)
	{
		*got = false;
		return;
	}

	if(p_vplayer->status == Playing)
	{
		*evcode = EC_UPDATE;
		// *p1 = 
		GetFrame(&_cur_frame);
		*param1 = &_cur_frame;
		*got = true;
		ev_events--;
	}
	else if(p_vplayer->status == Stoped) //read end.
	{
		*evcode = EC_COMPLETE;
		ev_events--;
	}

}

void FFVideoOverlay::FreeEventParams(long evcode, void* param1, void* param2)
{

}

void FFVideoOverlay::push_update_event()
{
	NativeEvent ev;
	// ev.code = WM_GRAPHNOTIFY;
	ev.set_code(WM_GRAPHNOTIFY);
	event_queue->PostEvent(ev);
	ev_events++;
}



//----------------------------------------------------------------------------
void GetVideoLayerObject(
	void* event_queue, tTJSBinaryStream *stream, const wchar_t * streamname,
	const wchar_t *type, uint64_t size, iTVPVideoOverlay **out)
{
	auto p = new FFVideoOverlay;
	p->set_owner(event_queue);
	*out = p;
	

	if( *out )
		static_cast<FFVideoOverlay*>(*out)->open(stream, streamname, type, size );
}
