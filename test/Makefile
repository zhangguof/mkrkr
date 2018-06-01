SRC_ROOT=../src/
CC=c++
LD=c++
LFALG= -L../libs/ -ltjs2 -lonig -lboost_filesystem -lboost_system
CFLAG=-g -I$(SRC_ROOT) -I$(SRC_ROOT)/tjs2 -I../../boost_1_67_0/

all: test

test: test_tjs_script.o
	$(LD) -o $@ $^ $(LFALG)



%.o: %.cpp
	$(CC) -c -o $@ $^ $(CFLAG)

# $(SRC_ROOT)/%.o: %.cpp
# 	$(CC) -c -o $@ $^ $(CFLAG)