#ifndef _FF_VIDEO_VOERLAY_H_
#define _FF_VIDEO_VOERLAY_H_
#include "krmovie.h"
#include "ffStream.hpp"
#include "ffvideo.hpp"

class FFVideoOverlay:public iTVPVideoOverlay
{
public:
	WHAND win;
	tTVPVideoStatus status;

public:
	void  AddRef(){}
	void  Release(){}

	void  SetWindow(WHAND window){win = window;}
	void  SetMessageDrainWindow(WHAND window){}
	void  SetRect(RECT *rect){}
	void  SetVisible(bool b) {}

	void  Play();
	void  Stop(); 
	void  Pause();
	void  SetPosition(uint64_t tick);
	void  GetPosition(uint64_t *tick);
	void  GetStatus(tTVPVideoStatus *status);
	void  GetEvent(long *evcode, void **param1,
			void **param2, bool *got);

	void  FreeEventParams(long evcode, void* param1, void* param2);

	void  Rewind();
	void  SetFrame( int f );
	void  GetFrame( int *f );
	void  GetFPS( double *f );
	void  GetNumberOfFrame( int *f );
	void  GetTotalTime( int64_t *t );
	
	void  GetVideoSize( long *width, long *height );
	void  GetFrontBuffer( uint8_t **buff );
	void  SetVideoBuffer( uint8_t *buff1, uint8_t *buff2, long size );

	void  SetStopFrame( int frame );
	void  GetStopFrame( int *frame );
	void  SetDefaultStopFrame();

	void  SetPlayRate( double rate );
	void  GetPlayRate( double *rate );

	void  SetAudioBalance( long balance ){}
	void  GetAudioBalance( long *balance ){}
	void  SetAudioVolume( long volume ) {}
	void  GetAudioVolume( long *volume ) {}

	void  GetNumberOfAudioStream( unsigned long *streamCount ) {}
	void  SelectAudioStream( unsigned long num ) {}
	void  GetEnableAudioStreamNum( long *num ) {}
	void  DisableAudioStream( void ) {}

	void  GetNumberOfVideoStream( unsigned long *streamCount );
	void  SelectVideoStream( unsigned long num );
	void  GetEnableVideoStreamNum( long *num );

	// void  SetMixingBitmap( HDC hdc, RECT *dest, float alpha ) = 0;
	// void  ResetMixingBitmap() = 0;

	void  SetMixingMovieAlpha( float a ) {}
	void  GetMixingMovieAlpha( float *a ) {}
	void  SetMixingMovieBGColor( unsigned long col ) {}
	void  GetMixingMovieBGColor( unsigned long *col ) {}

	void  PresentVideoImage();

	void  GetContrastRangeMin( float *v ){}
	void  GetContrastRangeMax( float *v ){}
	void  GetContrastDefaultValue( float *v ){}
	void  GetContrastStepSize( float *v ){}
	void  GetContrast( float *v ){}
	void  SetContrast( float v ){}

	void  GetBrightnessRangeMin( float *v ) {}
	void  GetBrightnessRangeMax( float *v ) {}
	void  GetBrightnessDefaultValue( float *v ) {}
	void  GetBrightnessStepSize( float *v ) {}
	void  GetBrightness( float *v ) {}
	void  SetBrightness( float v ) {}

	void  GetHueRangeMin( float *v ) {}
	void  GetHueRangeMax( float *v ) {}
	void  GetHueDefaultValue( float *v ) {}
	void  GetHueStepSize( float *v ) {}
	void  GetHue( float *v ) {}
	void  SetHue( float v ) {}

	void  GetSaturationRangeMin( float *v ) {}
	void  GetSaturationRangeMax( float *v ) {}
	void  GetSaturationDefaultValue( float *v ) {}
	void  GetSaturationStepSize( float *v ) {}
	void  GetSaturation( float *v ) {}
	void  SetSaturation( float v ) {}
};




#endif