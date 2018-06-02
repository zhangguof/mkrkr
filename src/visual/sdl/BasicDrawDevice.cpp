
#define NOMINMAX
#include "tjsCommHead.h"
#include "DrawDevice.h"
#include "BasicDrawDevice.h"
#include "LayerIntf.h"
#include "MsgIntf.h"
#include "SysInitIntf.h"
#include "WindowIntf.h"
#include "DebugIntf.h"
// #include "ThreadIntf.h"
#include "utils.h"
#include "ComplexRect.h"
#include "EventImpl.h"
#include "WindowImpl.h"

// #include <d3d9.h>
// #include <mmsystem.h>
#include <algorithm>

//---------------------------------------------------------------------------
// オプション
//---------------------------------------------------------------------------
static tjs_int TVPBasicDrawDeviceOptionsGeneration = 0;
bool TVPZoomInterpolation = true;
//---------------------------------------------------------------------------
static void TVPInitBasicDrawDeviceOptions()
{
	if(TVPBasicDrawDeviceOptionsGeneration == TVPGetCommandLineArgumentGeneration()) return;
	TVPBasicDrawDeviceOptionsGeneration = TVPGetCommandLineArgumentGeneration();

	tTJSVariant val;
	TVPZoomInterpolation = true;
	if(TVPGetCommandLine(TJS_W("-smoothzoom"), &val))
	{
		ttstr str(val);
		if(str == TJS_W("no"))
			TVPZoomInterpolation = false;
		else
			TVPZoomInterpolation = true;
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tTVPBasicDrawDevice::tTVPBasicDrawDevice()
{
	TVPAddLog(TJS_W("tTVPBasicDrawDevice:tTVPBasicDrawDevice!!"));
	// TVPInitBasicDrawDeviceOptions(); // read and initialize options
	TargetWindow = NULL;
	DrawUpdateRectangle = false;
	BackBufferDirty = true;

	// Direct3D = NULL;
	// Direct3DDevice = NULL;
	Texture = NULL;
	gldevice = NULL;

	ShouldShow = false;
	TextureBuffer = NULL;
	TextureWidth = TextureHeight = 0;
	// VsyncInterval = 16;

}
//---------------------------------------------------------------------------
tTVPBasicDrawDevice::~tTVPBasicDrawDevice()
{
	// DestroyD3DDevice();
}
//---------------------------------------------------------------------------
// void tTVPBasicDrawDevice::DestroyD3DDevice() {
// 	DestroyTexture();
// 	if(Direct3DDevice) Direct3DDevice->Release(), Direct3DDevice = NULL;
// 	if(Direct3D) Direct3D = NULL;
// }
//---------------------------------------------------------------------------
void tTVPBasicDrawDevice::DestroyTexture() {
	// if(TextureBuffer && Texture)
	// 	Texture->UnlockRect(0), TextureBuffer = NULL;
	if(Texture){
		delete Texture;
		Texture = NULL;
	} 
}
//---------------------------------------------------------------------------
void tTVPBasicDrawDevice::InvalidateAll()
{
	// レイヤ演算結果をすべてリクエストする
	// サーフェースが lost した際に内容を再構築する目的で用いる
	RequestInvalidation(tTVPRect(0, 0, DestRect.get_width(), DestRect.get_height()));
}
//---------------------------------------------------------------------------
void tTVPBasicDrawDevice::CheckMonitorMoved() {
	// UINT iCurrentMonitor = GetMonitorNumber( TargetWindow );
	assert(false);//TODO
	// if( CurrentMonitor != iCurrentMonitor ) {
	// 	// モニタ移動が発生しているので、デバイスを再生成する
	// 	CreateD3DDevice();
	// }
}
//---------------------------------------------------------------------------
bool tTVPBasicDrawDevice::IsTargetWindowActive() const {
	if( TargetWindow == NULL ) return false;
	//return ::GetForegroundWindow() == TargetWindow;

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
UINT tTVPBasicDrawDevice::GetMonitorNumber( HWND window )
{
	assert(false);//TODO
	return 0;
}

bool tTVPBasicDrawDevice::CreateGLDevice()
{
	TVPAddLog(TJS_W("in tTVPBasicDrawDevice::CreateGLDevice!!"));
	if(TargetWindow)
	{
		tjs_int w, h;
		GetSrcSize(w, h);
		if(w > 0 && h>0)
		{
			gldevice = create_gl_device(w,h);
			gldevice->init_render();
			TVPAddLog(TJS_W("create gl device success!!"));
			return CreateTexture();
		}
	}
	return false;
	

}

//---------------------------------------------------------------------------
bool tTVPBasicDrawDevice::CreateTexture() {
	// if(Texture) return false;
	//assert(false);//TODO
	DestroyTexture();
	tjs_int w, h;
	GetSrcSize( w, h );
	if(TargetWindow && w > 0 && h > 0) {

	// 	D3DCAPS9 d3dcaps;
	// 	// Direct3DDevice->GetDeviceCaps( &d3dcaps );

		TextureWidth = w;
		TextureHeight = h;
		Texture = new GLTexture(w,h);

		TVPAddLog(TJS_W("create gl texture success!"));
		return true;
	}

	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void tTVPBasicDrawDevice::EnsureDevice()
{
	TVPInitBasicDrawDeviceOptions();
	if( TargetWindow ) {
		try {
			if(gldevice == NULL)
			{
				CreateGLDevice();
			}

			if( Texture == NULL ) {
				if( CreateTexture() == false ) {
					return;
				}
			}
		} catch(const eTJS & e) {
			TVPAddImportantLog( TVPFormatMessage(TVPBasicDrawDeviceFailedToCreateDirect3DDevice,e.GetMessage()) );
			// DestroyD3DDevice();
		} catch(...) {
			TVPAddImportantLog( (const tjs_char*)TVPBasicDrawDeviceFailedToCreateDirect3DDeviceUnknownReason );
			// DestroyD3DDevice();
		}
	}
}



//---------------------------------------------------------------------------
void tTVPBasicDrawDevice::ErrorToLog( HRESULT hr ) {
	assert(false);//todo
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPBasicDrawDevice::AddLayerManager(iTVPLayerManager * manager)
{
	if(inherited::Managers.size() > 0)
	{
		// "Basic" デバイスでは２つ以上のLayer Managerを登録できない
		TVPThrowExceptionMessage(TVPBasicDrawDeviceDoesNotSupporteLayerManagerMoreThanOne);
	}
	inherited::AddLayerManager(manager);

	manager->SetDesiredLayerType(ltOpaque); // ltOpaque な出力を受け取りたい
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPBasicDrawDevice::SetTargetWindow(WHAND wnd, bool is_main)
{
	// TVPInitBasicDrawDeviceOptions();
	// DestroyD3DDevice();
	TargetWindow = wnd;
	IsMainWindow = is_main;
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPBasicDrawDevice::SetDestRectangle(const tTVPRect & rect)
{
	// assert(false);//TODO
	BackBufferDirty = true;
	// 位置だけの変更の場合かどうかをチェックする
	if(rect.get_width() == DestRect.get_width() && rect.get_height() == DestRect.get_height()) {
		// 位置だけの変更だ
		inherited::SetDestRectangle(rect);
	} else {
		// サイズも違う
		// if( rect.get_width() > (tjs_int)D3dPP.BackBufferWidth || rect.get_height() > (tjs_int)D3dPP.BackBufferHeight ) {
		// 	// バックバッファサイズよりも大きいサイズが指定された場合一度破棄する。後のEnsureDeviceで再生成される。
		// 	DestroyD3DDevice();
		// }
		bool success = true;
		inherited::SetDestRectangle(rect);

		try {
			EnsureDevice();
		} catch(const eTJS & e) {
			TVPAddImportantLog( TVPFormatMessage(TVPBasicDrawDeviceFailedToCreateDirect3DDevices,e.GetMessage() ) );
			success = false;
		} catch(...) {
			TVPAddImportantLog( (const tjs_char*)TVPBasicDrawDeviceFailedToCreateDirect3DDevicesUnknownReason );
			success = false;
		}
		// if( success == false ) {
		// 	DestroyD3DDevice();
		// }
	}
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPBasicDrawDevice::NotifyLayerResize(iTVPLayerManager * manager)
{
	inherited::NotifyLayerResize(manager);

	BackBufferDirty = true;

	// テクスチャを捨てて作り直す。
	CreateTexture();
}
//---------------------------------------------------------------------------
static unsigned int last_time = 0;//time_now();
void TJS_INTF_METHOD tTVPBasicDrawDevice::Show()
{
	// TVPAddLog(TJS_W("in tTVPBasicDrawDevice::Show"));
	// assert(false);//TODO
	if(!TargetWindow) return;
	if(!Texture) return;
	if(!gldevice) return;
	if(!ShouldShow) return;

	ShouldShow = false;
	unsigned int now = time_now();
	gldevice->render(now - last_time);
	last_time = now;

	// // HRESULT hr = D3D_OK;
	// RECT client;
	// if( ::GetClientRect( TargetWindow, &client ) ) {
	// 	RECT drect;
	// 	drect.left   = 0;
	// 	drect.top    = 0;
	// 	drect.right  = client.right - client.left;
	// 	drect.bottom = client.bottom - client.top;

	// 	RECT srect = drect;
	// 	hr = Direct3DDevice->Present( &srect, &drect, TargetWindow, NULL );
	// } else {
	// 	ShouldShow = true;
	// }
}
//---------------------------------------------------------------------------
bool TJS_INTF_METHOD tTVPBasicDrawDevice::WaitForVBlank( tjs_int* in_vblank, tjs_int* delayed )
{
	// if( Direct3DDevice == NULL ) return false;

	// bool inVsync = false;
	// D3DRASTER_STATUS rs;
	// if( D3D_OK == Direct3DDevice->GetRasterStatus(0,&rs) ) {
	// 	inVsync = rs.InVBlank == TRUE;
	// }

	// // VSync 待ちを行う
	// bool isdelayed = false;
	// if(!inVsync) {
	// 	// vblank から抜けるまで待つ
	// 	DWORD timeout_target_tick = ::timeGetTime() + 1;
	// 	rs.InVBlank = FALSE;
	// 	HRESULT hr = D3D_OK;
	// 	do {
	// 		hr = Direct3DDevice->GetRasterStatus(0,&rs);
	// 	} while( D3D_OK == hr && rs.InVBlank == TRUE && (long)(::timeGetTime() - timeout_target_tick) <= 0);

	// 	// vblank に入るまで待つ
	// 	rs.InVBlank = TRUE;
	// 	do {
	// 		hr = Direct3DDevice->GetRasterStatus(0,&rs);
	// 	} while( D3D_OK == hr && rs.InVBlank == FALSE && (long)(::timeGetTime() - timeout_target_tick) <= 0);

	// 	if((int)(::timeGetTime() - timeout_target_tick) > 0) {
	// 		// フレームスキップが発生したと考えてよい
	// 		isdelayed  = true;
	// 	}
	// 	inVsync = rs.InVBlank == TRUE;
	// }
	// *delayed = isdelayed ? 1 : 0;
	// *in_vblank = inVsync ? 1 : 0;
	return true;
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPBasicDrawDevice::StartBitmapCompletion(iTVPLayerManager * manager)
{
	// assert(false);//todo
	TVPAddLog(TJS_W("==tTVPBasicDrawDevice::StartBitmapCompletion!!"));
	EnsureDevice();

	if( Texture && TargetWindow ) {
		if(TextureBuffer) {
			TVPAddImportantLog( (const tjs_char*)TVPBasicDrawDeviceTextureHasAlreadyBeenLocked );
			TextureBuffer = NULL;
		}
		TextureBuffer = Texture->pixels;
		TexturePitch = Texture->pitch;
	}
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPBasicDrawDevice::NotifyBitmapCompleted(iTVPLayerManager * manager,
	tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
	const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity)
{
	// assert(false);//todo
	// bits, bitmapinfo で表されるビットマップの cliprect の領域を、x, y に描画
	// する。
	// opacity と type は無視するしかないので無視する
	tjs_int w, h;
	GetSrcSize( w, h );
	if( TextureBuffer && TargetWindow &&
		!(x < 0 || y < 0 ||
			x + cliprect.get_width() > w ||
			y + cliprect.get_height() > h) &&
		!(cliprect.left < 0 || cliprect.top < 0 ||
			cliprect.right > bitmapinfo->bmiHeader.biWidth ||
			cliprect.bottom > bitmapinfo->bmiHeader.biHeight))
	{
		// 範囲外の転送は(一部だけ転送するのではなくて)無視してよい
		ShouldShow = true;

		// bitmapinfo で表された cliprect の領域を x,y にコピーする
		long src_y       = cliprect.top;
		long src_y_limit = cliprect.bottom;
		long src_x       = cliprect.left;
		long width_bytes   = cliprect.get_width() * 4; // 32bit
		long dest_y      = y;
		long dest_x      = x;
		const tjs_uint8 * src_p = (const tjs_uint8 *)bits;
		long src_pitch;

		if(bitmapinfo->bmiHeader.biHeight < 0)
		{
			// bottom-down
			src_pitch = bitmapinfo->bmiHeader.biWidth * 4;
		}
		else
		{
			// bottom-up
			src_pitch = -bitmapinfo->bmiHeader.biWidth * 4;
			src_p += bitmapinfo->bmiHeader.biWidth * 4 * (bitmapinfo->bmiHeader.biHeight - 1);
		}

		for(; src_y < src_y_limit; src_y ++, dest_y ++)
		{
			const void *srcp = src_p + src_pitch * src_y + src_x * 4;
			void *destp = (tjs_uint8*)TextureBuffer + TexturePitch * dest_y + dest_x * 4;
			memcpy(destp, srcp, width_bytes);
		}
	}
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPBasicDrawDevice::EndBitmapCompletion(iTVPLayerManager * manager)
{
	// assert(false);//todo
	if(!TargetWindow) return;
	if(!Texture) return;
	if(!gldevice) return;

	if(!TextureBuffer) return;
	// Texture->UnlockRect(0);
	TextureBuffer = NULL;
	gldevice->set_texture(Texture);


// // フルスクリーン化後、1回は全体消去、それ以降はウィンドウの範囲内のみにした方が効率的。
// 	D3DVIEWPORT9 vp;
// 	vp.X  = 0;
// 	vp.Y  = 0;
// 	vp.Width = D3dPP.BackBufferWidth;
// 	vp.Height = D3dPP.BackBufferHeight;
// 	vp.MinZ  = 0.0f;
// 	vp.MaxZ  = 1.0f;
// 	Direct3DDevice->SetViewport(&vp);

// 	if( SUCCEEDED(Direct3DDevice->BeginScene()) ) {
// 		struct CAutoEndSceneCall {
// 			IDirect3DDevice9*	m_Device;
// 			CAutoEndSceneCall( IDirect3DDevice9* device ) : m_Device(device) {}
// 			~CAutoEndSceneCall() { m_Device->EndScene(); }
// 		} autoEnd(Direct3DDevice);

// 		if( BackBufferDirty ) {
// 			Direct3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0, 0 );
// 			BackBufferDirty = false;
// 		}

// 		//- draw as triangles
// 		if( FAILED(hr = Direct3DDevice->SetTexture(0, Texture)) )
// 			goto got_error;

// 		if( FAILED( hr = Direct3DDevice->SetFVF( D3DFVF_XYZRHW|D3DFVF_TEX1 ) ) )
// 			goto got_error;

// 		if( FAILED( hr = Direct3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(tVertices) ) ) )
// 			goto got_error;

// 		if( FAILED( hr = Direct3DDevice->SetTexture( 0, NULL) ) )
// 			goto got_error;
// 	}


}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPBasicDrawDevice::SetShowUpdateRect(bool b)
{
	DrawUpdateRectangle = b;
}
//---------------------------------------------------------------------------
bool TJS_INTF_METHOD tTVPBasicDrawDevice::SwitchToFullScreen( HWND window, tjs_uint w, tjs_uint h, tjs_uint bpp, tjs_uint color, bool changeresolution )
{
	// フルスクリーン化の処理はなにも行わない、互換性のためにウィンドウを全画面化するのみで処理する
	// Direct3D9 でフルスクリーン化するとフォーカスを失うとデバイスをロストするので、そのたびにリセットor作り直しが必要になる。
	// モーダルウィンドウを使用するシステムでは、これは困るので常にウィンドウモードで行う。
	// モーダルウィンドウを使用しないシステムにするのなら、フルスクリーンを使用するDrawDeviceを作ると良い。
	// BackBufferDirty = true;
	// ShouldShow = true;
	// CheckMonitorMoved();
	assert(false);//todo
	return true;
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPBasicDrawDevice::RevertFromFullScreen( HWND window, tjs_uint w, tjs_uint h, tjs_uint bpp, tjs_uint color )
{
	// BackBufferDirty = true;
	// ShouldShow = true;
	// CheckMonitorMoved();
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
// tTJSNI_BasicDrawDevice : BasicDrawDevice TJS native class
//---------------------------------------------------------------------------
tjs_uint32 tTJSNC_BasicDrawDevice::ClassID = (tjs_uint32)-1;
tTJSNC_BasicDrawDevice::tTJSNC_BasicDrawDevice() :
	tTJSNativeClass(TJS_W("BasicDrawDevice"))
{
	// register native methods/properties

	TJS_BEGIN_NATIVE_MEMBERS(BasicDrawDevice)
	TJS_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
// constructor/methods
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL(/*var.name*/_this, /*var.type*/tTJSNI_BasicDrawDevice,
	/*TJS class name*/BasicDrawDevice)
{
	return TJS_S_OK;
}
TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/BasicDrawDevice)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/recreate)
{
	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_BasicDrawDevice);
	_this->GetDevice()->SetToRecreateDrawer();
	_this->GetDevice()->EnsureDevice();
	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/recreate)
//----------------------------------------------------------------------


//---------------------------------------------------------------------------
//----------------------------------------------------------------------
// properties
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(interface)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_BasicDrawDevice);
		*result = reinterpret_cast<tjs_int64>(_this->GetDevice());
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_DENY_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(interface)
//----------------------------------------------------------------------
	TJS_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------
iTJSNativeInstance *tTJSNC_BasicDrawDevice::CreateNativeInstance()
{
	return new tTJSNI_BasicDrawDevice();
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tTJSNI_BasicDrawDevice::tTJSNI_BasicDrawDevice()
{
	Device = new tTVPBasicDrawDevice();
}
//---------------------------------------------------------------------------
tTJSNI_BasicDrawDevice::~tTJSNI_BasicDrawDevice()
{
	if(Device) Device->Destruct(), Device = NULL;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
	tTJSNI_BasicDrawDevice::Construct(tjs_int numparams, tTJSVariant **param,
		iTJSDispatch2 *tjs_obj)
{
	return TJS_S_OK;
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTJSNI_BasicDrawDevice::Invalidate()
{
	if(Device) Device->Destruct(), Device = NULL;
}
//---------------------------------------------------------------------------

