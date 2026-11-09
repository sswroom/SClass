# smake build system

`smake` is SClass's build tool. It reads an `SMake.cfg` from the current
directory, follows its `include` directives, and builds the requested targets.
It is a purpose-built build system, not GNU make: configuration files describe
targets and dependencies, while the tool constructs compiler and linker
commands from those records.

The implementation is in [`src/IO/SMake.cpp`](../src/IO/SMake.cpp). The build
directory configurations are under [`build/`](../build/), and shared target
and source mappings are in [`build/inc/smake_common`](../build/inc/smake_common)
and [`build/inc/smake_objects`](../build/inc/smake_objects).

## Building and running smake

Build the `smake` executable with the platform's native makefile. For example,
the repository README documents:

```sh
cd build/Linux_x64
make smake
```

Then run `smake` from the build directory whose `SMake.cfg` selects the desired
platform. The working directory matters: `smake` opens the literal
`SMake.cfg` in the current directory, writes objects under `obj/`, and writes
programs under `bin/`.

```sh
cd build/Linux_dbg_x64
smake all
smake test
smake MyProgram
smake clean
```

Replace `smake` in these examples with the path to the executable if it is not
on `PATH`. Targets are named by their configuration entries, not by source
file paths. Multiple target names may be supplied in one invocation.

## Command-line syntax

```text
smake [options] [target ...]
```

| Option | Effect |
| --- | --- |
| `-D<object>` | Select an object or target for dependency tracing. Combine with `-V` to display the trace; use without a space, e.g. `-DCore.o`. |
| `-V` | Verbose output, including loaded configuration values and build messages. |
| `-a` | Generate assembly listings while compiling. |
| `-q` | Suppress printing the executed command lines. |
| `-s` | Use one worker thread. |
| `-S` | Disable asynchronous build mode. |
| `-i` | Show the configuration information for each named target instead of building it. |

With no target, or if no requested target is recognized, `smake` prints its
usage. `clean` is a special target-like command: it removes object files from
the current build directory's `obj/`; it does not remove programs from `bin/`.

## Configuration file layout

Each `build/<platform>/SMake.cfg` sets up toolchain variables and includes
shared configuration fragments. For example,
[`build/Linux_dbg_x64/SMake.cfg`](../build/Linux_dbg_x64/SMake.cfg) includes
the common tool settings and platform, CPU, UI, and device mappings. The
`cc_*` directories are cross-compilation configurations.

The fragments are included in deliberate groups and order. A typical
configuration follows this sequence:

1. `smake_config` for shared defaults and include paths.
2. An optional `optional*.cfg` file for local feature switches.
3. The operating-system family (`smake_osfam_unix` or
   `smake_osfam_windows`).
4. The operating system or variant (`smake_os_linux`, `smake_os_windows`,
   `smake_os_android`, and similar).
5. The CPU architecture (`smake_cpua_*`).
6. The UI backend (`smake_ui_*`).
7. The device/platform family (`smake_dev_*`).
8. A device- or SoC-specific mapping (`smake_cpu_*`), when needed.
9. `smake_common`, which supplies shared targets and header mappings.

Not every platform uses every group; for example, some targets use an
`unk` fragment or omit optional configuration. Keep the order used by the
platform's existing `SMake.cfg`: mappings and variables are processed as
included, and each fragment is intended to augment the configuration built up
by the preceding groups.

The contents of `smake_common` are arranged in four parts:

1. **Build groups** — aggregate targets such as `mcu`, `test`, `console`, and
   `all`.
2. **Program definitions** — executable and test program targets, organized
   by category/directory and program name.
3. **Main-object mappings** — each main object file mapped to its source file,
   ordered by source-file path.
4. **Header mappings** — each header file mapped to the object files associated
   with it, ordered by header-file path.

The transition points move as entries are added or removed. In the current
file, the program definitions begin around line 325, main-object mappings
around line 1314, and header mappings around line 1634; treat these as
illustrative locations only, not stable line numbers. When adding an entry,
keep it in the appropriate part and preserve that part's ordering.

`smake_common` includes `smake_objects` after its mappings. The
`smake_objects` fragment groups source-to-object rules by code area (for
example, `AI`, `Core`, `Crypto`, and `DB`), keeping related compile flags and
link libraries beside the relevant mappings. It is usually clearer to add a
rule in its corresponding group than to append it arbitrarily at the end of
the file.

An `include` line loads another configuration file. Wildcards are supported,
which is how platform configurations can include optional fragments:

```make
include ../inc/smake_config
include ./optional*.cfg
include ../inc/smake_common
```

Includes are processed as encountered, so assignments and target definitions
in later files can use earlier configuration. The build configs and included
files normally use paths relative to the build directory.

Lines beginning with `#` are comments. Configuration syntax is line-oriented;
this is not GNU make syntax, and make recipes or make functions are not
supported.

## Assignments and expansion

Assignments create or update configuration variables:

```make
CXX := g++
CFLAGS += -Wall
```

`:=` replaces the variable's value. `+=` appends the expanded value, separated
from an existing non-empty value by a space. Expand a variable with `$(NAME)`;
an unset variable expands to an empty string.

```make
CPPFLAGS += -I $(INCLUDEPATH)
```

`$(shell command)` runs a shell command and substitutes its standard output
(with trailing line endings removed). Backtick-delimited commands are also
executed when a compile or link option is expanded:

```make
$`$(PKG-CONFIG) --cflags libpq`
```

An `export NAME` line copies the current configuration value of `NAME` into
the process environment. On a `$` compile-option line, `@(command)` includes
the remaining options when the command exits successfully, while
`@!(command)` includes them when it exits unsuccessfully:

```make
$@($(PKG-CONFIG) --exists gtk4) `$(PKG-CONFIG) --cflags gtk4`
$@!($(PKG-CONFIG) --exists gtk4) `$(PKG-CONFIG) --cflags gtk+-3.0`
```

Shell substitutions execute during configuration processing or command
construction. Use them only when the build environment is trusted.

## Target and dependency syntax

The active target is selected by a declaration. A target with no source is
typically a group or executable; a target with a source mapping ending in
`.o` is typically an object:

```make
all:
@console
@ImageView

ImageView:
@ImageViewMain.o
@GUIStartup.o

ImageViewMain.o: ../../main/SSWR/ImageViewMain.cpp
```

Each following dependency or option line applies to the most recently declared
target, until another target declaration or a different configuration
directive is encountered.

| Line form | Meaning |
| --- | --- |
| `name:` | Declare a target or target group. |
| `name: source` | Declare a target with a source file, commonly an object mapping. |
| `+name:` | Continue adding entries to an already declared target. |
| `@item` | Add a target dependency, such as an object in an executable or a child target in a group. |
| `!item` | Add a linker library or other link argument to the active target. |
| `$flags` | Add compile options to the active target. |

Object names used as dependencies must have corresponding `.o` source mappings.
At link time, the objects and libraries collected from the target dependency
graph are passed to the configured C++ compiler. A target group such as
`all` builds its child targets; it is not itself linked as an executable.

`smake_common` includes header-to-object mappings as well as named program
groups. For example:

```make
IO/SomeClass.h:
@SomeClass.o
```

These mappings let smake connect quoted `#include` lines to the object files
that provide the declarations.

### Conditions on dependencies

Prefix a dependency or library item with `?(condition)` to include it only
when the condition is true:

```make
@?(NO_MINIZ > 0)Miniz_Null.o
@?(NO_MINIZ == 0)Miniz.o
!?(ANDROID_API >= 24)-landroid
```

Conditions compare integer values. The supported operators are `==`, `!=`,
`>`, `<`, `>=`, and `<=`; either operand can be an integer literal or a
configuration variable. A missing or non-numeric variable value is treated as
zero.

## Source dependencies and rebuilding

For ordinary C and C++ source mappings, smake scans lines with quoted includes
(for example, `#include "IO/SomeClass.h"`), resolves those names through the
header mappings and `INCLUDEPATH`, and uses the resulting file modification
times for incremental rebuild decisions. `INCLUDEPATH` is a colon-separated
list of directories. Angle-bracket includes are not scanned by this mechanism.

On the source side of a mapping, source path prefixes affect dependency
handling:

* `@path/to/source.cpp` bypasses the existing-object freshness check, so the
  source is compiled each time the target is built. Include dependencies are
  still scanned.
* `!path/to/source.cpp` skips scanning the source's include dependencies;
  the source file's modification time is still used for the freshness check.
* `.asm` sources are not scanned for C/C++ includes.

The prefix is removed before the source is passed to the compiler or assembler.
These source-path prefixes are distinct from line-leading `@` and `!`, which
introduce target dependencies and link arguments.

## Toolchain and output variables

Build configurations set variables that smake uses to construct commands:

| Variable | Use |
| --- | --- |
| `CXX` | Compile `.cpp` files and link programs. |
| `CC` | Compile `.c` files. |
| `ASM` | Assemble `.asm` files. |
| `CFLAGS` | Common C and C++ compilation flags. |
| `CPPFLAGS` | Additional C++ compilation flags. |
| `ASMFLAGS` | Assembler flags. |
| `LIBS` | Linker flags/libraries used for all programs. |
| `INCLUDEPATH` | Colon-separated directories searched for mapped headers. |
| `OUTPOSTFIX` | Optional suffix appended to output program names. |
| `ENABLE_TEST` | When set to `1`, building the `test` group also runs its test programs. |

Objects are stored in `obj/`; linked executables are stored in `bin/`. These
directories are relative to the active build directory. Source files and
include paths in configuration entries should likewise be written for that
working directory.

## Adding or changing build mappings

1. Add or update a source-to-object mapping in `build/inc/smake_objects`,
   e.g. `Widget.o: ../../src/Widget.cpp`.
2. If a header is used by the dependency scanner, add its mapping in
   `build/inc/smake_common` or the appropriate platform fragment, e.g.
   `Widget/Widget.h:` followed by `@Widget.o`.
3. Add the object to the relevant executable or target group with an
   `@Widget.o` line.
4. Put target-specific compile flags or link libraries after the target they
   apply to.
5. Build the target from the intended `build/<platform>/` directory.
