#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/IOPinCapture.h"
#include "Text/MyString.h"

void IO::IOPinCapture::FreeCaptureData(CaptureBuff *buff)
{
	if (buff->prevBuff)
		FreeCaptureData(buff->prevBuff);
	MemFree(buff->times);
	MemFree(buff->isHigh);
	MemFree(buff);
}

UOSInt IO::IOPinCapture::AddCaptureData(Data::ArrayList<Double> *times, Data::ArrayList<Int32> *isHigh, CaptureBuff *buff)
{
	UOSInt ret = 0;
	UOSInt i;	
	if (buff->prevBuff)
	{
		ret += AddCaptureData(times, isHigh, buff->prevBuff);
	}
	i = 0;
	while (i < buff->thisDataCnt)
	{
		times->Add(buff->times[i]);
		isHigh->Add(buff->isHigh[i]);
		i++;
	}
	ret += buff->thisDataCnt;
	return ret;
};

IO::IOPinCapture::IOPinCapture(IO::IOPin *pin)
{
	this->pin = pin;
	NEW_CLASS(this->clk, Manage::HiResClock());
	this->capBuff = MemAlloc(CaptureBuff, 1);
	this->capBuff->prevBuff = 0;
	this->capBuff->thisBuffSize = 64;
	this->capBuff->thisDataCnt = 0;
	this->capBuff->times = MemAlloc(Double, 64);
	this->capBuff->isHigh = MemAlloc(Int32, 64);
	this->lastVal = this->pin->IsPinHigh();
	this->lastTime = this->clk->GetTimeDiff();
	this->startTime = 0;
}

IO::IOPinCapture::~IOPinCapture()
{
	FreeCaptureData(this->capBuff);
	DEL_CLASS(this->clk);
}

Bool IO::IOPinCapture::IsPinHigh()
{
	Bool currVal = this->pin->IsPinHigh();
	Double t = this->clk->GetTimeDiff();
	Double tVal;
	if (this->lastVal != currVal)
	{
		if (this->startTime == 0)
		{
			this->startTime = this->lastTime;
		}

		tVal = this->lastTime - this->startTime;
		if (tVal < 2.0)
		{
			if (this->capBuff->thisBuffSize == this->capBuff->thisDataCnt)
			{
				CaptureBuff *newBuff = MemAlloc(CaptureBuff, 1);
				newBuff->prevBuff = this->capBuff;
				newBuff->thisBuffSize = this->capBuff->thisBuffSize << 1;
				newBuff->thisDataCnt = 0;
				newBuff->times = MemAlloc(Double, newBuff->thisBuffSize);
				newBuff->isHigh = MemAlloc(Int32, newBuff->thisBuffSize);
				this->capBuff = newBuff;
			}
			this->capBuff->times[this->capBuff->thisDataCnt] = tVal;
			this->capBuff->isHigh[this->capBuff->thisDataCnt] = (this->lastVal?1:0);
			this->capBuff->thisDataCnt++;
		}

		tVal = t - this->startTime;
		if (tVal < 2.0)
		{
			if (this->capBuff->thisBuffSize == this->capBuff->thisDataCnt)
			{
				CaptureBuff *newBuff = MemAlloc(CaptureBuff, 1);
				newBuff->prevBuff = this->capBuff;
				newBuff->thisBuffSize = this->capBuff->thisBuffSize << 1;
				newBuff->thisDataCnt = 0;
				newBuff->times = MemAlloc(Double, newBuff->thisBuffSize);
				newBuff->isHigh = MemAlloc(Int32, newBuff->thisBuffSize);
				this->capBuff = newBuff;
			}
			this->capBuff->times[this->capBuff->thisDataCnt] = tVal;
			this->capBuff->isHigh[this->capBuff->thisDataCnt] = (currVal?1:0);
			this->capBuff->thisDataCnt++;
		}

		this->lastVal = currVal;
	}
	this->lastTime = t;
	return currVal;
}

Bool IO::IOPinCapture::IsPinOutput()
{
	return this->pin->IsPinOutput();
}

void IO::IOPinCapture::SetPinOutput(Bool isOutput)
{
	this->pin->SetPinOutput(isOutput);
	this->lastTime = this->clk->GetTimeDiff();
}

void IO::IOPinCapture::SetPinState(Bool isHigh)
{
	this->pin->SetPinState(isHigh);
	Double t = this->clk->GetTimeDiff();
	Double tVal;
	if (this->lastVal != isHigh)
	{
		if (this->startTime == 0)
		{
			this->startTime = this->lastTime;
		}

		tVal = this->lastTime - this->startTime;
		if (tVal < 2.0)
		{
			if (this->capBuff->thisBuffSize == this->capBuff->thisDataCnt)
			{
				CaptureBuff *newBuff = MemAlloc(CaptureBuff, 1);
				newBuff->prevBuff = this->capBuff;
				newBuff->thisBuffSize = this->capBuff->thisBuffSize << 1;
				newBuff->thisDataCnt = 0;
				newBuff->times = MemAlloc(Double, newBuff->thisBuffSize);
				newBuff->isHigh = MemAlloc(Int32, newBuff->thisBuffSize);
				this->capBuff = newBuff;
			}
			this->capBuff->times[this->capBuff->thisDataCnt] = tVal;
			this->capBuff->isHigh[this->capBuff->thisDataCnt] = (this->lastVal?1:0);
			this->capBuff->thisDataCnt++;
		}

		tVal = t - this->startTime;
		if (tVal < 2.0)
		{
			if (this->capBuff->thisBuffSize == this->capBuff->thisDataCnt)
			{
				CaptureBuff *newBuff = MemAlloc(CaptureBuff, 1);
				newBuff->prevBuff = this->capBuff;
				newBuff->thisBuffSize = this->capBuff->thisBuffSize << 1;
				newBuff->thisDataCnt = 0;
				newBuff->times = MemAlloc(Double, newBuff->thisBuffSize);
				newBuff->isHigh = MemAlloc(Int32, newBuff->thisBuffSize);
				this->capBuff = newBuff;
			}
			this->capBuff->times[this->capBuff->thisDataCnt] = tVal;
			this->capBuff->isHigh[this->capBuff->thisDataCnt] = (isHigh?1:0);
			this->capBuff->thisDataCnt++;
		}

		this->lastVal = isHigh;
	}
	this->lastTime = t;
}

Bool IO::IOPinCapture::SetPullType(PullType pt)
{
	return this->pin->SetPullType(pt);
}
UTF8Char *IO::IOPinCapture::GetName(UTF8Char *buff)
{
	return this->pin->GetName(Text::StrConcat(buff, (const UTF8Char*)"IOPinCapture - "));
}

UOSInt IO::IOPinCapture::GetCaptureData(Data::ArrayList<Double> *times, Data::ArrayList<Int32> *isHigh)
{
	return this->AddCaptureData(times, isHigh, this->capBuff);
}

