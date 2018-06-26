//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// krmovie.dll ( kirikiri movie playback support DLL ) interface
//---------------------------------------------------------------------------


#ifndef __KRMOVIE_H__
#define __KRMOVIE_H__
#include "SDLWindow.hpp"
#include "sdlEvent.hpp"
#include "RectItf.h"


//---------------------------------------------------------------------------
enum tTVPVideoStatus { vsStopped, vsPlaying, vsPaused, vsProcessing, vsEnded, vsReady };
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// iTVPVideoOverlay
//---------------------------------------------------------------------------
class iTVPVideoOverlay // this is not a COM object
{
public:
	virtual void  AddRef() = 0;
	virtual void  Release() = 0;

	virtual void  SetWindow(WHAND window) = 0;
	virtual void  SetMessageDrainWindow(WHAND window) = 0;
	virtual void  SetRect(RECT *rect) = 0;
	virtual void  SetVisible(bool b) = 0;
	virtual void  Play() = 0;
	virtual void  Stop() = 0;
	virtual void  Pause() = 0;
	virtual void  SetPosition(uint64_t tick) = 0;
	virtual void  GetPosition(uint64_t *tick) = 0;
	virtual void  GetStatus(tTVPVideoStatus *status) = 0;
	virtual void  GetEvent(long *evcode, void** param1,
				  void** param2, bool *got) = 0;

	virtual void  FreeEventParams(long evcode, void* param1, void* param2) = 0;

	virtual void  Rewind() = 0;
	virtual void  SetFrame( int f ) = 0;
	virtual void  GetFrame( int *f ) = 0;
	virtual void  GetFPS( double *f ) = 0;
	virtual void  GetNumberOfFrame( int *f ) = 0;
	virtual void  GetTotalTime( int64_t *t ) = 0;
	
	virtual void  GetVideoSize( long *width, long *height ) = 0;
	virtual void  GetFrontBuffer( uint8_t **buff ) = 0;
	virtual void  SetVideoBuffer( uint8_t *buff1, uint8_t *buff2, long size ) = 0;

	virtual void  SetStopFrame( int frame ) = 0;
	virtual void  GetStopFrame( int *frame ) = 0;
	virtual void  SetDefaultStopFrame() = 0;

	virtual void  SetPlayRate( double rate ) = 0;
	virtual void  GetPlayRate( double *rate ) = 0;

	virtual void  SetAudioBalance( long balance ) = 0;
	virtual void  GetAudioBalance( long *balance ) = 0;
	virtual void  SetAudioVolume( long volume ) = 0;
	virtual void  GetAudioVolume( long *volume ) = 0;

	virtual void  GetNumberOfAudioStream( unsigned long *streamCount ) = 0;
	virtual void  SelectAudioStream( unsigned long num ) = 0;
	virtual void  GetEnableAudioStreamNum( long *num ) = 0;
	virtual void  DisableAudioStream( void ) = 0;

	virtual void  GetNumberOfVideoStream( unsigned long *streamCount ) = 0;
	virtual void  SelectVideoStream( unsigned long num ) = 0;
	virtual void  GetEnableVideoStreamNum( long *num ) = 0;

	// virtual void  SetMixingBitmap( HDC hdc, RECT *dest, float alpha ) = 0;
	// virtual void  ResetMixingBitmap() = 0;

	virtual void  SetMixingMovieAlpha( float a ) = 0;
	virtual void  GetMixingMovieAlpha( float *a ) = 0;
	virtual void  SetMixingMovieBGColor( unsigned long col ) = 0;
	virtual void  GetMixingMovieBGColor( unsigned long *col ) = 0;

	virtual void  PresentVideoImage() = 0;

	virtual void  GetContrastRangeMin( float *v ) = 0;
	virtual void  GetContrastRangeMax( float *v ) = 0;
	virtual void  GetContrastDefaultValue( float *v ) = 0;
	virtual void  GetContrastStepSize( float *v ) = 0;
	virtual void  GetContrast( float *v ) = 0;
	virtual void  SetContrast( float v ) = 0;

	virtual void  GetBrightnessRangeMin( float *v ) = 0;
	virtual void  GetBrightnessRangeMax( float *v ) = 0;
	virtual void  GetBrightnessDefaultValue( float *v ) = 0;
	virtual void  GetBrightnessStepSize( float *v ) = 0;
	virtual void  GetBrightness( float *v ) = 0;
	virtual void  SetBrightness( float v ) = 0;

	virtual void  GetHueRangeMin( float *v ) = 0;
	virtual void  GetHueRangeMax( float *v ) = 0;
	virtual void  GetHueDefaultValue( float *v ) = 0;
	virtual void  GetHueStepSize( float *v ) = 0;
	virtual void  GetHue( float *v ) = 0;
	virtual void  SetHue( float v ) = 0;

	virtual void  GetSaturationRangeMin( float *v ) = 0;
	virtual void  GetSaturationRangeMax( float *v ) = 0;
	virtual void  GetSaturationDefaultValue( float *v ) = 0;
	virtual void  GetSaturationStepSize( float *v ) = 0;
	virtual void  GetSaturation( float *v ) = 0;
	virtual void  SetSaturation( float v ) = 0;

	virtual void update() = 0;
};
//---------------------------------------------------------------------------

//sdl event code.
//---------------------------------------------------------------------------
#define WM_GRAPHNOTIFY  (Native_Event_Code+1)
#define WM_CALLBACKCMD  (Native_Event_Code+2)
// #define EC_UPDATE		(Native_Event_Code+3)
#define WM_STATE_CHANGE	(Native_Event_Code+4)
//---------------------------------------------------------------------------


#endif


