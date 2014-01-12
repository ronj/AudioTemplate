AudioTemplate
=============

Template for realtime audio processing including MP3 decoding support.

Building
========

Make sure a compiler is used that supports C++11 features! Tested on >= GCC4.7 and Clang/LLVM 5.0 under OSX.

mkdir -p build/{release,debug}
cd build/debug && cmake -DCMAKE_BUILD_TYPE=Debug && cd -
cd build/release && cmake -DCMAKE_BUILD_TYPE=Release && cd -
cd build/debug && make

If you have a multi-core machine you may append -jX (X = num cores + 1) to make e.g. -j5 for a quad core machine.
To use a different compiler prepend the cmake command with for example CC=/usr/bin/gcc-4.7 CXX=/usr/bin/g++-4.7.
