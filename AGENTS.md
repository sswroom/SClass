# Project: API framework
​
This is a C++ framework support multi-platform and minimum dependencies.
​
## Structure
- `build/` - Build config for each platform
- `header/` - Header files
- `src/` - Source files
- `Java/` - Java sources for Java platform
- `main/` - Source files for each program entry point
- `startup/` - Platform entry point
- `ThirdParty/` - Embedded third-party libraries
​
## Conventions
- *_C.h are files with C-style function syntax_
- *.hpp are files with function body in headers
- build/Linux_dbg_x64 is the default build and test environment
​
## Commands
* smake all: Build all programs and do self test

## Testing
* No specific testing instructions found