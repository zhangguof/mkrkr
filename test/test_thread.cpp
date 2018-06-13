// #include "TVPTimer.h"
#include <string>
#include "tjsCommHead.h"
#include "ThreadIntf.h"
#include "utils.h"
#include "SDL.h"
#include "win_def.h"
#include "NativeEventQueue.h"

// 
extern void init_sdl();
extern void sdl_loop();

void paratia_test(int* arg)
{
	int cnt = *arg;
	printf("====start test:%d\n",cnt);
	while(cnt>0)
	{
		printf("int paratia_test:%d\n",cnt);
		cnt--;
	}
	printf("====end test:%d\n",cnt);
}

class TestThread:public tTVPThread
{
public:
	NativeEventQueue<TestThread>* event_q;

	tTVPThreadEvent* event;
	TestThread(bool s,tTVPThreadEvent* e):tTVPThread(s){
		event = e;
		event_q = new NativeEventQueue<TestThread>(this,
		&TestThread::event_handle);

	}
	void event_handle(NativeEvent& e)
	{
		// int* p = (int*)e.data2;
		int p = reinterpret_cast<uint64_t>(e.data2);
		printf("handle in %lu,get num:%d\n",
		SDL_ThreadID(),p);
		Sleep(2000);
	}
	void Execute()
	{
		printf("test event queue!!!\n");
		printf("in thread1:%lu\n", SDL_ThreadID());
		int cnt = 5;
		int param = 0;
		NativeEvent e;
		while (cnt--)
		{
			e.data2 = reinterpret_cast<void*>(param);
			event_q->PostEvent(e);
			printf("==post event\n");
			Sleep(500);
			param++;
		}
		printf("end thread1 !!!\n");
	}
	void Execute1()
	{
		printf("in thread1:%lu\n", SDL_ThreadID());
		printf("in thread1:wait for thread2\n");
		bool r = event->WaitFor(3000);
		if(r)
			printf("in thread1:WaitFor ok:\n");
		else
			printf("in thread1:WaitFor timeout!\n");
		// event->Reset();
		Sleep(5000);
		event->Set();
		printf("end thread1:%lu\n", GetThreadId());

	}
	~TestThread()
	{
		delete event_q;
	}
};

class TestThread2:public tTVPThread
{
public:
	tTVPThreadEvent* event;
	TestThread2(bool s,tTVPThreadEvent* e):tTVPThread(s){
		event = e;
	}

	void Execute1()
	{
		printf("in thread2:%lu\n", SDL_ThreadID());
		Sleep(2000);
		event->Set();
		Sleep(1000);
		printf("in thread2:wait for thread1 end\n");
		event->WaitFor(-1);
		printf("end thread2:%lu\n", SDL_ThreadID());
	}
};

int main()
{
	init_sdl();

   //  tjs_int taskNum = TVPGetThreadNum();
   //  printf("taskNum:%d\n",taskNum);
   //  TVPBeginThreadTask(taskNum);
   // 	int params[TVPMaxThreadNum];
   // 	for (tjs_int i = 0; i < taskNum; i++) {
   //   int *param = params + i;
   //   *param = 10*(i+1)/taskNum;

   //   TVPExecThreadTask((TVP_THREAD_TASK_FUNC)paratia_test, TVP_THREAD_PARAM(param));
   // }
   // TVPEndThreadTask();

   tTVPThreadEvent event;
   printf("in main thread:%lu\n", SDL_ThreadID());

   TestThread t1(true,&event);
   // TestThread2 t2(true,&event);
   t1.Resume();
   // t2.Resume();
   
   // t2.WaitFor();
  


	sdl_loop();
	t1.WaitFor();
	printf("end all!\n");

	// SDL_Delay(2000);

	return 0;
}