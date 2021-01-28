#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Compress/LZWEncStream.h"
#include "IO/BitWriterLSB.h"
#include "IO/BitWriterMSB.h"

void Data::Compress::LZWEncStream::ResetTable()
{
	this->currTableSize = (Int32)(1 << this->minCodeSize) + 2;
	this->currCodeSize = this->minCodeSize + 1;
	this->nextTableSize = (Int32)(1 << (this->currCodeSize));
	this->lzwTable[this->resetCode].outByte = 0;
	this->lzwTable[this->resetCode].prevCode = -1;
	this->lzwTable[this->endCode].outByte = 0;
	this->lzwTable[this->endCode].prevCode = -1;
	Int32 i = this->resetCode;
	while (i-- > 0)
	{
		this->lzwTable[i].prevCode = -1;
		this->lzwTable[i].outByte = (UInt8)i;
		this->lzwTable[i].outLen = 1;
		this->lzwTable[i].firstByte = this->lzwTable[i].outByte;
	}
	this->maxCodeLeng = 1;
}

Data::Compress::LZWEncStream::LZWEncStream(IO::Stream *stm, Bool lsb, OSInt minCodeSize, OSInt maxCodeSize, OSInt codeSizeAdj) : IO::Stream(stm->GetSourceNameObj())
{
	this->tableSize = (Int32)(1 << maxCodeSize);
	this->lzwTable = MemAlloc(TableItem, this->tableSize);
	this->minCodeSize = minCodeSize;
	this->maxCodeSize = maxCodeSize;
	this->codeSizeAdj = codeSizeAdj;
	this->resetCode = 1 << minCodeSize;
	this->endCode = this->resetCode + 1;
	this->encBuff = MemAlloc(UInt8, this->tableSize);
	this->buffSize = 0;
	if (lsb)
	{
		NEW_CLASS(this->writer, IO::BitWriterLSB(stm));
	}
	else
	{
		NEW_CLASS(this->writer, IO::BitWriterMSB(stm));
	}
	this->toRelease = true;
	ResetTable();
	this->writer->WriteBits(this->resetCode, this->currCodeSize);
}

Data::Compress::LZWEncStream::~LZWEncStream()
{
	UInt8 *buff = this->encBuff;
	OSInt i;
	OSInt j;
	Int32 bestCode;
	UInt8 bestCodeLen;
	UInt8 codeLen;
	while (this->buffSize > 0)
	{
		if (this->currCodeSize == this->maxCodeSize && this->currTableSize >= this->nextTableSize - this->codeSizeAdj)
		{
			this->writer->WriteBits(this->resetCode, this->currCodeSize);
			ResetTable();
		}

		bestCode = buff[0];
		bestCodeLen = 1;
		if (this->buffSize > 1)
		{
			i = this->currTableSize - 1;
			while (i > this->endCode)
			{
				codeLen = this->lzwTable[i].outLen;
				if (codeLen > bestCodeLen && this->lzwTable[i].firstByte == buff[0] && codeLen <= this->buffSize)
				{
					Bool eq = true;
					j = i;
					while (codeLen-- > 0)
					{
						if (this->lzwTable[j].outByte != buff[codeLen])
						{
							eq = false;
							break;
						}
						else
						{
							j = this->lzwTable[j].prevCode;
						}
					}
					if (eq)
					{
						bestCode = (Int32)i;
						bestCodeLen = this->lzwTable[i].outLen;
						if (bestCodeLen >= this->buffSize)
						{
							break;
						}
					}
				}
				i--;
			}
		}
		this->writer->WriteBits(bestCode, this->currCodeSize);
		this->lzwTable[this->currTableSize].prevCode = bestCode;
		if (bestCodeLen < this->buffSize)
		{
			this->lzwTable[this->currTableSize].outByte = buff[bestCodeLen];
		}
		else
		{
			this->lzwTable[this->currTableSize].outByte = 0;
		}
		this->lzwTable[this->currTableSize].outLen = bestCodeLen + 1;
		this->lzwTable[this->currTableSize].firstByte = buff[0];
		this->currTableSize++;
		buff += bestCodeLen;
		this->buffSize -= bestCodeLen;
		if (this->currTableSize >= this->nextTableSize + 1 - this->codeSizeAdj)
		{
			if (this->currCodeSize < this->maxCodeSize)
			{
				this->currCodeSize++;
				this->nextTableSize = this->nextTableSize << 1;
			}
		}
	}
	this->writer->WriteBits(this->endCode, this->currCodeSize);
	MemFree(this->lzwTable);
	if (this->toRelease)
	{
		DEL_CLASS(this->writer);
	}
	MemFree(this->encBuff);
}

OSInt Data::Compress::LZWEncStream::Read(UInt8 *buff, OSInt size)
{
	return 0;
}

OSInt Data::Compress::LZWEncStream::Write(const UInt8 *buff, OSInt size)
{
	UInt8 *relBuff = 0;
	OSInt sizeLeft;
	Int32 bestCode;
	Int32 bestCodeLen;
	Int32 codeLen;
	OSInt i;
	OSInt j;
	if (this->buffSize == 0)
	{
		sizeLeft = size;
	}
	else
	{
		relBuff = MemAlloc(UInt8, size + this->buffSize);
		MemCopyNO(relBuff, this->encBuff, this->buffSize);
		MemCopyNO(&relBuff[this->buffSize], buff, size);
		sizeLeft = size + this->buffSize;
		buff = relBuff;
	}

	while (sizeLeft >= (this->maxCodeLeng + 1))
	{
		if (this->currCodeSize == this->maxCodeSize && this->currTableSize >= this->nextTableSize + 1 - this->codeSizeAdj)
		{
			this->writer->WriteBits(this->resetCode, this->currCodeSize);
			ResetTable();
		}

		bestCode = buff[0];
		bestCodeLen = 1;
		i = this->currTableSize - 1;
		while (i > this->endCode)
		{
			codeLen = this->lzwTable[i].outLen;
			if (codeLen > bestCodeLen && this->lzwTable[i].firstByte == buff[0])
			{
				Bool eq = true;
				j = i;
				while (codeLen-- > 0)
				{
					if (this->lzwTable[j].outByte != buff[codeLen])
					{
						eq = false;
						break;
					}
					else
					{
						j = this->lzwTable[j].prevCode;
					}
				}
				if (eq)
				{
					bestCode = (Int32)i;
					bestCodeLen = this->lzwTable[i].outLen;
				}
			}
			i--;
		}
		this->writer->WriteBits((UInt32)bestCode, this->currCodeSize);
		this->lzwTable[this->currTableSize].prevCode = bestCode;
		this->lzwTable[this->currTableSize].outByte = buff[bestCodeLen];
		this->lzwTable[this->currTableSize].outLen = bestCodeLen + 1;
		this->currTableSize++;
		buff += bestCodeLen;
		sizeLeft -= bestCodeLen;
		if (this->currTableSize >= this->nextTableSize + 1 - this->codeSizeAdj)
		{
			if (this->currCodeSize < this->maxCodeSize)
			{
				this->currCodeSize++;
				this->nextTableSize = this->nextTableSize << 1;
			}
		}
	}
	MemCopyNO(this->encBuff, buff, sizeLeft);
	this->buffSize = sizeLeft;
	if (relBuff)
	{
		MemFree(relBuff);
	}
	
	return size;
}

Int32 Data::Compress::LZWEncStream::Flush()
{
	return 0;
}

void Data::Compress::LZWEncStream::Close()
{
}

Bool Data::Compress::LZWEncStream::Recover()
{
	return true;
}
