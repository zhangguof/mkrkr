
#ifndef __NATIVE_EVENT_QUEUE_H__
#define __NATIVE_EVENT_QUEUE_H__
#include "sdlEvent.hpp"
#include <map>

// 呼び出されるハンドラがシングルスレッドで動作するイベントキュー

class NativeEvent//:public SDL_UserEvent
{
public:
	SDL_UserEvent _ev;
	void* param;
	int64_t _id;
	static int64_t gen_event_id(){
		static int64_t cur_id = 0;
		return cur_id++;
	}
	// NativeEvent(){
	// 	param = nullptr;
	// 	_id = gen_event_id();
	// 	init_sdl_ev(nullptr,Native_Event_Code);
	// }


	NativeEvent(void* func=nullptr,void* data=nullptr,
				int _code=Native_Event_Code)
	{
		param = data;
		_id = gen_event_id();
		init_sdl_ev(func, _code);
	}
	NativeEvent(NativeEvent& e)
	{
		_id = gen_event_id();
		param = e.param;
		init_sdl_ev(_ev.data1,e.get_code());
		e._id = -1;
	}

	void init_sdl_ev(void* f, int _code)
	{
		_ev.data1 = f; // data1 = NativeEventQueue*
		_ev.data2 = reinterpret_cast<void*>(_id);
		// _ev.data2 = this;
		_ev.code = _code;
		_ev.type = SDL_USEREVENT;
	}
	void* get_data1(){return _ev.data1;}
	void set_data1(void* f){_ev.data1 = f;}
	int get_code(){return _ev.code;}
	void set_code(int code){_ev.code = code;}
	SDL_UserEvent& get_event()
	{
		// return *this;
		return _ev;
	}
};

class NativeEventQueueIntarface {
public:
	// デフォルトハンドラ
	virtual void HandlerDefault( class NativeEvent& event ) = 0;

	// Queue の生成
	virtual void Allocate() = 0;

	// Queue の削除
	virtual void Deallocate() = 0;

	virtual void Dispatch( class NativeEvent& event ) = 0;

	virtual void PostEvent( NativeEvent event ) = 0;
	// virtual void PopEvent(int64_t e_id) = 0;
};
typedef std::shared_ptr<NativeEvent> tPtrNativeEvent;

class NativeEventQueueImplement : public NativeEventQueueIntarface {
	// HWND window_handle_;
	// WNDCLASSEX	wc_;
	void* handler;
	friend void process_native_event(void* p_imp, SDL_UserEvent& e);
	std::map<int64_t,tPtrNativeEvent>  ev_map;
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

	void PostEvent(NativeEvent e);

	void PopEvent(int64_t e_id);
	int get_event_num(){return ev_map.size();}

	void* GetOwner() { return handler; }
	tPtrNativeEvent find_event(int64_t eid);
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
	void* GetOwner()
	{
		return (void*) owner_;
	}
};

#endif // __NATIVE_EVENT_QUEUE_H__
