SRC_ROOT=../src/
CC=c++
LD=c++
LFALG= -L../libs/ -ltjs2 -lonig
CFLAG=-g -I$(SRC_ROOT) -I$(SRC_ROOT)/tjs2

all: test

test: test_tjs_script.o
	$(LD) -o $@ $^ $(LFALG)



%.o: %.cpp
	$(CC) -c -o $@ $^ $(CFLAG)