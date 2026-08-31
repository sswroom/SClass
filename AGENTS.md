# Project: API framework
​
This is a C++ framework support multi-platform and minimum dependencies.
​
## Structure
- `build/` - Build configurations:
  - Native build directories (e.g., Linux_dbg_x64, Windows_x64) for building and running on the same platform
  - Cross-compile directories (e.g., cc_windows_x64, cc_Linux_ARM) where you build on one platform to run on another
  - Native build directories contain both Makefile and SMake.cfg
  - Cross-compile directories contain SMake.cfg only (Makefiles in cc_* directories are old residuals and not maintained)
  - Makefile in native build directories is used to build applications with smake, and can also build smake itself with "make smake"
- `header/` - Header files
- `src/` - Source files (including platform-specific code in subsystem/*_xxx.cpp or subsystem/*_xxx.asm where _xxx represents subsystem/device names like Win64, SysV64, MT7620, etc.)
- `Java/` - Java sources for Java platform
- `main/` - Source files for each program entry point (executables)
- `startup/` - Platform entry point code
- `ThirdParty/` - Embedded third-party libraries
​
## Conventions
- *_C.h are files with C-style function syntax_
- *.hpp are files with function body in headers
- build/Linux_dbg_x64 is the default build and test environment
​
## Commands
* smake all: Build all programs and do self test
* smake test: Build all test objects

## Build System
The build system uses `smake` which loads `SMake.cfg` in the current directory first, then includes other files specified in it.
The two key configuration files are `smake_common` and `smake_objects` (typically found in `build/inc/` and included via the SMake.cfg chain).
- Objects are only compiled if referenced in code or explicitly requested via `smake ObjectName`

### smake_common
- Contains header file declarations
- Format:
  ```
  Path/Header.h:
  @ObjectName.o
  ```

### smake_objects  
- Contains source-to-object mapping with dependencies
- Format:
  ```
  ObjectName.o: ../../src/Path/Source.cpp
  $`pkg-config --cflags libpq`
  !`pkg-config --libs libpq`
  ```
- Libraries are specified with `!` prefix

### Adding new source files:
1. Add header declaration to `smake_common`
2. Add build rule to `smake_objects` including any dependencies
3. Objects are only compiled if referenced in code or explicitly requested via `smake ObjectName`

## Testing
* Run `smake test` to build all test objects
* After building, programs are located at ./bin/, object files are located at ./obj/ (relative to the build directory)
* Individual test programs can be run directly from the ./bin/ directory after building