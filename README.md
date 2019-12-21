# Memory

This repository provides a simple memory game implemented in C using GTK+3.

## Compile

### Linux

In order to build the project you need a C compiler, cmake, and the development
package of GTK+3. On Ubuntu you can install all necessary packages with:
```
$ sudo apt install gcc cmake libc-dev libgtk-3-dev
```

You can compile the project using:
```
$ mkdir build
$ cd build/
$ cmake ..
$ make
```
This generates the binary memory.
