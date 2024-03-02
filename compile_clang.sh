#!/bin/sh
#

mkdir bin 2>/dev/null
rm -f bin/rehlds_sq.so 2>/dev/null

clang -s -g0 -O3 -msse -msse2 -msse3 -msse4.1 -msse4.2 -fno-exceptions -fno-rtti -fno-stack-protector -m32 -fno-rtti -mcrc32 -fPIC -s -shared -static-libgcc \-Isrc \-Idep/hlsdk/common -Idep/hlsdk/dlls -Idep/hlsdk/engine -Idep/hlsdk/public \-Idep/metamod \-Idep/steam \src/*.cpp \-o bin/clang/rehlds_sq.so
	

