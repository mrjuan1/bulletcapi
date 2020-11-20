# Bullet C API

## Why

I wanted to use Bullet Physics in C (not a C++ fan).

## But doesn't Bullet already have a C API

Yes, but alas, it's tightly enmeshed with the rest of the example code (like the exampleBrowser). I want to just use the core code of Bullet that I need and also provide my own graphics routines.

## So what's the point

The primary goal of this C API is to provide the core functionality of Bullet without weighing it down with unrelated graphics or other utility code.

## Instructions

Start off in the `capi` sub directory.

Compiling this project will provide you with a textual demonstration and also a static library of the parts of Bullet the C API is currently using. In addition, this is where the C header lives (`capi.h`).

In order for the Makefile to work you will have to locate where you have extracted the Bullet SDL. Look inside `bullet.mk`:

```makefile
# root of bullet source code
BULL=../../bullet3/src/
```

Adjust this path to point to the source directory of Bullet. This part of the build system is possibly less than optimal but it does ensure complete independence from the Bullet build system.

Once you have verified that the `capi` example works (`test`), you can move onto the simple graphical example (`gluTest`).

Alternatively if you are not bothered about playing with the console test, you can just run the following:

```sh
make lib/libbullet.a
```

In addition to `libbullet.a`, you will need `capi.o` (compiled from `capi.cpp`).

## `gluTest`

This is a horror. No truly, it's NOT an example of how you should produce graphics.

That said it's a minimum amount of code that can show some kind of 3D graphics, without there being the confusion of a mini graphics engine with model loaders, shaders and lots of other things that could enmesh it into a very specific use case.

This example does show the use of some basic modifications to a body, such as changing basic properties like friction and restitution to make a pleasing demonstration.

Outside this folder, `gluTest` links in `libbullet.a` and compiles `capi.cpp`, then links that to `main.o`, the C frontend of this example.

## `GL3v3`

While a better (though probably not ideal) example of producing 3D graphics, this does add a bunch of extra clutter (theres way more OpenGL and support stuff than actual Bullet use). This example is where new features are introduced. You will need to adjust the Makefile for its dependency ([https://github.com/Kazade/kazmath](https://github.com/Kazade/kazmath)). If you clone it to a location alongside `bulletcapi`, you should be good to go.

## `cppHello`

This is a simple CPP reference example. Building this with libraries compiled by Bullet's own build system could lead to difficult to diagnose memory corruption (hence I decided to implement `bullet.mk`). It was initially used to provide a reference design that I used to begin writing `capi.cpp` and `capi.hpp` which constitutes the C++ layer between the C front end and the C++ backend. This will be removed before long as the "wrapper" is already more fully featured than this example.

## The API

Please see the Doxygen docs.

## Conclusion

These are very early days. There is lots to do and any contributions, especially examples, would be most appreciated.If an example needs some missing functionality please post an issue.
