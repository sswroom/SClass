#include "Stdafx.h"
#if defined(WIN32) || defined(_WIN64) || defined(_WIN32)
#if defined(CPU_X86_64) || defined(_M_ARM64EC)
#include "ThreadContextX86_64W.cpp"
#include "ThreadContextX86_32W.cpp"
#elif defined(CPU_X86_32)
#include "ThreadContextX86_32W.cpp"
#elif defined(CPU_ARM)
#include "ThreadContextARMW.cpp"
#elif defined(CPU_ARM64)
#include "ThreadContextARM64W.cpp"
#else
#include "ThreadContextNull.cpp"
#endif
#else
#if defined(CPU_X86_64)
#include "ThreadContextX86_64L.cpp"
#elif defined(CPU_X86_32)
#include "ThreadContextX86_32L.cpp"
#elif defined(CPU_ARM)
#include "ThreadContextARML.cpp"
#elif defined(CPU_ARM64)
#include "ThreadContextARM64L.cpp"
#elif defined(CPU_MIPS)
#include "ThreadContextMIPSL.cpp"
#elif defined(CPU_AVL)
#include "ThreadContextAVR.cpp"
#else
#include "ThreadContextNull.cpp"
#endif
#endif