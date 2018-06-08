//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Thread base class
//---------------------------------------------------------------------------
#ifndef ThreadImplH
#define ThreadImplH
#include "tjsNative.h"
#include "ThreadIntf.h"
#include "SDL.h" // implent by SDL_Thread;

#define INFINITE (-1)
//---------------------------------------------------------------------------
// tTVPThread
//---------------------------------------------------------------------------
class tTVPThread
{
	bool Terminated;
	// HANDLE Handle;
	SDL_Thread* Handle;
	SDL_threadID ThreadId;
	bool suspended; //stop?

	static int StartProc(void * arg);

public:
	tTVPThread(bool _suspended);
	void do_create();
	virtual ~tTVPThread();

	bool GetTerminated() const { return Terminated; }
	void SetTerminated(bool s) { Terminated = s; }
	void Terminate() { Terminated = true; }

protected:
	virtual void Execute() = 0;

public:
	void WaitFor();
	SDL_ThreadPriority _Priority;

	tTVPThreadPriority GetPriority();
	void SetPriority(tTVPThreadPriority pri);

	void Suspend();
	void Resume();

	SDL_Thread* GetHandle() const { return Handle; } 	
	SDL_threadID GetThreadId() const { return ThreadId; }  

};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTVPThreadEvent
//---------------------------------------------------------------------------
class tTVPThreadEvent
{
	// SDL_Thread* Handle;
	bool manualreset;
	SDL_mutex *lock;
	SDL_cond *cond;
	bool condition;

public:
	tTVPThreadEvent(bool _manualreset = false);
	virtual ~tTVPThreadEvent();

	void Set();
	void Reset();
	bool WaitFor(tjs_uint timeout);
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
#endif
