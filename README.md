# SClass

Classes in C++(>=11) for building cross-platform applications.
See [SClassDemo](http://github.com/sswroom/SClassDemo) for sample usage

You need to generate ThirdParty files after getting file from git:
cd SClass
git submodule update --init
cd ThirdParty/miniz
mkdir _build
cd _build
cmake ..

For Windows version, please download [ffmpeg-master-latest-win64-gpl-shared.zip](https://github.com/BtbN/FFmpeg-Builds/releases/tag/latest) and extract to same directory as SClass.

It uses 'smake' as the build system. smake can be compiled as follows:

In linux (x64)
```
cd build/Linux_x64
make smake
```

In Windows (x64)
```
cd build/Windows_x64
mingw32-make smake
```

In FreeBSD (x64)
```
cd build/FreeBSD_x64
gmake smake
```


## Directory Description
### build
It is the directory for compiling to different. cc_xxxx means it is cross-compiled target, most likely it is compiling from Linux x64 platform.

### doc
It is the directory of document generated using doxygen

### header
It is the directory for all header files.

### main
It is the directory for program entry.

### src
It is the directory for source files. (C/C++/Assembly)

### startup
It is the directory for startup file.

### ThirdParty
It is the directory for ThirdParty libraries source/header files.


## Naming Convention
### File Name
*_C.h - C-Styled function, all function should be named as *_xxx
*.hpp - template class with body