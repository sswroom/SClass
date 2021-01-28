#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/BitWriterLSB.h"
#include "Data/ByteTool.h"

#define BUFFSIZE 1024

IO::BitWriterLSB::BitWriterLSB(IO::Stream *stm)
{
	this->buff = MemAlloc(UInt8, BUFFSIZE);
	this->currBytePos = 0;
	this->currBitPos = 0;
	this->stm = stm;
}

IO::BitWriterLSB::~BitWriterLSB()
{
	this->ByteAlign();
	if (this->currBytePos > 0)
	{
		this->stm->Write(this->buff, this->currBytePos);
	}
	MemFree(this->buff);
}

Bool IO::BitWriterLSB::WriteBits(UInt32 code, OSInt bitCount)
{
	if (this->currBitPos != 0)
	{
		this->buff[this->currBytePos] = (UInt8)(this->buff[this->currBytePos] | (code << this->currBitPos)) & 0xff;
		if ((this->currBitPos + bitCount) < 8)
		{
			this->currBitPos += bitCount;
			return true;
		}
		code = code >> (8 - this->currBitPos);
		bitCount -= 8 - this->currBitPos;
		this->currBitPos = 0;
		this->currBytePos++;
	}
	while (bitCount >= 8)
	{
		this->buff[this->currBytePos] = (UInt8)(code & 0xff);
		this->currBytePos++;
		bitCount -= 8;
		code = code >> 8;
	}
	if (this->currBytePos >= BUFFSIZE - 5)
	{
		this->stm->Write(this->buff, this->currBytePos);
		this->currBytePos = 0;
	}
	if (bitCount > 0)
	{
		this->buff[this->currBytePos] = (UInt8)(code & 0xff);
		this->currBitPos = bitCount;
	}
	return true;
}

Bool IO::BitWriterLSB::ByteAlign()
{
	if (this->currBitPos)
	{
		this->currBytePos++;
		this->currBitPos = 0;
	}
	return true;
}
