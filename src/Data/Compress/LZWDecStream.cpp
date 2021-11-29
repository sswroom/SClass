#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Compress/LZWDecStream.h"
#include "IO/BitReaderLSB.h"
#include "IO/BitReaderMSB.h"

#define DECBUFFSIZE 512

void Data::Compress::LZWDecStream::ResetTable()
{
	this->currTableSize = ((UOSInt)1 << this->minCodeSize) + 2;
	this->currCodeSize = this->minCodeSize + 1;
	this->nextTableSize = ((UOSInt)1 << (this->currCodeSize));
	*(Int32*)&this->lzwTable[this->resetCode * 4] = 0;
	*(Int32*)&this->lzwTable[this->endCode * 4] = 0;

	UInt32 i = 0;
	UInt32 j = this->resetCode;
	while (i < j)
	{
#if IS_BYTEORDER_LE
		*(UInt32*)&this->lzwTable[i * 4] = i << 16;
#else
		*(UInt16*)&this->lzwTable[i * 4] = 0;
		*(UInt16*)&this->lzwTable[i * 4 + 2] = (UInt16)i;
#endif
		i++;
	}
	this->localCode = (UInt32)-1;
}

Data::Compress::LZWDecStream::LZWDecStream(IO::Stream *stm, Bool lsb, UOSInt minCodeSize, UOSInt maxCodeSize, UOSInt codeSizeAdj) : IO::Stream(stm->GetSourceNameObj())
{
	this->tableSize = ((UOSInt)1 << maxCodeSize);
	this->lzwTable = MemAlloc(UInt8, this->tableSize * 4);
	this->minCodeSize = minCodeSize;
	this->maxCodeSize = maxCodeSize;
	this->decBuff = MemAlloc(UInt8, DECBUFFSIZE);
	this->decBuffSize = 0;
	this->codeSizeAdj = codeSizeAdj;
	this->resetCode = (UInt32)(1 << minCodeSize);
	this->endCode = this->resetCode + 1;
	if (lsb)
	{
		NEW_CLASS(this->reader, IO::BitReaderLSB(stm));
	}
	else
	{
		NEW_CLASS(this->reader, IO::BitReaderMSB(stm));
	}
	this->toRelease = true;
	ResetTable();
}

Data::Compress::LZWDecStream::LZWDecStream(IO::BitReader *reader, Bool toRelease, UOSInt minCodeSize, UOSInt maxCodeSize, UOSInt codeSizeAdj) : IO::Stream((const UTF8Char*)"LZWStream")
{
	this->tableSize = ((UOSInt)1 << maxCodeSize);
	this->lzwTable = MemAlloc(UInt8, this->tableSize * 4);
	this->minCodeSize = minCodeSize;
	this->maxCodeSize = maxCodeSize;
	this->decBuff = MemAlloc(UInt8, DECBUFFSIZE);
	this->decBuffSize = 0;
	this->codeSizeAdj = codeSizeAdj;
	this->resetCode = (UInt32)(1 << minCodeSize);
	this->endCode = this->resetCode + 1;
	this->reader = reader;
	this->toRelease = toRelease;
	ResetTable();
}

Data::Compress::LZWDecStream::~LZWDecStream()
{
	MemFree(this->decBuff);
	MemFree(this->lzwTable);
	if (this->toRelease)
	{
		DEL_CLASS(this->reader);
	}
}

UOSInt Data::Compress::LZWDecStream::Read(UInt8 *buff, UOSInt size)
{
	UOSInt writeSize = 0;
	UInt32 code;

	if (this->decBuffSize > 0)
	{
		if (this->decBuffSize >= size)
		{
			MemCopyNO(buff, this->decBuff, size);
			buff += size;
			this->decBuffSize -= size;
			if (this->decBuffSize > 0)
			{
				MemCopyO(this->decBuff, &this->decBuff[size], this->decBuffSize);
			}
			return size;
		}
		else
		{
			MemCopyNO(buff, this->decBuff, this->decBuffSize);
			buff += this->decBuffSize;
			writeSize += this->decBuffSize;
			size -= this->decBuffSize;
			this->decBuffSize = 0;
		}
	}
	if (size <= 0)
	{
		return writeSize;
	}
	while (reader->ReadBits(&code, this->currCodeSize))
	{
		if (code == this->endCode)
		{
			break;
		}
		else if (code == this->resetCode)
		{
			this->ResetTable();
		}
		else
		{
			if (code > this->currTableSize)
			{
				break;
				////////////////////////
			}
			else
			{
				if (this->localCode == (UInt32)-1)
				{
					*buff++ = (UInt8)code;
					this->localCode = code;
					writeSize++;
					if (--size <= 0)
					{
						return writeSize;
					}
				}
				else if (this->currTableSize >= this->nextTableSize)
				{
					break;
				}
				else
				{
					UOSInt codeSize = 1;
					UOSInt i;
					*(UInt16*)&this->lzwTable[this->currTableSize * 4] = (UInt16)this->localCode;
					this->localCode = code;
					while (code >= this->resetCode)
					{
						code = *(UInt16*)&this->lzwTable[code * 4];
						codeSize++;
					}
					this->lzwTable[this->currTableSize * 4 + 2] = (UInt8)code;
					this->currTableSize++;

					if (this->currTableSize >= this->nextTableSize - codeSizeAdj)
					{
						if (this->currCodeSize < this->maxCodeSize)
						{
							this->currCodeSize = this->currCodeSize + 1;
							this->nextTableSize = ((UOSInt)1 << this->currCodeSize);
						}
					}
	
					code = this->localCode;
					if (codeSize >= size)
					{
						i = codeSize - size;
						this->decBuffSize = i;
						while (i-- > 0)
						{
							this->decBuff[i] = this->lzwTable[code * 4 + 2];
							code = *(UInt16*)&this->lzwTable[code * 4];
						}
						i = size;
						while (i-- > 0)
						{
							buff[i] = this->lzwTable[code * 4 + 2];
							code = *(UInt16*)&this->lzwTable[code * 4];
						}
						buff += size;
						writeSize += size;
						return writeSize;
					}
					else
					{
						writeSize += codeSize;
						size -= codeSize;
						i = codeSize;
						while (i-- > 0)
						{
							buff[i] = this->lzwTable[code * 4 + 2];
							code = *(UInt16*)&this->lzwTable[code * 4];
						}
						buff += codeSize;
					}
				}
			}
		}
	}
	return writeSize;
}

UOSInt Data::Compress::LZWDecStream::Write(const UInt8 *buff, UOSInt size)
{
	return 0;
}

Int32 Data::Compress::LZWDecStream::Flush()
{
	return 0;
}

void Data::Compress::LZWDecStream::Close()
{
}

Bool Data::Compress::LZWDecStream::Recover()
{
	return true;
}
