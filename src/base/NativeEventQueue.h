
#ifndef __NATIVE_EVENT_QUEUE_H__
#define __NATIVE_EVENT_QUEUE_H__
#include "sdlEvent.hpp"

// 呼び出されるハンドラがシングルスレッドで動作するイベントキュー

class NativeEvent:public SDL_UserEvent
{
public:

	NativeEvent(){
		data1 = nullptr;
		data2 = nullptr;
		code = Native_Event_Code;
		type = SDL_USEREVENT;
	}

	NativeEvent(void* func,void* data)
	{
		data1 = func;
		data2 = data;
		code = Native_Event_Code;
		type = SDL_USEREVENT;
	}
	SDL_UserEvent& get_event()
	{
		return *this;
	}
};

class NativeEventQueueIntarface {
public:
	// デフォルトハンドラ
	// virtual void HandlerDefault( class NativeEvent& event ) = 0;

	// Queue の生成
	virtual void Allocate() = 0;

	// Queue の削除
	virtual void Deallocate() = 0;

	virtual void Dispatch( class NativeEvent& event ) = 0;

	virtual void PostEvent( NativeEvent& event ) = 0;
};

class NativeEventQueueImplement : public NativeEventQueueIntarface {
	// HWND window_handle_;
	// WNDCLASSEX	wc_;
	void* handler;
	friend void process_native_event(void* p_imp, SDL_UserEvent& e);

	// int CreateUtilWindow();
	static void WndProc(NativeEvent& event);

public:
	NativeEventQueueImplement() : handler(nullptr) {}

	// デフォルトハンドラ
	void HandlerDefault( NativeEvent& event );

	// Queue の生成
	void Allocate();

	// Queue の削除
	void Deallocate();

	void PostEvent( NativeEvent& event );

	void* GetOwner() { return handler; }
};

//native queue in thread x posevent, 
//main thread handle the envet.

template<typename T>
class NativeEventQueue : public NativeEventQueueImplement {
	void (T::*handler_)(NativeEvent&);
	T* owner_;

public:
	NativeEventQueue( T* owner, void (T::*Handler)(NativeEvent&) ) : owner_(owner), handler_(Handler) {}

	void Dispatch( NativeEvent &ev ) {
		(owner_->*handler_)(ev);
	}
};

#endif // __NATIVE_EVENT_QUEUE_H__
