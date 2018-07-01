#include "SDL.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>



//in test.cpp
//extern int init_core_test(int argc,char *argv[]);
//in sdlinit.cpp
//extern void sdl_loop();
extern int app_main(int argc,char* argv[]);

int main(int argc, char *argv[])
{
//    init_core_test(argc,argv);
	//do loop
//    sdl_loop();

    /* shutdown SDL */
//    SDL_Quit();

    return app_main(argc,argv);
}
