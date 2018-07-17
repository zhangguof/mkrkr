//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Thread base class
//---------------------------------------------------------------------------
#define NOMINMAX
#include "tjsCommHead.h"

// #include <process.h>
#include <algorithm>

#include "ThreadIntf.h"
#include "ThreadImpl.h"
#include "MsgIntf.h"
#include <atomic>
#include "SDL.h"
#include "win_def.h"
#include "utils.h"

// SDL_Thread* SDL_CreateThread(SDL_ThreadFunction fn,
//                              const char*        name,
//                              void*              data)

//---------------------------------------------------------------------------
// tTVPThread : a wrapper class for thread
//---------------------------------------------------------------------------
tTVPThread::tTVPThread(bool _suspended)
{
	Terminated = false;
	Handle = NULL;
	ThreadId = 0;
	suspended = _suspended;
	_Priority = SDL_THREAD_PRIORITY_NORMAL; //normal
	if(!_suspended)
	{
		do_create();
	}

	// Handle = (HANDLE) _beginthreadex(
	// 	NULL, 0, StartProc, this, suspended ? CREATE_SUSPENDED : 0,
	// 		(unsigned *)&ThreadId);

	// if(Handle == INVALID_HANDLE_VALUE) TVPThrowInternalError;
}
void tTVPThread::do_create()
{
	//real do create thread here;
	Handle = SDL_CreateThread(tTVPThread::StartProc,
	"tTVPThread",
	this);
	if(Handle == NULL)
	{
		printf("SDL_CreateThread failed: %s\n", SDL_GetError());
	}

}
//---------------------------------------------------------------------------
tTVPThread::~tTVPThread()
{
	// CloseHandle(Handle);
}
//---------------------------------------------------------------------------
int tTVPThread::StartProc(void * arg)
{
	tTVPThread* _this = (tTVPThread*)arg;
	_this->ThreadId = SDL_ThreadID();
	(_this)->Execute();
	return 0;
}
//---------------------------------------------------------------------------
void tTVPThread::WaitFor()
{
	int ret;
	assert(Handle);
	SDL_WaitThread(Handle,&ret);
	// printf("Thread returned value: %d", ret);
	// WaitForSingleObject(Handle, INFINITE);
}
//---------------------------------------------------------------------------
tTVPThreadPriority tTVPThread::GetPriority()
{
	// int n = GetThreadPriority(Handle);
	switch(_Priority)
	{
	case SDL_THREAD_PRIORITY_LOW:			return ttpIdle;
	case SDL_THREAD_PRIORITY_NORMAL:		return ttpNormal;
	case SDL_THREAD_PRIORITY_HIGH:			return ttpTimeCritical;
	}

	return ttpNormal;
}
//---------------------------------------------------------------------------
void tTVPThread::SetPriority(tTVPThreadPriority pri)
{
	SDL_ThreadPriority npri = SDL_THREAD_PRIORITY_NORMAL;
	switch(pri)
	{
	case ttpIdle:			npri = SDL_THREAD_PRIORITY_LOW;			break;
	case ttpLowest:			npri = SDL_THREAD_PRIORITY_LOW;			break;
	case ttpLower:			npri = SDL_THREAD_PRIORITY_LOW;			break;
	case ttpNormal:			npri = SDL_THREAD_PRIORITY_NORMAL;		break;
	case ttpHigher:			npri = SDL_THREAD_PRIORITY_NORMAL;		break;
	case ttpHighest:		npri = SDL_THREAD_PRIORITY_NORMAL;		break;
	case ttpTimeCritical:	npri = SDL_THREAD_PRIORITY_HIGH;		break;
	}
	if(SDL_SetThreadPriority(npri)<0)
	{
		printf("SetThreadPriority error:%s\n",SDL_GetError());
	}

	// SetThreadPriority(Handle, npri);
}
//---------------------------------------------------------------------------
void tTVPThread::Suspend()
{
	// SuspendThread(Handle);
}
//---------------------------------------------------------------------------
void tTVPThread::Resume()
{
	// while((tjs_int32)ResumeThread(Handle) > 1) ;
	if(!Handle)
	{
		suspended = false;
		do_create();
	}
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
// tTVPThreadEvent
//---------------------------------------------------------------------------
tTVPThreadEvent::tTVPThreadEvent(bool _manualreset)
{
	// Handle = CreateEvent(NULL, manualreset?TRUE:FALSE, FALSE, NULL);
	manualreset = _manualreset;
	condition = false;
	lock = SDL_CreateMutex();
	cond = SDL_CreateCond();
	if(!lock || !cond)
	{
		printf("tTVPThreadEvent error:%s\n",SDL_GetError());
		TVPThrowInternalError;
	}

	// if(!Handle) TVPThrowInternalError;
}
//---------------------------------------------------------------------------
tTVPThreadEvent::~tTVPThreadEvent()
{
	// CloseHandle(Handle);
	SDL_DestroyCond(cond);
	SDL_DestroyMutex(lock);
}
//---------------------------------------------------------------------------
void tTVPThreadEvent::Set()
{
	SDL_LockMutex(lock);
	condition = true;
	if(manualreset)
		SDL_CondSignal(cond);
	else
		SDL_CondBroadcast(cond);
    SDL_UnlockMutex(lock);
	// SetEvent(Handle);

}
//---------------------------------------------------------------------------
void tTVPThreadEvent::Reset()
{
	// ResetEvent(Handle);
	SDL_LockMutex(lock);
	condition = false;
	SDL_UnlockMutex(lock);

}
//---------------------------------------------------------------------------
bool tTVPThreadEvent::WaitFor(tjs_uint timeout)
{
	// wait for event;
	// returns true if the event is set, otherwise (when timed out) returns false.
	SDL_LockMutex(lock);
	int status = 0;

	if(timeout == INFINITE)
	{
		if(!condition)
		{
			status = SDL_CondWait(cond,lock);
		}
	}
	else
	{
		if(!condition)
		{
			status = SDL_CondWaitTimeout(cond, lock, timeout);
		}
	}
	if(!manualreset && status==0)
	{
		condition = false;
	}
	SDL_UnlockMutex(lock);
	// DWORD state = WaitForSingleObject(Handle, timeout == 0 ? INFINITE : timeout);

	// if(state == WAIT_OBJECT_0) return true;
	if(status == 0)
		return true;

	if(status<0)
	{
		printf("WaitFor error:%s\n", SDL_GetError());
	}
	return false;
	// return false;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tjs_int TVPDrawThreadNum = 1;

enum ThreadStatus
{
	Runing,
	Stop,
};
struct ThreadInfo {
  bool readyToExit;
  SDL_Thread* thread;
  TVP_THREAD_TASK_FUNC  lpStartAddress;
  TVP_THREAD_PARAM lpParameter;
  ThreadStatus status;

};
static std::vector<ThreadInfo*> TVPThreadList;
// static std::vector<tjs_int> TVPProcesserIdList;
static int ThreadLoop(void* p);

void resume_thread(ThreadInfo* threadInfo)
{
	threadInfo->status = Runing;
	if(threadInfo->thread == NULL)
	{
		//create
		threadInfo->thread = SDL_CreateThread(
		ThreadLoop,"thread pool",
		threadInfo);
		if(threadInfo->thread == NULL)
		{
			printf("SDL_CreateThread failed: %s\n", SDL_GetError());
		}
	}
}

// static LONG TVPRunningThreadCount = 0;
std::atomic<long> TVPRunningThreadCount(0);


static tjs_int TVPThreadTaskNum, TVPThreadTaskCount;

//---------------------------------------------------------------------------
static tjs_int GetProcesserNum(void)
{
  static tjs_int processor_num = 0;
  if (! processor_num) {
    // SYSTEM_INFO info;
    // GetSystemInfo(&info);
    // processor_num = info.dwNumberOfProcessors;
    processor_num = SDL_GetCPUCount();
  }
  return processor_num;
}

tjs_int TVPGetProcessorNum(void)
{
  return GetProcesserNum();
}

//---------------------------------------------------------------------------
tjs_int TVPGetThreadNum(void)
{
  tjs_int threadNum = TVPDrawThreadNum ? TVPDrawThreadNum : GetProcesserNum();
  threadNum = std::min(threadNum, TVPMaxThreadNum);
  return threadNum;
}

//---------------------------------------------------------------------------
static int ThreadLoop(void* p)
{
  ThreadInfo *threadInfo = (ThreadInfo*)p;
  for(;;) {
    if (threadInfo->readyToExit)
      break;
  	while(threadInfo->status == Stop)
  		Sleep(200);
    (threadInfo->lpStartAddress)(threadInfo->lpParameter);
    // InterlockedDecrement(&TVPRunningThreadCount);
    TVPRunningThreadCount--;
    // SuspendThread(GetCurrentThread());
    threadInfo->status = Stop;
  }
  delete threadInfo;
  // ExitThread(0);

  return 1;
}
//---------------------------------------------------------------------------
void TVPBeginThreadTask(tjs_int taskNum)
{
  TVPThreadTaskNum = taskNum;
  TVPThreadTaskCount = 0;
  tjs_int extraThreadNum = TVPGetThreadNum() - 1;

//ensure list size == thread_num -1?
  while ( static_cast<tjs_int>(TVPThreadList.size()) < extraThreadNum) 
  {
    ThreadInfo *threadInfo = new ThreadInfo();
    threadInfo->readyToExit = false;
    // threadInfo->thread = CreateThread(NULL, 0, ThreadLoop, threadInfo, CREATE_SUSPENDED, NULL);
    //CREATE_SUSPENDED
    threadInfo->thread = NULL;//SDL_CreateThread()
    threadInfo->status = Stop;
    // SetThreadIdealProcessor(threadInfo->thread, TVPProcesserIdList[TVPThreadList.size() % TVPProcesserIdList.size()]);
    TVPThreadList.push_back(threadInfo);
  }
  while ( static_cast<tjs_int>(TVPThreadList.size()) > extraThreadNum) {
    ThreadInfo *threadInfo = TVPThreadList.back();
    threadInfo->readyToExit = true;
    // while (ResumeThread(threadInfo->thread) == 0)
    //   Sleep(0);
    resume_thread(threadInfo);
    // threadInfo->status = Runing;
    TVPThreadList.pop_back();
  }
}

//---------------------------------------------------------------------------
void TVPExecThreadTask(TVP_THREAD_TASK_FUNC func, TVP_THREAD_PARAM param)
{
  if (TVPThreadTaskCount >= (TVPThreadTaskNum - 1)) {
    func(param);
    return;
  }    
  ThreadInfo *threadInfo;
  //assert(TVPThreadTaskCount<TVPThreadList.size())
  threadInfo = TVPThreadList[TVPThreadTaskCount++];
  threadInfo->lpStartAddress = func;
  threadInfo->lpParameter = param;
  // InterlockedIncrement(&TVPRunningThreadCount);
  TVPRunningThreadCount++;
  resume_thread(threadInfo);
  // while (ResumeThread(threadInfo->thread) == 0)
  //   Sleep(0);
  // threadInfo->status = Runing;
}
//---------------------------------------------------------------------------
void TVPEndThreadTask(void) 
{
  // while ((LONG)InterlockedCompareExchange(&TVPRunningThreadCount, 0, 0) != 0)
    // Sleep(0);
    while(TVPRunningThreadCount.load() != 0)
    {
    	Sleep(500);
    }
}

//---------------------------------------------------------------------------
