#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Compress/LZWEncStream2.h"
#include "IO/BitWriterLSB.h"
#include "IO/BitWriterMSB.h"

void Data::Compress::LZWEncStream2::ResetTable()
{
	this->currTableSize = (UInt16)((1 << this->minCodeSize) + 2);
	this->currCodeSize = this->minCodeSize + 1;
	this->nextTableSize = (UInt16)(1 << (this->currCodeSize));
	UInt16 i = 0;
	UInt16 j = this->resetCode;
	while (i < j)
	{
#if IS_BYTEORDER_LE
		*(UInt32*)&this->lzwTable[i * 2] = i;
#else
		this->lzwTable[i * 2] = i;
		this->lzwTable[i * 2 + 1] = 0;
#endif
		i++;
	}
	this->nextTableOfst = 1;
}

Data::Compress::LZWEncStream2::LZWEncStream2(IO::Stream *stm, Bool lsb, UOSInt minCodeSize, UOSInt maxCodeSize, OSInt codeSizeAdj) : IO::Stream(stm->GetSourceNameObj())
{
	this->tableSize = ((UOSInt)1 << maxCodeSize);
	this->lzwTable = MemAllocA(UInt16, this->tableSize * 512);
	this->minCodeSize = minCodeSize;
	this->maxCodeSize = maxCodeSize;
	this->codeSizeAdj = codeSizeAdj;
	this->resetCode = (UInt16)(1 << minCodeSize);
	this->endCode = (UInt16)(this->resetCode + 1);
	this->encBuff = MemAlloc(UInt8, this->tableSize);
	this->buffSize = 0;
	this->stm  = stm;
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

Data::Compress::LZWEncStream2::~LZWEncStream2()
{
	UInt8 *buff = this->encBuff;
	UOSInt i;
	UOSInt j;
	UInt32 bestCode;
	Bool found;
	while (this->buffSize > 0)
	{
		if (this->currCodeSize == this->maxCodeSize && this->currTableSize >= this->nextTableSize - this->codeSizeAdj)
		{
			this->writer->WriteBits(this->resetCode, this->currCodeSize);
			ResetTable();
		}

		found = false;
		bestCode = buff[0];
		i = 0;
		j = 0;
		while (i < this->buffSize)
		{
			if (this->lzwTable[j + (UOSInt)buff[i] * 2] == 0 && j != 0)
			{
				this->writer->WriteBits(bestCode, this->currCodeSize);
				this->lzwTable[j + (UOSInt)buff[i] * 2] = this->currTableSize;
				this->lzwTable[j + (UOSInt)buff[i] * 2 + 1] = 0;
				this->currTableSize++;
				found = true;
				buff += i;
				this->buffSize -= i;
				break;
			}
			else
			{
				bestCode = this->lzwTable[j + (UOSInt)buff[i] * 2];
				if (this->lzwTable[j + (UOSInt)buff[i] * 2 + 1] == 0)
				{
					this->lzwTable[j + (UOSInt)buff[i] * 2 + 1] = (UInt16)(this->currTableSize - this->endCode);
					j = (UOSInt)(this->currTableSize - this->endCode) * 512;
					MemClearAC((UInt8*)&this->lzwTable[j], 1024);
				}
				else
				{
					j = (UOSInt)this->lzwTable[j + (UOSInt)buff[i] * 2 + 1] * 512;
				}
			}
			i++;
		}
		if (!found)
		{
			this->writer->WriteBits(bestCode, this->currCodeSize);
			this->currTableSize++;
			break;
		}
		if (this->currTableSize >= this->nextTableSize + 1 - this->codeSizeAdj)
		{
			if (this->currCodeSize < this->maxCodeSize)
			{
				this->currCodeSize++;
				this->nextTableSize = (UInt16)(this->nextTableSize << 1);
			}
		}
	}
	this->writer->WriteBits(this->endCode, this->currCodeSize);
	MemFreeA(this->lzwTable);
	if (this->toRelease)
	{
		DEL_CLASS(this->writer);
	}
	MemFree(this->encBuff);
}

Bool Data::Compress::LZWEncStream2::IsDown()
{
	return this->stm->IsDown();
}

UOSInt Data::Compress::LZWEncStream2::Read(UInt8 *buff, UOSInt size)
{
	return 0;
}

UOSInt Data::Compress::LZWEncStream2::Write(const UInt8 *buff, UOSInt size)
{
	UInt8 *relBuff = 0;
	UOSInt sizeLeft;
	UInt16 bestCode;
	UOSInt i;
	UOSInt j;
	Bool found;
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

	while (true) //sizeLeft >= 0
	{
		if (this->currCodeSize == this->maxCodeSize && this->currTableSize >= this->nextTableSize + 1 - this->codeSizeAdj)
		{
			this->writer->WriteBits(this->resetCode, this->currCodeSize);
			ResetTable();
		}

		found = false;
		bestCode = buff[0];
		i = 0;
		j = 0;
		while (i < sizeLeft)
		{
			if (this->lzwTable[j + (UOSInt)buff[i] * 2] == 0 && j != 0)
			{
				this->writer->WriteBits(bestCode, this->currCodeSize);
#if IS_BYTEORDER_LE
				*(UInt32*)&this->lzwTable[j + (UOSInt)buff[i] * 2] = this->currTableSize;
#else
				this->lzwTable[j + buff[i] * 2] = this->currTableSize;
				this->lzwTable[j + buff[i] * 2 + 1] = 0;
#endif
				this->currTableSize++;
				found = true;
				buff += i;
				sizeLeft -= i;
				break;
			}
			else
			{
				bestCode = this->lzwTable[j + (UOSInt)buff[i] * 2];
				if (this->lzwTable[j + (UOSInt)buff[i] * 2 + 1] == 0)
				{
					this->lzwTable[j + (UOSInt)buff[i] * 2 + 1] = this->nextTableOfst;
					j = (UOSInt)this->nextTableOfst * 512;
					MemClearAC((UInt8*)&this->lzwTable[j], 1024);
					this->nextTableOfst++;
				}
				else
				{
					j = (UOSInt)this->lzwTable[j + (UOSInt)buff[i] * 2 + 1] * 512;
				}
			}
			i++;
		}
		if (!found)
		{
			break;
		}
		if (this->currTableSize >= this->nextTableSize + 1 - this->codeSizeAdj)
		{
			if (this->currCodeSize < this->maxCodeSize)
			{
				this->currCodeSize++;
				this->nextTableSize = (UInt16)(this->nextTableSize << 1);
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

Int32 Data::Compress::LZWEncStream2::Flush()
{
	return 0;
}

void Data::Compress::LZWEncStream2::Close()
{
}

Bool Data::Compress::LZWEncStream2::Recover()
{
	return true;
}
