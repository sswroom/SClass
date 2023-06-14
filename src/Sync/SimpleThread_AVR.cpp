#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/StreamReader.h"
#include "Sync/SimpleThread.h"
#include "Text/MyString.h"
#include <avr/interrupt.h>
#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )

void Sync::SimpleThread::Sleep(UOSInt ms)
{
	while (ms-- > 0)
	{
		Sleepus(1000);
	}
}

void Sync::SimpleThread::Sleepus(UOSInt us)
{
#if F_CPU >= 24000000L
	if (!us) return; //  = 3 cycles, (4 when true)
	us *= 6; // x6 us, = 7 cycles
	us -= 5; //=2 cycles
#elif F_CPU >= 20000000L
	__asm__ __volatile__ (
		"nop" "\n\t"
		"nop" "\n\t"
		"nop" "\n\t"
		"nop"); //just waiting 4 cycles
	if (us <= 1) return; //  = 3 cycles, (4 when true)
	us = (us << 2) + us; // x5 us, = 7 cycles
	us -= 7; // 2 cycles
#elif F_CPU >= 16000000L
	if (us <= 1) return; //  = 3 cycles, (4 when true)
	us <<= 2; // x4 us, = 4 cycles
	us -= 5; // = 2 cycles,
#elif F_CPU >= 12000000L
	if (us <= 1) return; //  = 3 cycles, (4 when true)
	us = (us << 1) + us; // x3 us, = 5 cycles
	us -= 5; //2 cycles
#elif F_CPU >= 8000000L
	if (us <= 2) return; //  = 3 cycles, (4 when true)
	us <<= 1; //x2 us, = 2 cycles
	us -= 4; // = 2 cycles

#else
	if (us <= 16) return; //= 3 cycles, (4 when true)
	if (us <= 25) return; //= 3 cycles, (4 when true), (must be at least 25 if we want to substract 22)
	us -= 22; // = 2 cycles
	us >>= 2; // us div 4, = 4 cycles
#endif

	__asm__ __volatile__ (
		"1: sbiw %0,1" "\n\t" // 2 cycles
		"brne 1b" : "=w" (us) : "0" (us) // 2 cycles
	);
}

/*UInt32 Sync::Thread::GetThreadId()
{
	return 0;
}

Int32 Sync::Thread::GetThreadCnt()
{
	return 1;
}

void Sync::Thread::SetPriority(ThreadPriority priority)
{
}
*/