#include "SDLWindow.hpp"
#include <stdio.h>
#include <codecvt>
#include <string>
#include <locale>

#include "sdlinit.h"
// g_sdl_win = NULL;

extern int init_sdl();
extern int init_gl_context(SDL_Window* win);
SDLWindow::SDLWindow(int w, int h,
	tTVPApplication* app, tTJSNI_Window* ni):FullScreenDestRect(0,0,0,0),
LayerLeft(0), LayerTop(0),touch_points_(this)
{
	std::string title;
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
	title = cv.to_bytes(app->GetTitle());
	NextSetWindowHandleToDrawDevice = true;
	LastSentDrawDeviceDestRect.clear();


	ZoomDenom = ActualZoomDenom = 1;
	ZoomNumer = ActualZoomNumer = 1;

	DisplayOrientation = orientUnknown;
	DisplayRotate = -1;

	init_sdl();

    win = create_sdl_window(title.c_str(),w,h);
    if(!win)
    {
    	wprintf(TJS_W("create sdl win error!"));
 	    printf("win create error:%s\n",SDL_GetError());

    }
    int _w,_h;
    SDL_GetWindowSize(win, &_w, &_h); //dev true size.
    if(w!=_w || h!=_h)
    {
        SDL_Log("win resize!(%d,%d)->(%d,%d)",w,h,_w,_h);
        w = _w;
        h = _h;
    }
        

    if(win)
    {
    	init_gl_context(win);
    }
    ViewRect src = {0,0,w,h};
    ViewRect dst = src;
    if(g_scale_win)
    {
        scale_view(src, dst, g_scale_win);
        SDL_Log("do scale win:(%d,%d,%d,%d)->(%d,%d,%d,%d)",
                src.x,src.y,src.w,src.h,
                dst.x,dst.y,dst.w,dst.h
                );
    }
    
    LayerLeft = dst.x;
    LayerTop = dst.y;
    LayerWidth = dst.w;
    LayerHeight = dst.h;
    
    width = w;
    height = h;
    left = top = 0;
    TJSNativeInstance = ni;
    app->AddWindow(this);
    SetVisibleFromScript(true);
}
SDLWindow::~SDLWindow()
{
	Application->RemoveWindow(this);
}


void SDLWindow::InvalidateClose()
{

}

void SDLWindow::SendCloseMessage()
{

}

void SDLWindow::TickBeat()
{
// called every 50ms intervally

}

void SDLWindow::ResetDrawDevice()
{
	NextSetWindowHandleToDrawDevice = true;
	LastSentDrawDeviceDestRect.clear();

}

void SDLWindow::SetPaintBoxSize(int w, int h)
{
	LayerWidth  = w;
	LayerHeight = h;
	InternalSetPaintBoxSize();
}
void SDLWindow::SetDrawDeviceDestRect()
{
	tTVPRect destrect;
	tjs_int w = MulDiv(LayerWidth,  ActualZoomNumer, ActualZoomDenom);
	tjs_int h = MulDiv(LayerHeight, ActualZoomNumer, ActualZoomDenom);
	if( w < 1 ) w = 1;
	if( h < 1 ) h = 1;
	if( GetFullScreenMode() ) {
		destrect.left = FullScreenDestRect.left;
		destrect.top = FullScreenDestRect.top;
		destrect.right = destrect.left + w;
		destrect.bottom = destrect.top + h;
	} else {
		destrect.left = destrect.top = 0;
		destrect.right = w;
		destrect.bottom = h;
	}

	if( LastSentDrawDeviceDestRect != destrect ) {
		if( TJSNativeInstance ) {
			if( GetFullScreenMode() ) {
				TJSNativeInstance->GetDrawDevice()->SetClipRectangle(FullScreenDestRect);
			} else {
				TJSNativeInstance->GetDrawDevice()->SetClipRectangle(destrect);
			}
			TJSNativeInstance->GetDrawDevice()->SetDestRectangle(destrect);
		}
		LastSentDrawDeviceDestRect = destrect;
	}
}


void SDLWindow::SetSize(int w, int  h)
{
	if(width != w || height != h)
	{
		width = w;
		height = h;
		SDL_SetWindowSize(win,w,h);
	}
}

void SDLWindow::ZoomRectangle( tjs_int& left, tjs_int& top, tjs_int& right, tjs_int& bottom) 
{
	left =   MulDiv(left  ,  ActualZoomNumer, ActualZoomDenom);
	top =    MulDiv(top   ,  ActualZoomNumer, ActualZoomDenom);
	right =  MulDiv(right ,  ActualZoomNumer, ActualZoomDenom);
	bottom = MulDiv(bottom,  ActualZoomNumer, ActualZoomDenom);
}
void SDLWindow::GetVideoOffset(tjs_int &ofsx, tjs_int &ofsy) 
{
	if( GetFullScreenMode() ) {
		ofsx = FullScreenDestRect.left;
		ofsy = FullScreenDestRect.top;
	} else {
		ofsx = 0;
		ofsy = 0;
	}
}

void SDLWindow::RegisterWindowMessageReceiver(tTVPWMRRegMode mode,
		void * proc, const void *userdata)
{

}
void SDLWindow::UpdateWindow(tTVPUpdateType type )
{
	if( TJSNativeInstance ) {
		tTVPRect r;
		r.left = 0;
		r.top = 0;
		r.right = LayerWidth;
		r.bottom = LayerHeight;
		TJSNativeInstance->NotifyWindowExposureToLayer(r);
		TVPDeliverWindowUpdateEvents();
	}
}

void SDLWindow::SetVisibleFromScript(bool b)
{
	TVPAddLog(TJS_W("set visible from sciprt!"));
	bool ismain = false;
	if( TJSNativeInstance ) ismain = TJSNativeInstance->IsMainWindow();
	if( TJSNativeInstance ) TJSNativeInstance->GetDrawDevice()->SetTargetWindow( GetHandle(), ismain);
	SetVisible( b );
}

void SDLWindow::AdjustNumerAndDenom(tjs_int &n, tjs_int &d)
{
	tjs_int a = n;
	tjs_int b = d;
	while( b ) {
		tjs_int t = b;
		b = a % b;
		a = t;
	}
	n = n / a;
	d = d / a;
}

void SDLWindow::SetZoom( tjs_int numer, tjs_int denom, bool set_logical ) {
	bool ischanged = false;
	// set layer zooming factor;
	// the zooming factor is passed in numerator/denoiminator style.
	// we must find GCM to optimize numer/denium via Euclidean algorithm.
	AdjustNumerAndDenom(numer, denom);
	if( set_logical ) {
		if( ZoomNumer != numer || ZoomDenom != denom ) {
			ischanged = true;
		}
		ZoomNumer = numer;
		ZoomDenom = denom;
	}
	if( !GetFullScreenMode() ) {
		// in fullscreen mode, zooming factor is controlled by the system
		ActualZoomDenom = denom;
		ActualZoomNumer = numer;
	}
	InternalSetPaintBoxSize();
	// if( ischanged ) ::InvalidateRect( GetHandle(), NULL, FALSE );
}

void SDLWindow::UpdateOrientation() {
	if( DisplayOrientation == orientUnknown || DisplayRotate < 0 ) {
		int orient, rot;
		if( GetOrientation( orient, rot ) ) {
			if( DisplayOrientation != orient || DisplayRotate != rot ) {
				DisplayOrientation = orient;
				DisplayRotate = rot;
			}
		}
	}
}

bool SDLWindow::GetOrientation( int& orientation, int& rotate ) const {
	// DEVMODE mode = {0};
	// mode.dmSize = sizeof(DEVMODE);
	// mode.dmDriverExtra = 0;
	// mode.dmFields |= DM_DISPLAYORIENTATION | DM_PELSWIDTH | DM_PELSHEIGHT;

	// BOOL ret = ::EnumDisplaySettingsEx( NULL, ENUM_CURRENT_SETTINGS, &mode, EDS_ROTATEDMODE );
	// if( ret ) {
	// 	if( mode.dmPelsWidth > mode.dmPelsHeight ) {
	// 		orientation = orientLandscape;
	// 	} else if( mode.dmPelsWidth < mode.dmPelsHeight ) {
	// 		orientation = orientPortrait;
	// 	} else {
	// 		orientation = orientUnknown;
	// 	}
		/* dmDisplayOrientation と共有(unionなので)されているので、以下では取得できない
		if( mode.dmOrientation == DMORIENT_PORTRAIT ) {	// 横
			orientation = orientPortrait;
		} else if( mode.dmOrientation == DMORIENT_LANDSCAPE ) {	// 縦
			orientation = orientLandscape;
		} else {	// unknown
			orientation = orientUnknown;
		}
		*/
	// 	switch( mode.dmDisplayOrientation ) {
	// 	case DMDO_DEFAULT:
	// 		rotate = 0;
	// 		break;
	// 	case DMDO_90:
	// 		rotate = 90;
	// 		break;
	// 	case DMDO_180:
	// 		rotate = 180;
	// 		break;
	// 	case DMDO_270:
	// 		rotate = 270;
	// 		break;
	// 	default:
	// 		rotate = -1;
	// 	}
	// }
	// return ret != FALSE;
	return false;
}

tTVPMouseButton TVP_TMouseButton_To_tTVPMouseButton(int button) {
	return (tTVPMouseButton)button;
}
tjs_uint32 TVP_TShiftState_To_uint32(int b)
{
	return 0;
}

void SDLWindow::trans_point_frome_win(int &x,int &y)
{
    int _x = x;
    int _y = y;
    if(width != LayerWidth || LayerLeft != left)
    {
        float rate = LayerWidth * 1.0 / width;
        x = LayerLeft + int((x - left) * rate);
    }
    if(height != LayerHeight || LayerTop != top)
    {
        float rate = LayerHeight * 1.0 / height;
        y = LayerTop + int((y - top) * rate);
    }
    if(_x!=x || _y!=y)
    {
        SDL_Log("trans mouse point:(%d,%d)->(%d,%d)",_x,_y,x,y);
    }
}

void SDLWindow::OnMouseDown( int button, int shift, int x, int y )
{
	// MouseVelocityTracker.addMovement( TVPGetRoughTickCount32(), (float)x, (float)y );

	LastMouseDownX = x;
	LastMouseDownY = y;

	if(TJSNativeInstance) {
		tjs_uint32 s = TVP_TShiftState_To_uint32(shift);
		// s |= GetMouseButtonState();
		tTVPMouseButton b = TVP_TMouseButton_To_tTVPMouseButton(button);
		TVPPostInputEvent( new tTVPOnMouseDownInputEvent(TJSNativeInstance, x, y, b, s));
	}
}
void SDLWindow::OnMouseUp( int button, int shift, int x, int y )
{
	// MouseVelocityTracker.addMovement( TVPGetRoughTickCount32(), (float)x, (float)y );
	if(TJSNativeInstance) {
		tjs_uint32 s = TVP_TShiftState_To_uint32(shift);
		// s |= GetMouseButtonState();
		tTVPMouseButton b = TVP_TMouseButton_To_tTVPMouseButton(button);
		TVPPostInputEvent( new tTVPOnMouseUpInputEvent(TJSNativeInstance, x, y, b, s));
	}
}
void SDLWindow::OnMouseMove( int shift, int x, int y )
{
	// TranslateWindowToDrawArea(x, y);
	// MouseVelocityTracker.addMovement( TVPGetRoughTickCount32(), (float)x, (float)y );
	if( TJSNativeInstance ) {
		tjs_uint32 s = TVP_TShiftState_To_uint32(shift);
		// s |= GetMouseButtonState();
		TVPPostInputEvent( new tTVPOnMouseMoveInputEvent(TJSNativeInstance, x, y, s), TVP_EPT_DISCARDABLE );
	}

	// RestoreMouseCursor();

	int pos = (y << 16) + x;
	// TVPPushEnvironNoise(&pos, sizeof(pos));

	LastMouseMovedPos.x = x;
	LastMouseMovedPos.y = y;
}

void SDLWindow::OnMouseDoubleClick( int button, int x, int y )
{
	// fire double click event
	if( TJSNativeInstance ) {
		TVPPostInputEvent( new tTVPOnDoubleClickInputEvent(TJSNativeInstance, LastMouseDownX, LastMouseDownY));
	}
}
void SDLWindow::OnMouseClick( int button, int shift, int x, int y )
{
		// fire click event
	if( TJSNativeInstance ) {
		TVPPostInputEvent( new tTVPOnClickInputEvent(TJSNativeInstance, LastMouseDownX, LastMouseDownY));
	}
}

