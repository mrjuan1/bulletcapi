CFLAGS=-c -I$(BULL) -O3 -pipe -m64
CFLAGS+=-ftree-vectorize -ffast-math -funroll-loops
#CFLAGS+=-Wfatal-errors

all: test test-shared

include bullet.mk

install: all
	@mkdir -pv ../include ../lib
	@cp -v capi.h ../include
	@cp -v lib/libbullet.so lib/libcapi.so ../lib
	@strip -s ../lib/libbullet.so ../lib/libcapi.so

clean:
	@rm -fv test test-shared
	@rm -fv obj/test.o obj/capi.o
	@rm -fv lib/libcapi.a lib/libcapi.so

distclean: clean
	@rm -fv obj/*.o
	@rm -fv lib/libbullet.a lib/libbullet.so
	@rm -Rfv ../include ../lib

obj/capi.o: capi.cpp
	g++ -c $< -o $@ $(CFLAGS) -fPIC

lib/libcapi.a: obj/capi.o
	ar rcs $@ $<

lib/libcapi.so: obj/capi.o
	g++ $< -o $@ -shared -Wl,-soname,libcapi.so

obj/test.o: test.c
	gcc -c $< -o $@ -nocpp $(CFLAGS)

test: obj/test.o lib/libcapi.a lib/libbullet.a
	g++ $^ -o $@

test-shared: obj/test.o lib/libbullet.so lib/libcapi.so
	gcc $< -o $@ -Llib -Wl,-rpath,lib -lbullet -lcapi
