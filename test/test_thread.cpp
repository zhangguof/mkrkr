// #include "TVPTimer.h"
#include <string>
#include "tjsCommHead.h"
#include "ThreadIntf.h"
#include "utils.h"
#include "SDL.h"
#include "win_def.h"

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
	tTVPThreadEvent* event;
	TestThread(bool s,tTVPThreadEvent* e):tTVPThread(s){
		event = e;
	}
	void Execute()
	{
		printf("in thread1:%lu\n", GetThreadId());
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
};

class TestThread2:public tTVPThread
{
public:
	tTVPThreadEvent* event;
	TestThread2(bool s,tTVPThreadEvent* e):tTVPThread(s){
		event = e;
	}
	void Execute()
	{
		printf("in thread2:%lu\n", GetThreadId());
		Sleep(2000);
		event->Set();
		Sleep(1000);
		printf("in thread2:wait for thread1 end\n");
		event->WaitFor(-1);
		printf("end thread2:%lu\n", GetThreadId());
	}
};

int main()
{
	init_sdl();

    tjs_int taskNum = TVPGetThreadNum();
    printf("taskNum:%d\n",taskNum);
    TVPBeginThreadTask(taskNum);
   	int params[TVPMaxThreadNum];
   	for (tjs_int i = 0; i < taskNum; i++) {
     int *param = params + i;
     *param = 10*(i+1)/taskNum;

     TVPExecThreadTask((TVP_THREAD_TASK_FUNC)paratia_test, TVP_THREAD_PARAM(param));
   }
   TVPEndThreadTask();

   tTVPThreadEvent event;

   TestThread t1(true,&event);
   TestThread2 t2(true,&event);
   t1.Resume();
   t2.Resume();
   t1.WaitFor();
   t2.WaitFor();
   printf("end all!\n");


	// sdl_loop();

	// SDL_Delay(2000);

	return 0;
}