#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/BitWriterMSB.h"
#include "Data/ByteTool.h"

#define BUFFSIZE 1024

IO::BitWriterMSB::BitWriterMSB(IO::Stream *stm)
{
	this->buff = MemAlloc(UInt8, BUFFSIZE);
	this->currBytePos = 0;
	this->currBitPos = 0;
	this->stm = stm;
}

IO::BitWriterMSB::~BitWriterMSB()
{
	this->ByteAlign();
	if (this->currBytePos > 0)
	{
		this->stm->Write(this->buff, this->currBytePos);
	}
	MemFree(this->buff);
}

Bool IO::BitWriterMSB::WriteBits(UInt32 code, OSInt bitCount)
{
	if (this->currBitPos != 0)
	{
		OSInt rShift = (bitCount - 8 + this->currBitPos);
		if (rShift > 0)
		{
			this->buff[this->currBytePos] = (UInt8)((this->buff[this->currBytePos] | (code >> rShift)) & 0xff);
		}
		else if (rShift < 0)
		{
			this->buff[this->currBytePos] = (UInt8)((this->buff[this->currBytePos] | (code << -rShift)) & 0xff);
		}
		else
		{
			this->buff[this->currBytePos] = (UInt8)((this->buff[this->currBytePos] | code) & 0xff);
		}
		if ((this->currBitPos + bitCount) < 8)
		{
			this->currBitPos += bitCount;
			return true;
		}
		bitCount -= 8 - this->currBitPos;
		code = code & ((1 << bitCount) - 1);
		this->currBitPos = 0;
		this->currBytePos++;
	}
	while (bitCount >= 8)
	{
		this->buff[this->currBytePos] = (UInt8)(code >> (bitCount - 8));
		this->currBytePos++;
		bitCount -= 8;
		code = code & ((1 << bitCount) - 1);
	}
	if (this->currBytePos >= BUFFSIZE - 5)
	{
		this->stm->Write(this->buff, this->currBytePos);
		this->currBytePos = 0;
	}
	if (bitCount > 0)
	{
		this->buff[this->currBytePos] = (UInt8)(code << (8 - bitCount));
		this->currBitPos = bitCount;
	}
	return true;
}

Bool IO::BitWriterMSB::ByteAlign()
{
	if (this->currBitPos)
	{
		this->currBytePos++;
		this->currBitPos = 0;
	}
	return true;
}
