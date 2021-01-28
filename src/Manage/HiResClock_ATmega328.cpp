#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "Text/MyString.h"
#include <avr/io.h>
#include <avr/interrupt.h>

extern "C"
{
	void SerialPort_Init(Int32 baudRate);
	void SerialPort_Write(UInt8 b);
}

void DebugWrite(const Char *msg)
{
	Char c;
	while ((c = *msg++) != 0)
	{
		SerialPort_Write((UInt8)c);
	}
}

static Bool HiResClock_Inited = false;
volatile UInt32 HiResClock_timer0_overflow_count = 0;

#if defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
ISR(TIM0_OVF_vect)
#else
ISR(TIMER0_OVF_vect)
#endif
{
	HiResClock_timer0_overflow_count++;
}

void HiResClock_GetTime(UInt32 *v) {
	UInt32 m;
	UInt8 oldSREG = SREG;
	UInt8 t;
	
	SREG &= ~0x80;
	m = HiResClock_timer0_overflow_count;
#if defined(TCNT0)
	t = TCNT0;
#elif defined(TCNT0L)
	t = TCNT0L;
#else
	#error TIMER 0 not defined
#endif

#ifdef TIFR0
	if ((TIFR0 & (1 << TOV0)) && (t < 255))
		m++;
#else
	if ((TIFR & (1 << TOV0)) && (t < 255))
		m++;
#endif

	SREG = oldSREG;
	
	v[1] = m;
	v[0] = t;
}

Manage::HiResClock::HiResClock()
{
	Init();
	this->Start();
}

Manage::HiResClock::~HiResClock()
{
}

void Manage::HiResClock::Start()
{
	HiResClock_GetTime((UInt32*)&this->stTime);
}

Double Manage::HiResClock::GetTimeDiff()
{
	UInt32 v[2];
	UInt32 *stTime = (UInt32*)&this->stTime;
	HiResClock_GetTime(v);
	v[1] -= stTime[1];
	if (v[0] >= stTime[0])
	{
		v[0] -= stTime[0];
	}
	else
	{
		v[0] += 256 - stTime[0];
		v[1]--;
	}
	return (v[1] * 256.0 * 64.0 / (Double)F_CPU) + (v[0] * 64.0 / (Double)F_CPU);
}

Int64 Manage::HiResClock::GetTimeDiffus()
{
	UInt32 v[2];
	UInt32 *stTime = (UInt32*)&this->stTime;
	HiResClock_GetTime(v);
	v[1] -= stTime[1];
	if (v[0] >= stTime[0])
	{
		v[0] -= stTime[0];
	}
	else
	{
		v[0] += 256 - stTime[0];
		v[1]--;
	}

	return ((v[1] << 8) + v[0]) * 64 / (F_CPU / 1000000);
}

Double Manage::HiResClock::GetAndRestart()
{
	UInt32 v[2];
	UInt32 *stTime = (UInt32*)&this->stTime;
	v[0] = stTime[0];
	v[1] = stTime[1];
	HiResClock_GetTime(stTime);
	v[1] = stTime[1] - v[1];
	if (v[0] <= stTime[0])
	{
		v[0] = stTime[0] - v[0];
	}
	else
	{
		v[0] = stTime[0] + 256 - v[0];
		v[1]--;
	}
	return (v[1] * 256.0 * 64.0 / F_CPU) + (v[0] * 64.0 / (Double)F_CPU);
}

void Manage::HiResClock::Init()
{
	if (!HiResClock_Inited)
	{
#if defined(TCCR0A) && defined(WGM01)
		TCCR0A |= (1 << WGM01) | (1 << WGM00);
#endif

#if defined(__AVR_ATmega128__)
		TCCR0 |= (1 << CS02);
#elif defined(TCCR0) && defined(CS01) && defined(CS00)
		TCCR0 |= (1 << CS01) | (1 << CS00);
#elif defined(TCCR0B) && defined(CS01) && defined(CS00)
		TCCR0B = (TCCR0B & ~(1 << CS02)) | (1 << CS01) | (1 << CS00);
#elif defined(TCCR0A) && defined(CS01) && defined(CS00)
		TCCR0A |= (1 << CS01) | (1 << CS00);
#else
		#error Timer 0 prescale factor 64 not set correctly
#endif

#if defined(TIMSK) && defined(TOIE0)
		TIMSK = (1 << TOIE0);
#elif defined(TIMSK0) && defined(TOIE0)
		TIMSK0 = (1 << TOIE0);
#else
		#error	Timer 0 overflow interrupt not set correctly
#endif

		SREG |= 0x80;
		HiResClock_Inited = true;
	}
}

Int64 Manage::HiResClock::GetRelTime_us()
{
	UInt32 v[2];
	HiResClock_GetTime(v);
	return (((Int64)v[1] << 8) + v[0]) * 64 / (F_CPU / 1000000);

}
