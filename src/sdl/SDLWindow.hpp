#ifndef _SDL_WINDOW_H_
#define _SDL_WINDOW_H_

#include "tjsCommHead.h"
// #include "TVPWindow.h"
#include "TouchPoint.h"
#include "WindowIntf.h"
#include "Application.h"
#include <SDL.h>
#include "DebugIntf.h"

enum {
	orientUnknown,
	orientPortrait,
	orientLandscape,
};

// enum tTVPWMRRegMode { wrmRegister=0, wrmUnregister=1 };

typedef SDL_Window* WHAND;
// extern WHAND g_sdl_win;
extern SDL_Window* gWindow;

class tTJSNI_Window;
struct tTVPRect;
class tTVPBaseBitmap;

class SDLWindow: public TouchHandler
{
private:
	SDL_Window* win;
	int width;
	int height;
	int left;

	unsigned long LastRecheckInputStateSent;

	//-- layer position / size
	int LayerLeft;
	int LayerTop;
	int LayerWidth;
	int LayerHeight;
	int ZoomDenom; // Zooming factor denominator (setting)
	int ZoomNumer; // Zooming factor numerator (setting)
	int ActualZoomDenom; // Zooming factor denominator (actual)
	int ActualZoomNumer; // Zooming factor numerator (actual)


	TouchPointList touch_points_;

	tjs_int DisplayOrientation;
	tjs_int DisplayRotate;

		//-- drawdevice related
	bool NextSetWindowHandleToDrawDevice;
	tTVPRect LastSentDrawDeviceDestRect;
	//-- TJS object related
	tTJSNI_Window * TJSNativeInstance;
	int LastMouseDownX, LastMouseDownY; // in Layer coodinates
	
	POINT LastMouseMovedPos;  // in Layer coodinates
	//-- full screen managemant related
	int InnerWidthSave;
	int InnerHeightSave;
	DWORD OrgStyle;
	DWORD OrgExStyle;
	int OrgLeft;
	int OrgTop;
	int OrgWidth;
	int OrgHeight;
	int OrgClientWidth;
	int OrgClientHeight;
	tTVPRect FullScreenDestRect;

public:
	SDLWindow(int w, int h,tTVPApplication* app, tTJSNI_Window* ni);
	WHAND GetHandle() { return win; }
	WHAND GetHandle() const { return win; }

	void InvalidateClose();
	bool GetFormEnabled(){return true;}
	void SendCloseMessage();
	void TickBeat();
	bool GetWindowActive(){return true;}
	void ResetDrawDevice();
	void SetPaintBoxSize(int w, int h);
	void InternalSetPaintBoxSize(){SetDrawDeviceDestRect();}
	void SetDrawDeviceDestRect();

	void InternalKeyDown(WORD key, tjs_uint32 shift){}

	// void SetSize(int w, int h);

	bool GetFullScreenMode(){return false;}

	SIZE		window_client_size_;
	SIZE		min_size_;
	SIZE		max_size_;
	int			border_style_;
	bool		in_window_;
	bool		ignore_touch_mouse_;

	// bool in_mode_;
	// int modal_result_;

	// bool has_parent_;


	static void SetClientSize( HWND hWnd, SIZE& size );

//-- properties
	bool GetVisible() const{
		// assert(false);//TODO
		return true;
	}
	void SetVisible(bool s){
		TVPAddLog(TJS_W("set visible in SDLWindow"));
		// assert(false);//TODO
	}
	void Show() { SetVisible( true ); BringToFront(); }
	void Hide() { SetVisible( false ); }

	bool GetEnable() const{
		assert(false);//TODO
		return true;
	}
	void SetEnable( bool s ){
		assert(false);//TODO
	}

	void GetCaption( std::wstring& v ) const{
		assert(false);//TODO
		// return TJS_W("");
	}
	void SetCaption( const std::wstring& v ){

	}
	
	void SetBorderStyle( enum tTVPBorderStyle st){

	}
	enum tTVPBorderStyle GetBorderStyle() const{
		return (tTVPBorderStyle)0;
	}

	void SetWidth( int w ){
		width = w;
	}
	int GetWidth() const{
		return width;
	}
	void SetHeight( int h ){
		height = h;
	}
	int GetHeight() const{
		return height;
	}
	void SetSize( int w, int h );
	void GetSize( int &w, int &h ){
		w = width;
		h = height;
	}

	void SetLeft( int l ){

	}
	int GetLeft() const{
		return 0;
	}
	void SetTop( int t ){;}
	int GetTop() const{
		return 0;
	}
	void SetPosition( int l, int t ){;}
	
	void SetBounds( int x, int y, int width, int height ){;}

	void SetMinWidth( int v ) {
		min_size_.cx = v;
		CheckMinMaxSize();
	}
	int GetMinWidth() const{ return min_size_.cx; }
	void SetMinHeight( int v ) {
		min_size_.cy = v;
		CheckMinMaxSize();
	}
	int GetMinHeight() const { return min_size_.cy; }
	void SetMinSize( int w, int h ) {
		min_size_.cx = w;
		min_size_.cy = h;
		CheckMinMaxSize();
	}

	void SetMaxWidth( int v ) {
		max_size_.cx = v;
		CheckMinMaxSize();
	}
	int GetMaxWidth() const{ return max_size_.cx; }
	void SetMaxHeight( int v ) {
		max_size_.cy = v;
		CheckMinMaxSize();
	}
	int GetMaxHeight() const{ return max_size_.cy; }
	void SetMaxSize( int w, int h ) {
		max_size_.cx = w;
		max_size_.cy = h;
		CheckMinMaxSize();
	}
	void CheckMinMaxSize() {
		int maxw = max_size_.cx;
		int maxh = max_size_.cy;
		int minw = min_size_.cx;
		int minh = min_size_.cy;
		int dw, dh;
		GetSize( dw, dh );
		int sw = dw;
		int sh = dh;
		if( maxw > 0 && dw > maxw ) dw = maxw;
		if( maxh > 0 && dh > maxh ) dh = maxh;
		if( minw > 0 && dw < minw ) dw = minw;
		if( minh > 0 && dh < minh ) dh = minh;
		if( sw != dw || sh != dh ) {
			SetSize( dw, dh );
		}
	}

	void SetInnerWidth( int w ){;}
	int GetInnerWidth() const{return width;}
	void SetInnerHeight( int h ){;}
	int GetInnerHeight() const{return height;}
	void SetInnerSize( int w, int h ){;}
	
	void BringToFront(){}
	void SetStayOnTop( bool b ){}
	bool GetStayOnTop() const {return false;}

	int ShowModal();
	void closeModal();
	// bool IsModal() const { return in_mode_; }
	void Close(){}

	void GetClientRect( struct tTVPRect& rt ){;}

	void SetDefaultMouseCursor(){}
	void SetMouseCursor(int h){}
	void GetCursorPos(int &x,int &y){}
	void SetCursorPos(int x, int y){}

	void SetHintText(iTJSDispatch2* sender,  const ttstr &text ){}
	void SetAttentionPoint(tjs_int left, tjs_int top, const tTVPFont * font){}
	void DisableAttentionPoint(){}
	WHAND GetSurfaceWindowHandle() { return GetHandle();}
	WHAND GetWindowHandle() {return GetHandle();}
	WHAND GetWindowHandleForPlugin() {return GetHandle();}

	void ZoomRectangle( tjs_int& left, tjs_int& top, tjs_int& right, tjs_int& bottom);
	void GetVideoOffset(tjs_int &ofsx, tjs_int &ofsy);

	void RegisterWindowMessageReceiver(tTVPWMRRegMode mode,
		void * proc, const void *userdata);

	void OnCloseQueryCalled(bool b){}
	// void BeginMove();
	void UpdateWindow(tTVPUpdateType type);

	void SetVisibleFromScript(bool s);

	void SetFullScreenMode(bool b){;}

	void AdjustNumerAndDenom(tjs_int &n, tjs_int &d);
	void SetZoom(tjs_int numer, tjs_int denom, bool set_logical = true);
	
	void SetZoomNumer( tjs_int n ) { SetZoom(n, ZoomDenom); }
	tjs_int GetZoomNumer() const { return ZoomNumer; }
	void SetZoomDenom(tjs_int d) { SetZoom(ZoomNumer, d); }
	tjs_int GetZoomDenom() const { return ZoomDenom; }
	
	void SetTouchScaleThreshold( double threshold ) {
		touch_points_.SetScaleThreshold( threshold );
	}
	double GetTouchScaleThreshold() const {
		return touch_points_.GetScaleThreshold();
	}
	void SetTouchRotateThreshold( double threshold ) {
		touch_points_.SetRotateThreshold( threshold );
	}
	double GetTouchRotateThreshold() const {
		return touch_points_.GetRotateThreshold();
	}
	tjs_real GetTouchPointStartX( tjs_int index ) const { return touch_points_.GetStartX(index); }
	tjs_real GetTouchPointStartY( tjs_int index ) const { return touch_points_.GetStartY(index); }
	tjs_real GetTouchPointX( tjs_int index ) const { return touch_points_.GetX(index); }
	tjs_real GetTouchPointY( tjs_int index ) const { return touch_points_.GetY(index); }
	tjs_int GetTouchPointID( tjs_int index ) const { return touch_points_.GetID(index); }
	tjs_int GetTouchPointCount() const { return touch_points_.CountUsePoint(); }
	bool GetTouchVelocity( tjs_int id, float& x, float& y, float& speed ) const {
		// return TouchVelocityTracker.getVelocity( id, x, y, speed );
		return false;
	}

	void SetEnableTouch( bool b ){}
	bool GetEnableTouch() const{return false;}

	void UpdateOrientation();
	bool GetOrientation( int& orientation, int& rotate ) const;

	int GetDisplayOrientation() { UpdateOrientation(); return DisplayOrientation; }
	int GetDisplayRotate() { UpdateOrientation(); return DisplayRotate; }


	// メッセージハンドラ
	virtual void OnActive( HWND preactive ) {}
	virtual void OnDeactive( HWND postactive ) {}
	// virtual void OnClose( CloseAction& action ){}
	virtual bool OnCloseQuery() { return true; }
	virtual void OnFocus(HWND hFocusLostWnd) {}
	virtual void OnFocusLost(HWND hFocusingWnd) {}
	virtual void OnMouseDown( int button, int shift, int x, int y ){}
	virtual void OnMouseUp( int button, int shift, int x, int y ){}
	virtual void OnMouseMove( int shift, int x, int y ){}
	virtual void OnMouseDoubleClick( int button, int x, int y ) {}
	virtual void OnMouseClick( int button, int shift, int x, int y ){}
	virtual void OnMouseWheel( int delta, int shift, int x, int y ){}
	virtual void OnKeyUp( WORD vk, int shift ){}
	virtual void OnKeyDown( WORD vk, int shift, int repeat, bool prevkeystate ){}
	virtual void OnKeyPress( WORD vk, int repeat, bool prevkeystate, bool convertkey ){}
	virtual void OnMove( int x, int y ) {}
	virtual void OnResize( UINT_PTR state, int w, int h ) {}
	// virtual void OnDropFile( HDROP hDrop ) {}
	// virtual int OnMouseActivate( HWND hTopLevelParentWnd, WORD hitTestCode, WORD MouseMsg ) { return MA_ACTIVATE; }
	virtual bool OnSetCursor( HWND hContainsCursorWnd, WORD hitTestCode, WORD MouseMsg ) { return false; }
	virtual void OnEnable( bool enabled ) {}
	virtual void OnEnterMenuLoop( bool entered ) {}
	virtual void OnExitMenuLoop( bool isShortcutMenu ) {}
	virtual void OnDeviceChange( UINT_PTR event, void *data ) {}
	virtual void OnNonClientMouseDown( int button, UINT_PTR hittest, int x, int y ){}
	virtual void OnMouseEnter() {}
	virtual void OnMouseLeave() {}
	virtual void OnShow( UINT_PTR status ) {}
	virtual void OnHide( UINT_PTR status ) {}

	virtual void OnTouchDown( double x, double y, double cx, double cy, DWORD id, DWORD tick ) {}
	virtual void OnTouchMove( double x, double y, double cx, double cy, DWORD id, DWORD tick ) {}
	virtual void OnTouchUp( double x, double y, double cx, double cy, DWORD id, DWORD tick ) {}
	virtual void OnTouchSequenceStart() {}
	virtual void OnTouchSequenceEnd() {}

	virtual void OnTouchScaling( double startdist, double currentdist, double cx, double cy, int flag ){}
	virtual void OnTouchRotate( double startangle, double currentangle, double distance, double cx, double cy, int flag ){}
	virtual void OnMultiTouch(){}

	virtual void OnDisplayChange( UINT_PTR bpp, WORD hres, WORD vres ) {}
	virtual void OnApplicationActivateChange( bool activated, DWORD thread_id ) {}

~SDLWindow();


};



#endif