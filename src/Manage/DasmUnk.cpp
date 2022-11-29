#include "Stdafx.h"
#if defined(CPU_ARM64) && defined(_WIN64)
#include "DasmX86_64.cpp"
#include "DasmX86_32.cpp"
#include "DasmARM64.cpp"
#elif defined(CPU_X86_64)
#include "DasmX86_64.cpp"
#include "DasmX86_32.cpp"
#elif defined(CPU_X86_32)
#include "DasmX86_32.cpp"
#elif defined(CPU_ARM64)
#include "DasmARM64.cpp"
#elif defined(CPU_ARM)
#include "DasmARM.cpp"
#elif defined(CPU_MIPS)
#include "DasmMIPS.cpp"
#else
#include "DasmNull.cpp"
#endif