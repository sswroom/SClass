#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/RS232GPIO.h"
#include "Manage/HiResClock.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"

UInt32 __stdcall IO::RS232GPIO::ReadThread(AnyType userObj)
{
	NN<IO::RS232GPIO> me = userObj.GetNN<IO::RS232GPIO>();
	Manage::HiResClock *clk;
	UInt8 readBuff[12];
	UInt32 fullClk = 1000000 / me->baudRate;
	UInt32 halfClk = fullClk >> 1;
	Bool started;
	UOSInt readPos;
	UInt8 v;
	Int64 nextTime;
	Int32 t;
	started  = false;

	Sync::ThreadUtil::SetPriority(Sync::ThreadUtil::TP_REALTIME);
	NEW_CLASS(clk, Manage::HiResClock());
	me->running = true;
	while (!me->toStop)
	{
		if (!started)
		{
			Sync::SimpleThread::Sleepus(halfClk);
			if (!me->gpio->IsPinHigh(me->rxdPin))
			{
				nextTime = clk->GetTimeDiffus() + fullClk;
				readBuff[0] = 0;
				readPos = 1;
				started = true;
			}
			else
			{
				clk->Start();
			}
		}
		else
		{
			t = (Int32)(nextTime - clk->GetTimeDiffus());
			if (t > 0)
			{
				Sync::SimpleThread::Sleepus((UInt32)t);
			}
			nextTime += fullClk;

			readBuff[readPos] = me->gpio->IsPinHigh(me->rxdPin)?1:0;
			readPos++;
			if (readPos == 10)
			{
				if (readBuff[9] == 1)
				{
					v = (UInt8)(readBuff[1] | (readBuff[2] << 1) | (readBuff[3] << 2) | (readBuff[4] << 3) | (readBuff[5] << 4) | (readBuff[6] << 5) | (readBuff[7] << 6) | (readBuff[8] << 7));
					me->readBuff[me->readBuffEnd] = v;
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
					started = false;
				}
				else
				{
					started = false;
					readPos = 1;
					while (readPos < 10)
					{
						if (readBuff[readPos - 1] == 1 && readBuff[readPos] == 0)
						{
							v = 0;
							while (readPos < 10)
							{
								readBuff[v++] = readBuff[readPos++];
							}
							readPos = v;
							started = true;
							break;
						}
						readPos++;
					}
				}
			}
		}
	}
	DEL_CLASS(clk);
	me->running = false;
	return 0;
}

IO::RS232GPIO::RS232GPIO(NN<IO::GPIOControl> gpio, UOSInt rxdPin, UOSInt txdPin, UInt32 baudRate) : IO::Stream(CSTR("RS-232"))
{
	this->running = false;
	this->toStop = false;
	this->gpio = gpio;
	this->rxdPin = rxdPin;
	this->txdPin = txdPin;
	this->readBuffStart = 0;
	this->readBuffEnd = 0;
	this->baudRate = baudRate;

	Sync::ThreadUtil::Create(ReadThread, this);
	while (!this->running)
	{
		Sync::SimpleThread::Sleep(1);
	}
	this->gpio->SetPinOutput(txdPin, true);
	this->gpio->SetPinState(txdPin, true);
}

IO::RS232GPIO::~RS232GPIO()
{
	this->Close();
	MemFree(this->readBuff);
}

Bool IO::RS232GPIO::IsDown() const
{
	return !this->running;
}

UOSInt IO::RS232GPIO::Read(const Data::ByteArray &buff)
{
	Manage::HiResClock clk;
	clk.Start();
	Double t = 0;
	while (true)
	{
		if (this->readBuffStart != this->readBuffEnd)
		{
			break;
		}
		if (!this->running)
		{
			break;
		}
		t = clk.GetTimeDiff();
		if (t >= 2.0)
		{
			break;
		}
		Sync::SimpleThread::Sleep(10);
	}
	if (this->readBuffStart == this->readBuffEnd)
	{
		return 0;
	}
	UOSInt bStart = this->readBuffStart;
	UOSInt bEnd = this->readBuffEnd;
	UOSInt buffSize = bEnd - bStart;
	Data::ByteArray myBuff = buff;
	if ((OSInt)buffSize < 0)
	{
		buffSize += RS232GPIO_BUFFSIZE;
	}
	if (buffSize > myBuff.GetSize())
	{
		bEnd -= buffSize - myBuff.GetSize();
		if ((OSInt)bEnd < 0)
		{
			bEnd += RS232GPIO_BUFFSIZE;
		}
		buffSize = myBuff.GetSize();
	}

	if (bEnd < bStart)
	{
		myBuff.CopyFrom(Data::ByteArray(&this->readBuff[bStart], RS232GPIO_BUFFSIZE - bStart));
		myBuff += RS232GPIO_BUFFSIZE - bStart;
		bStart = 0;
	}
	myBuff.CopyFrom(Data::ByteArray(&this->readBuff[bStart], bEnd - bStart));
	this->readBuffStart = bEnd;
	return buffSize;
}

UOSInt IO::RS232GPIO::Write(Data::ByteArrayR buff)
{
	UInt32 fullClk = 1000000 / this->baudRate;
	UOSInt ret = buff.GetSize();
	UInt8 v;
	while (buff.GetSize() > 0)
	{
		v = *buff;
		buff += 1;
		this->gpio->SetPinState(txdPin, false);
		Sync::SimpleThread::Sleepus(fullClk);
		this->gpio->SetPinState(txdPin, v & 1);
		v = (UInt8)(v >> 1);
		Sync::SimpleThread::Sleepus(fullClk);
		this->gpio->SetPinState(txdPin, v & 1);
		v = (UInt8)(v >> 1);
		Sync::SimpleThread::Sleepus(fullClk);
		this->gpio->SetPinState(txdPin, v & 1);
		v = (UInt8)(v >> 1);
		Sync::SimpleThread::Sleepus(fullClk);
		this->gpio->SetPinState(txdPin, v & 1);
		v = (UInt8)(v >> 1);
		Sync::SimpleThread::Sleepus(fullClk);
		this->gpio->SetPinState(txdPin, v & 1);
		v = (UInt8)(v >> 1);
		Sync::SimpleThread::Sleepus(fullClk);
		this->gpio->SetPinState(txdPin, v & 1);
		v = (UInt8)(v >> 1);
		Sync::SimpleThread::Sleepus(fullClk);
		this->gpio->SetPinState(txdPin, v & 1);
		v = (UInt8)(v >> 1);
		Sync::SimpleThread::Sleepus(fullClk);
		this->gpio->SetPinState(txdPin, v & 1);
		Sync::SimpleThread::Sleepus(fullClk);
		this->gpio->SetPinState(txdPin, true);
		Sync::SimpleThread::Sleepus(fullClk);
	}
	return ret;
}

Bool IO::RS232GPIO::HasData()
{
	return this->readBuffStart != this->readBuffEnd;
}

Optional<IO::StreamReadReq> IO::RS232GPIO::BeginRead(const Data::ByteArray &buff, NN<Sync::Event> evt)
{
	UOSInt ret = Read(buff);
	if (ret)
	{
		evt->Set();
	}
	return (IO::StreamReadReq*)ret;
}

UOSInt IO::RS232GPIO::EndRead(NN<IO::StreamReadReq> reqData, Bool toWait, OutParam<Bool> incomplete)
{
	incomplete.Set(false);
	return (UOSInt)reqData.Ptr();
}

void IO::RS232GPIO::CancelRead(NN<IO::StreamReadReq> reqData)
{
}

Optional<IO::StreamWriteReq> IO::RS232GPIO::BeginWrite(Data::ByteArrayR buff, NN<Sync::Event> evt)
{
	UOSInt ret = Write(buff);
	if (ret)
	{
		evt->Set();
	}
	return (IO::StreamWriteReq*)ret;
}

UOSInt IO::RS232GPIO::EndWrite(NN<IO::StreamWriteReq> reqData, Bool toWait)
{
	return (UOSInt)reqData.Ptr();
}

void IO::RS232GPIO::CancelWrite(NN<IO::StreamWriteReq> reqData)
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
		this->toStop = true;
		while (this->running)
		{
			Sync::SimpleThread::Sleep(1);
		}
	}
}

Bool IO::RS232GPIO::Recover()
{
	///////////////////////////////////
	return false;
}

IO::StreamType IO::RS232GPIO::GetStreamType() const
{
	return IO::StreamType::RS232GPIO;
}
