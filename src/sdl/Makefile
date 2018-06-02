SDL_ROOT = ../../../sdl/SDL2-2.0.8/include
# LIBJPEG  = ../tools/libjpeg-turbo-1.5.3
CC = c++
CFLAG = -g -I${SDL_ROOT} -I./  -std=c++11  -DBMP_SUPPORTED -DPPM_SUPPORTED
# CFLAG2 = -g -I${SDL_ROOT} -I./ -I${LIBJPEG} -DBMP_SUPPORTED -DPPM_SUPPORTED
LD = c++
#frameworks = ['OpenGL','CoreFoundation','Cocoa','IOKit','CoreVideo']
LFALG = -arch x86_64 \
        -L../../libs -L/usr/local/lib -lSDL2 -lGLEW \
        -lsoil\
        -lm -liconv -framework CoreAudio \
        -framework OpenGL  \
        -framework AudioToolbox \
        -framework ForceFeedback -lobjc \
        -framework CoreVideo \
        -framework Cocoa -framework Carbon \
        -framework IOKit -framework Metal

all: test

test: test.o GLDevice.o load_bmp.o gltexture.o
	$(LD) -o $@ $^ $(LFALG)


%.o: %.cpp
	$(CC) -c -o $@ $^ $(CFLAG)

# $(LIBJPEG)/%.o: $(LIBJPEG)/%.c
# 	cc -c -o $@ $^ $(CFLAG2)



clean:
	rm *.o
	rm test
