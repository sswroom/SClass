# Project: SClass Framework

This is a cross-platform C++ framework (C++11+) designed for building applications with minimum dependencies. It supports multiple platforms including Linux, Windows, macOS, FreeBSD, and various embedded systems.

## Structure
  
- `build/` - Build configurations:
  - Native build directories (e.g., Linux_dbg_x64, Windows_x64) for building and running on the same platform
  - Cross-compile directories (e.g., cc_windows_x64, cc_Linux_ARM) where you build on one platform to run on another
  - Native build directories contain both Makefile and SMake.cfg
  - Cross-compile directories contain SMake.cfg only (Makefiles in cc_* directories are old residuals and not maintained)
  - Makefile in native build directories is used to build applications with smake, and can also build smake itself with "make smake"
- `header/` - Header files organized by subsystem
- `src/` - Source files (.cpp, .c, .asm) organized by subsystem
- `Java/` - Java sources for Java platform bindings
- `main/` - Source files for each program entry point (executables)
- `startup/` - Platform-specific entry point code
- `ThirdParty/` - Embedded third-party libraries (miniz, etc.)
- `.opencode/` - Opencode configuration and skills

## Conventions

### File Naming
- `*_C.h` - Files with C-style function syntax (functions named `*_xxx`)
- `*.hpp` - Header files with function bodies (typically templates)
- Platform-specific source files use `*_xxx` naming where `_xxx` represents subsystem/device names like Win64, SysV64, MT7620, etc. (seen in files like GPIOControl_MT7620.cpp, ConsoleL.cpp, etc.)
- Source files use descriptive names matching their functionality

### Build System
The framework uses a custom build system called `smake` which loads `SMake.cfg` in the current directory first, then includes other files specified in it.
The two key configuration files are `smake_common` and `smake_objects` (typically found in `build/inc/` and included via the SMake.cfg chain):
- `smake_common` - Header file declarations and object groupings
- `smake_objects` - Source-to-object mapping with dependencies

### Default Build Environment
- `build/Linux_dbg_x64` is the default debug build and test environment for Linux x64
- After building here, programs are in ./bin/ and object files are in ./obj/

## Commands

### Building the Framework
```bash
# Build all programs and run self-test
smake all

# Build smake itself (if needed)
cd build/Linux_x64
make smake  # Linux
# or
cd build/Windows_x64
mingw32-make smake  # Windows
# or
cd build/FreeBSD_x64
gmake smake  # FreeBSD

# After building, programs are located at ./bin/, object files are located at ./obj/
# (relative to the build directory)
```

### Building Specific Targets with smake
All builds should be run from the appropriate platform-specific build directory (e.g., `build/Linux_dbg_x64`) where `SMake.cfg` exists.
After building, programs are located at ./bin/, object files are located at ./obj/ (relative to the build directory).
```bash
# Example: building in the Linux debug x64 environment
cd build/Linux_dbg_x64
# Build a specific object or program
smake TargetName

# Build all test objects (lowercase 'test')
smake test

# Build objects in a specific group
smake crypto  # Builds all objects in the crypto group
smake io      # Builds all objects in the io group
# etc.
```

### Using smake for all builds
All regular builds should use the smake build system. Traditional Makefiles in the repository are not maintained except for building smake itself.

## Build System Details

### smake_common
Contains header file declarations grouped by functionality. Format:
```
GroupName:
@ObjectName1.o
@ObjectName2.o
```

Examples from actual smake_common:
```
mcu:
@MCU_Console
@MCU_Distance
@MCU_GPIO
@MCU_HiResClock
@MCU_Serial
@MCU_SerialEcho
@MCU_SIM7000

test:
@TestAdler32
@TestAES128
@TestAES192
@TestAES256
# ... many more test objects
```

### smake_objects
Contains source-to-object mapping with dependencies. Format:
```
ObjectName.o: ../../src/Path/Source.cpp
$`pkg-config --cflags libname`   # Compilation flags
!`pkg-config --libs libname`     # Link libraries (note ! prefix)
```

Examples from actual smake_objects:
```
MemTool.o: ../../src/MemTool.cpp

HOTP.o: ../../src/Crypto/HOTP.cpp
OTP.o: ../../src/Crypto/OTP.cpp
PBKDF2.o: ../../src/Crypto/PBKDF2.cpp
TOTP.o: ../../src/Crypto/TOTP.cpp

# With library dependencies
CurlCert.o: ../../src/Crypto/Cert/CurlCert.cpp
!-lcurl

OpenSSLCert.o: ../../src/Crypto/Cert/OpenSSLCert.cpp
!-lssl
!-lcrypto
```

### Adding New Source Files
1. **Add header declaration** to `smake_common` under appropriate group
2. **Add build rule** to `smake_objects` including any dependencies
3. **Objects are compiled** only when referenced in code or explicitly requested via `smake ObjectName`

Example of adding a new crypto module:
1. In `smake_common` under `crypto:` group:
   ```
   crypto:
   @MyNewCrypto.o
   ```
2. In `smake_objects`:
   ```
   MyNewCrypto.o: ../../src/Crypto/MyNewCrypto.cpp
   !-lrequiredlib  # if needed
   ```

## Testing

The framework includes a comprehensive test suite that can be built and run via the smake build system.

### Building Tests with smake
```bash
# Change to your platform-specific build directory (e.g., build/Linux_dbg_x64)
cd build/Linux_dbg_x64
# Build all test objects (this is what "smake test" does)
smake test

# After building, programs are located at ./bin/, object files are located at ./obj/
# (relative to the current build directory)
```

### Building and Running Test Programs
After building test objects with `smake test` in your build directory, individual test executables can be run directly from the `bin/` directory and object files are in the `obj/` subdirectory.
```bash
# Example: if you built in build/Linux_dbg_x64
cd build/Linux_dbg_x64
./bin/CRC32RTest
./bin/MD5Test
# etc.
```

### Test Targets in Makefile_linuxtest
- `Test` - Builds all test programs
- `Test_CryptoTest` - Cryptography tests
- `Test_Data` - Data structure and algorithm tests
- `Test_IO` - Input/output device tests
- `Test_Manage` - System management tests
- `Test_Math` - Mathematical function tests
- `Test_Media` - Multimedia tests
- `Test_Memory` - Memory tests
- `Test_NetTest` - Network tests
- `Test_MCU` - Microcontroller unit tests

## Platform Support
 
The framework supports numerous platforms through specialized build configurations in the `build/` directory:
- Platform-specific build directories (Linux_dbg_x64, Windows_x64, etc.) for native builds
- Cross-compile targets in `build/cc_*` directories (cc_Linux_ARM, cc_Windows_x64, etc.)
 
Each platform has its own directory under `build/` with specific compiler flags and settings.

## Documentation

API documentation can be generated using Doxygen:
```bash
doxygen doxygen.conf
```
Generated documentation appears in the `doc/` directory.

## Submodules

The project uses Git submodules for third-party libraries:
```bash
git submodule update --init
```
After updating submodules, some libraries (like miniz) require additional build steps:
```bash
cd ThirdParty/miniz
mkdir _build
cd _build
cmake ..
```

## Dependencies

While designed to have minimum dependencies, certain features require external libraries:
- OpenSSL (`libssl`, `libcrypto`) for cryptography
- libcurl for HTTP functionality
- bcrypt for password hashing
- System-specific libraries for hardware access

These are specified in `smake_objects` with the `!` prefix for link flags.

## Best Practices

1. **Follow Naming Conventions** - Use `*_C.h` for C-style headers, `*.hpp` for template headers
2. **Group Related Objects** - Add new objects to appropriate groups in `smake_common`
3. **Specify Dependencies Clearly** - Use `$` for compile flags, `!` for link flags in `smake_objects`
4. **Keep Platform Code Separated** - Platform-specific code goes in `startup/` or platform-specific source files
5. **Use Existing Patterns** - Follow the existing code style and structure when adding new features
6. **Test Thoroughly** - Add tests for new functionality in the appropriate test group
7. **Use smake for all builds** - The smake build system is the only maintained way to build the framework and its components