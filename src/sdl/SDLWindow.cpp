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
    
    innerLayerLeft = dst.x;
    innerLayerTop = dst.y;
    innerLayerWidth = dst.w;
    innerLayerHeight = dst.h;
    
    width = w;
    height = h;
    FullScreenDestRect.set_width(width);
    FullScreenDestRect.set_height(height);
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
tjs_uint32 TVP_TShiftState_To_uint32(TShiftState state)
{
	tjs_uint32 result = 0;
	if( state & KMOD_SHIFT ) {
		result |= ssShift;
	}
	if( state & KMOD_CTRL ) {
		result |= ssCtrl;
	}
	if( state & KMOD_ALT ) {
		result |= ssAlt;
	}
	return result;
}

void SDLWindow::trans_point_frome_win(int &x,int &y)
{
    int _x = x;
    int _y = y;
    
    if(width != LayerWidth || LayerLeft != left)
    {
//        float rate = innerLayerWidth * 1.0 / width;
//        x = innerLayerLeft + int((x - left) * rate);
        float rate = LayerWidth * 1.0 / innerLayerWidth;
        x = (int)((x - left) - innerLayerLeft) * rate;
        
    }
    if(height != LayerHeight || LayerTop != top)
    {
//        float rate = innerLayerHeight * 1.0 / height;
//        y = innerLayerTop + int((y - top) * rate);
        float rate = LayerHeight * 1.0 / innerLayerHeight;
        y = (int)((y - top) - innerLayerTop)*rate;
    }
    if(_x!=x || _y!=y)
    {
//        SDL_Log("trans mouse point:(%d,%d)->(%d,%d)",_x,_y,x,y);
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
	OnMouseMove(shift,x,y);
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

void SDLWindow::OnKeyUp( WORD vk, int shift ){
	tjs_uint32 s = TVP_TShiftState_To_uint32(shift);
//    s |= GetMouseButtonState();
	InternalKeyUp(vk, s );
}
void SDLWindow::OnKeyDown( WORD vk, int shift, int repeat, bool prevkeystate ){
	if(TJSNativeInstance) {
		tjs_uint32 s = TVP_TShiftState_To_uint32( shift );
//        s |= GetMouseButtonState();
		if( prevkeystate && repeat > 0 ) s |= TVP_SS_REPEAT;
		InternalKeyDown( vk, s );
	}
}
void SDLWindow::OnKeyPress( WORD vk, int repeat, bool prevkeystate, bool convertkey ){
	// if( TJSNativeInstance && vk ) {
	// 	if(UseMouseKey && (vk == 0x1b || vk == 13 || vk == 32)) return;
	// 	// UNICODE なのでそのまま渡してしまう
	// 	TVPPostInputEvent(new tTVPOnKeyPressInputEvent(TJSNativeInstance, vk));
	// }
}

void SDLWindow::InternalKeyUp(WORD key, tjs_uint32 shift) {
	// DWORD tick = GetTickCount();
	// TVPPushEnvironNoise(&tick, sizeof(tick));
	// TVPPushEnvironNoise(&key, sizeof(key));
	// TVPPushEnvironNoise(&shift, sizeof(shift));
	if( TJSNativeInstance ) {
		// if( UseMouseKey /*&& PaintBox*/ ) {
		// 	if( key == VK_RETURN || key == VK_SPACE || key == VK_ESCAPE || key == VK_PAD1 || key == VK_PAD2) {
		// 		POINT p;
		// 		::GetCursorPos(&p);
		// 		::ScreenToClient( GetHandle(), &p );
		// 		if( p.x >= 0 && p.y >= 0 && p.x < GetInnerWidth() && p.y < GetInnerHeight() ) {
		// 			if( key == VK_RETURN || key == VK_SPACE || key == VK_PAD1 ) {
		// 				OnMouseClick( mbLeft, 0, p.x, p.y );
		// 				MouseLeftButtonEmulatedPushed = false;
		// 				OnMouseUp( mbLeft, 0, p.x, p.y );
		// 			}

		// 			if( key == VK_ESCAPE || key == VK_PAD2 ) {
		// 				MouseRightButtonEmulatedPushed = false;
		// 				OnMouseUp( mbRight, 0, p.x, p.y );
		// 			}
		// 		}
		// 		return;
		// 	}
		// }

		TVPPostInputEvent(new tTVPOnKeyUpInputEvent(TJSNativeInstance, key, shift));
	}
}

void SDLWindow::InternalKeyDown(WORD key, tjs_uint32 shift) {
	// DWORD tick = GetTickCount();
	// TVPPushEnvironNoise(&tick, sizeof(tick));
	// TVPPushEnvironNoise(&key, sizeof(key));
	// TVPPushEnvironNoise(&shift, sizeof(shift));

	if( TJSNativeInstance ) {
		// if(UseMouseKey /*&& PaintBox*/ ) {
		// 	if(key == VK_RETURN || key == VK_SPACE || key == VK_ESCAPE || key == VK_PAD1 || key == VK_PAD2) {
		// 		POINT p;
		// 		::GetCursorPos(&p);
		// 		::ScreenToClient( GetHandle(), &p );
		// 		if( p.x >= 0 && p.y >= 0 && p.x < GetInnerWidth() && p.y < GetInnerHeight() ) {
		// 			if( key == VK_RETURN || key == VK_SPACE || key == VK_PAD1 ) {
		// 				MouseLeftButtonEmulatedPushed = true;
		// 				OnMouseDown( mbLeft, 0, p.x, p.y );
		// 			}

		// 			if(key == VK_ESCAPE || key == VK_PAD2) {
		// 				MouseRightButtonEmulatedPushed = true;
		// 				OnMouseDown( mbLeft, 0, p.x, p.y );
		// 			}
		// 		}
		// 		return;
		// 	}

		// 	switch(key) {
		// 	case VK_LEFT:
		// 	case VK_PADLEFT:
		// 		if( MouseKeyXAccel == 0 && MouseKeyYAccel == 0 ) {
		// 			GenerateMouseEvent(true, false, false, false);
		// 			LastMouseKeyTick = GetTickCount() + 100;
		// 		}
		// 		return;
		// 	case VK_RIGHT:
		// 	case VK_PADRIGHT:
		// 		if(MouseKeyXAccel == 0 && MouseKeyYAccel == 0)
		// 		{
		// 			GenerateMouseEvent(false, true, false, false);
		// 			LastMouseKeyTick = GetTickCount() + 100;
		// 		}
		// 		return;
		// 	case VK_UP:
		// 	case VK_PADUP:
		// 		if(MouseKeyXAccel == 0 && MouseKeyYAccel == 0)
		// 		{
		// 			GenerateMouseEvent(false, false, true, false);
		// 			LastMouseKeyTick = GetTickCount() + 100;
		// 		}
		// 		return;
		// 	case VK_DOWN:
		// 	case VK_PADDOWN:
		// 		if(MouseKeyXAccel == 0 && MouseKeyYAccel == 0)
		// 		{
		// 			GenerateMouseEvent(false, false, false, true);
		// 			LastMouseKeyTick = GetTickCount() + 100;
		// 		}
		// 		return;
		// 	}
		// }
		TVPPostInputEvent(new tTVPOnKeyDownInputEvent(TJSNativeInstance, key, shift));
	}
}

