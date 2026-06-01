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

## Build System
The build system uses `smake` with two key configuration files in `build/inc/`:

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
* No specific testing instructions found