#ifndef _SM_WINDEBUG
#define _SM_WINDEBUG
#if defined(_DEBUG) && defined(_MSC_VER)
#include <windows.h>
#define printf WinDebug_DebugOut
static void WINAPIV WinDebug_DebugOut(const Char* fmt, ...) {
	Char s[1025];
	va_list args;
	va_start(args, fmt);
	vsprintf(s, fmt, args);
	va_end(args);
	OutputDebugStringA(s);
}
#endif
#endif
