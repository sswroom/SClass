#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/RS232GPIO.h"
#include "Manage/HiResClock.h"
#include "Sync/ThreadUtil.h"

extern "C"
{
	void SerialPort_Init(Int32 baudRate);
	void SerialPort_Write(UInt8 b);
}

void __stdcall IO::RS232GPIO::IntHdlr(void *userObj)
{
	IO::RS232GPIO *me = (IO::RS232GPIO*)userObj;
	UInt32 fullClk = 1000000 / me->baudRate;
	Bool isHigh = me->gpio->IsPinHigh(me->rxdPin);
	SerialPort_Write(isHigh?'1':'0');
	if (me->readStartTime == 0)
	{
		if (!isHigh)
		{
			me->readStartTime = Manage::HiResClock::GetRelTime_us();
			me->readBit = 0;
			me->readVal = 0;
		}
	}
	else
	{
		Int64 t = Manage::HiResClock::GetRelTime_us() - me->readStartTime;
		OSInt currBit = t + (fullClk >> 1) / fullClk;
		if (currBit <= me->readBit)
		{

		}
		else
		{
			while (me->readBit < currBit)
			{
				if (!isHigh)
				{
					switch (me->readBit)
					{
					case 2:
						me->readVal |= 1;
						break;
					case 3:
						me->readVal |= 2;
						break;
					case 4:
						me->readVal |= 4;
						break;
					case 5:
						me->readVal |= 8;
						break;
					case 6:
						me->readVal |= 16;
						break;
					case 7:
						me->readVal |= 32;
						break;
					case 8:
						me->readVal |= 64;
						break;
					case 9:
						me->readVal |= 128;
						break;
					}
				}
				me->readBit++;
			}
			if (me->readBit >= 10)
			{
				OSInt readPos;
				me->readBuff[me->readBuffEnd] = me->readVal;
				readPos = me->readBuffEnd + 1;
				if (readPos == RS232GPIO_BUFFSIZE)
				{
					readPos = 0;
				}
				if (readPos == me->readBuffStart)
				{
					if (me->readBuffStart + 1 == RS232GPIO_BUFFSIZE)
					{
						me->readBuffStart = 0;
					}
					else
					{
						me->readBuffStart++;
					}
				}
				me->readBuffEnd = readPos;

				if (isHigh)
				{
					me->readStartTime = 0;
				}
				else
				{
					me->readStartTime = Manage::HiResClock::GetRelTime_us();
					me->readBit = 0;
					me->readVal = 0;
				}
			}
		}
	}
}

IO::RS232GPIO::RS232GPIO(IO::GPIOControl *gpio, UOSInt rxdPin, UOSInt txdPin, UInt32 baudRate) : IO::Stream(UTF8STRC("RS-232"))
{
	this->running = false;
	this->toStop = false;
	this->gpio = gpio;
	this->rxdPin = rxdPin;
	this->txdPin = txdPin;
	this->readBuffStart = 0;
	this->readBuffEnd = 0;
	this->baudRate = baudRate;
	this->readStartTime = 0;
	this->readBit = 0;
	Manage::HiResClock::Init();

	this->gpio->SetPinOutput(txdPin, true);
	this->gpio->SetPinState(txdPin, true);
	this->gpio->HandleInterrupt(IntHdlr, this);
	this->gpio->InterruptEnable(rxdPin, true);
	this->running = true;
}

IO::RS232GPIO::~RS232GPIO()
{
	this->Close();
	this->gpio->UnhandleInterrupt(IntHdlr, this);
}

UOSInt IO::RS232GPIO::Read(const Data::ByteArray &buff)
{
	Manage::HiResClock clk;
	clk.Start();
	Int64 t = 0;
	while (true)
	{
		if (this->readBuffStart != this->readBuffEnd)
		{
			break;
		}
		if (!this->running)
			break;
		t = clk.GetTimeDiffus();
		if (t >= 2000000)
		{
			break;
		}
		Sync::SimpleThread::Sleep(10);
	}
	if (this->readBuffStart == this->readBuffEnd)
	{
		return 0;
	}
	OSInt bStart = this->readBuffStart;
	OSInt bEnd = this->readBuffEnd;
	OSInt buffSize = bEnd - bStart;
	if (buffSize < 0)
	{
		buffSize += RS232GPIO_BUFFSIZE;
	}
	if (buffSize > size)
	{
		bEnd -= buffSize - size;
		if (bEnd < 0)
		{
			bEnd += RS232GPIO_BUFFSIZE;
		}
		buffSize = size;
	}

	if (bEnd < bStart)
	{
		MemCopyNO(buff, &this->readBuff[bStart], RS232GPIO_BUFFSIZE - bStart);
		buff += RS232GPIO_BUFFSIZE - bStart;
		bStart = 0;
	}
	MemCopyNO(buff, &this->readBuff[bStart], bEnd - bStart);
	this->readBuffStart = bEnd;
	return buffSize;
}

UOSInt IO::RS232GPIO::Write(const UInt8 *buff, UOSInt size)
{
	UInt32 t = 1000000 / this->baudRate;
	UOSInt ret = size;
	UInt8 v;
	while (size-- > 0)
	{
		v = *buff++;
		this->gpio->SetPinState(txdPin, false);
		Sync::SimpleThread::Sleepus(t);
		this->gpio->SetPinState(txdPin, v & 1);
		v >>= 1;
		Sync::SimpleThread::Sleepus(t);
		this->gpio->SetPinState(txdPin, v & 1);
		v >>= 1;
		Sync::SimpleThread::Sleepus(t);
		this->gpio->SetPinState(txdPin, v & 1);
		v >>= 1;
		Sync::SimpleThread::Sleepus(t);
		this->gpio->SetPinState(txdPin, v & 1);
		v >>= 1;
		Sync::SimpleThread::Sleepus(t);
		this->gpio->SetPinState(txdPin, v & 1);
		v >>= 1;
		Sync::SimpleThread::Sleepus(t);
		this->gpio->SetPinState(txdPin, v & 1);
		v >>= 1;
		Sync::SimpleThread::Sleepus(t);
		this->gpio->SetPinState(txdPin, v & 1);
		v >>= 1;
		Sync::SimpleThread::Sleepus(t);
		this->gpio->SetPinState(txdPin, v & 1);
		Sync::SimpleThread::Sleepus(t);
		this->gpio->SetPinState(txdPin, true);
		Sync::SimpleThread::Sleepus(t);
	}
	return ret;
}

Bool IO::RS232GPIO::HasData()
{
	return this->readBuffStart != this->readBuffEnd;
}

void *IO::RS232GPIO::BeginRead(const Data::ByteArray &buff, Sync::Event *evt)
{
	void *ret = (void*)Read(buff, size);
	if (ret)
	{
		evt->Set();
	}
	return ret;
}

UOSInt IO::RS232GPIO::EndRead(void *reqData, Bool toWait, Bool *incomplete)
{
	if (incomplete)
	{
		*incomplete = false;
	}
	return (UOSInt)reqData;
}

void IO::RS232GPIO::CancelRead(void *reqData)
{
}

void *IO::RS232GPIO::BeginWrite(const UInt8 *buff, UOSInt size, Sync::Event *evt)
{
	void *ret = (void*)Write(buff, size);
	if (ret)
	{
		evt->Set();
	}
	return ret;
}

UOSInt IO::RS232GPIO::EndWrite(void *reqData, Bool toWait)
{
	return (UOSInt)reqData;
}

void IO::RS232GPIO::CancelWrite(void *reqData)
{
}

Int32 IO::RS232GPIO::Flush()
{
	return 0;
}

void IO::RS232GPIO::Close()
{
	if (this->running)
	{
		this->gpio->InterruptEnable(this->rxdPin, false);
		this->running = false;
	}
}

Bool IO::RS232GPIO::Recover()
{
	///////////////////////////////////
	return false;
}
