#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/IOPinCapture.h"
#include "Text/MyString.h"

void IO::IOPinCapture::FreeCaptureData(NN<CaptureBuff> buff)
{
	NN<CaptureBuff> prevBuff;
	if (buff->prevBuff.SetTo(prevBuff))
		FreeCaptureData(prevBuff);
	MemFreeArr(buff->times);
	MemFreeArr(buff->isHigh);
	MemFreeNN(buff);
}

UIntOS IO::IOPinCapture::AddCaptureData(NN<Data::ArrayListNative<Double>> times, NN<Data::ArrayListNative<Int32>> isHigh, NN<CaptureBuff> buff)
{
	UIntOS ret = 0;
	UIntOS i;
	NN<CaptureBuff> prevBuff;
	if (buff->prevBuff.SetTo(prevBuff))
	{
		ret += AddCaptureData(times, isHigh, prevBuff);
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

IO::IOPinCapture::IOPinCapture(NN<IO::IOPin> pin)
{
	this->pin = pin;
	this->capBuff = MemAllocNN(CaptureBuff);
	this->capBuff->prevBuff = nullptr;
	this->capBuff->thisBuffSize = 64;
	this->capBuff->thisDataCnt = 0;
	this->capBuff->times = MemAllocArr(Double, 64);
	this->capBuff->isHigh = MemAllocArr(Int32, 64);
	this->lastVal = this->pin->IsPinHigh();
	this->lastTime = this->clk.GetTimeDiff();
	this->startTime = 0;
}

IO::IOPinCapture::~IOPinCapture()
{
	FreeCaptureData(this->capBuff);
}

Bool IO::IOPinCapture::IsPinHigh()
{
	Bool currVal = this->pin->IsPinHigh();
	Double t = this->clk.GetTimeDiff();
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
				NN<CaptureBuff> newBuff = MemAllocNN(CaptureBuff);
				newBuff->prevBuff = this->capBuff;
				newBuff->thisBuffSize = this->capBuff->thisBuffSize << 1;
				newBuff->thisDataCnt = 0;
				newBuff->times = MemAllocArr(Double, newBuff->thisBuffSize);
				newBuff->isHigh = MemAllocArr(Int32, newBuff->thisBuffSize);
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
				NN<CaptureBuff> newBuff = MemAllocNN(CaptureBuff);
				newBuff->prevBuff = this->capBuff;
				newBuff->thisBuffSize = this->capBuff->thisBuffSize << 1;
				newBuff->thisDataCnt = 0;
				newBuff->times = MemAllocArr(Double, newBuff->thisBuffSize);
				newBuff->isHigh = MemAllocArr(Int32, newBuff->thisBuffSize);
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
	this->lastTime = this->clk.GetTimeDiff();
}

void IO::IOPinCapture::SetPinState(Bool isHigh)
{
	this->pin->SetPinState(isHigh);
	Double t = this->clk.GetTimeDiff();
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
				NN<CaptureBuff> newBuff = MemAllocNN(CaptureBuff);
				newBuff->prevBuff = this->capBuff;
				newBuff->thisBuffSize = this->capBuff->thisBuffSize << 1;
				newBuff->thisDataCnt = 0;
				newBuff->times = MemAllocArr(Double, newBuff->thisBuffSize);
				newBuff->isHigh = MemAllocArr(Int32, newBuff->thisBuffSize);
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
				NN<CaptureBuff> newBuff = MemAllocNN(CaptureBuff);
				newBuff->prevBuff = this->capBuff;
				newBuff->thisBuffSize = this->capBuff->thisBuffSize << 1;
				newBuff->thisDataCnt = 0;
				newBuff->times = MemAllocArr(Double, newBuff->thisBuffSize);
				newBuff->isHigh = MemAllocArr(Int32, newBuff->thisBuffSize);
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
UnsafeArray<UTF8Char> IO::IOPinCapture::GetName(UnsafeArray<UTF8Char> buff)
{
	return this->pin->GetName(Text::StrConcatC(buff, UTF8STRC("IOPinCapture - ")));
}

UIntOS IO::IOPinCapture::GetCaptureData(NN<Data::ArrayListNative<Double>> times, NN<Data::ArrayListNative<Int32>> isHigh)
{
	return this->AddCaptureData(times, isHigh, this->capBuff);
}

