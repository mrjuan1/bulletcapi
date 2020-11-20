CFLAGS=-I../bullet3/src -I../include
CFLAGS+=-Og -g3 -pipe -m64 -march=native -mtune=native
CFLAGS+=-ftree-vectorize -ffast-math -funroll-loops
#CFLAGS+=-Wfatal-errors

LDFLAGS=../capi/lib/libbullet.a -lglut -lGL -lGLU

all: main main-shared

clean:
	@rm -fv main main-shared
	@rm -fv obj/capi.o obj/main.o

check: main
	valgrind --suppressions=valgind.suppression.txt --leak-check=full ./main

check-shared: main-shared
	valgrind --suppressions=valgind.suppression.txt --leak-check=full ./main-shared

obj/main.o: main.c
	gcc -c $< -o $@ -nocpp $(CFLAGS)

obj/capi.o: ../capi/capi.cpp
	g++ -c $< -o $@ $(CFLAGS) -fPIC

main: obj/main.o obj/capi.o
	g++ $^ -o $@ $(LDFLAGS)

main-shared: obj/main.o
	gcc $< -o $@ -lglut -lGL -lGLU -L../lib -Wl,-rpath,../lib -lbullet -lcapi
