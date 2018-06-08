#include "TVPTimer.h"
#include "utils.h"
#include "SDL.h"

class TimerH:public TVPTimerEventIntarface
{
public:
	void Handle()
	{
		printf("in timer handle!!!:%d\n",GetTickCount());
	}
};

extern void init_sdl();
extern void sdl_loop();

int main()
{
	init_sdl();
	// printf("main tid:%lu\n",SDL_ThreadID());

	TVPTimer timer;
	timer.SetInterval(500);
	TimerH *th = new TimerH();
	timer.SetOnTimerHandler(th, &TimerH::Handle);

	sdl_loop();

	// SDL_Delay(2000);

	return 0;
}