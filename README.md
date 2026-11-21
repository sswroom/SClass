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

It uses 'smake' as the build system. See [doc/smake.md](doc/smake.md) for
smake usage and configuration syntax. smake can be compiled as follows:

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

## PDF parser tests

In the Linux debug build, run `smake PDFParserTest` followed by
`./bin/PDFParserTest --self-test` to run the synthetic PDF regression tests.
Running without arguments scans the configured local PDF corpus.

Raw ICCBased PDF images support RGB matrix/TRC profiles with default component
ranges and Decode arrays, at 1, 2, 4, 8, or 16 bits per component. Samples
are stored as 8-bit RGB. Other ICCBased profiles or non-default ranges and
Decode arrays are reported as unsupported rather than being silently
misinterpreted.


## Directory Description
### build
It is the directory for compiling to different. cc_xxxx means it is cross-compiled target, most likely it is compiling from Linux x64 platform.

### cppdoc
It is the directory of document generated using doxygen

### doc
It is the directory of document in md format for detailed description

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