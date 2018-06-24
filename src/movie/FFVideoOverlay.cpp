#include "FFVideooverlay.hpp"

//FFvideoOverlay

void FFVideoOverlay::Play()
{

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

}
void FFVideoOverlay::GetFrame( int *f ) 
{

}
void FFVideoOverlay::GetFPS( double *f ) 
{

}
void FFVideoOverlay::GetNumberOfFrame( int *f ) 
{

}
void FFVideoOverlay::GetTotalTime( int64_t *t ) 
{

}


void FFVideoOverlay::GetVideoSize( long *width, long *height ) 
{

}
void FFVideoOverlay::GetFrontBuffer( uint8_t **buff ) 
{

}
void FFVideoOverlay::SetVideoBuffer( uint8_t *buff1, uint8_t *buff2, long size ) 
{

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

}

void FFVideoOverlay::FreeEventParams(long evcode, void* param1, void* param2)
{

}



//----------------------------------------------------------------------------
void GetVideoLayerObject(
	void* callbackwin, tTJSBinaryStream *stream, const wchar_t * streamname,
	const wchar_t *type, uint64_t size, iTVPVideoOverlay **out)
{
	*out = new FFVideoOverlay;

	// if( *out )
		// static_cast<tTVPDSLayerVideo*>(*out)->BuildGraph( callbackwin, stream, streamname, type, size );
}
