#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/BitReaderLSB.h"
#include "IO/StreamData.h"
#include "Media/IMediaSource.h"
#include "Media/IAudioSource.h"
#include "Media/NWASource.h"
#include "Text/MyString.h"
#include "Sync/Event.h"

Media::NWASource::NWASource(IO::StreamData *fd, UInt32 sampleCount, UInt32 blockSize, UInt32 compLevel, UInt32 nBlocks, Media::AudioFormat *format, Text::String *name) : Media::LPCMSource(fd, 0, fd->GetDataSize(), format, name)
{
	this->sampleCount = sampleCount;
	this->blockSize = blockSize;
	this->compLevel = compLevel;
	this->nBlocks = nBlocks;
	this->blockOfsts = MemAlloc(UInt32, this->nBlocks);
	this->blockBuff = MemAlloc(UInt8, blockSize * (this->format.bitpersample / 8) * 2);
	this->currBlock = 0;
	fd->GetRealData(0x2c, this->nBlocks * 4, (UInt8*)this->blockOfsts);
}

Media::NWASource::NWASource(IO::StreamData *fd, UInt32 sampleCount, UInt32 blockSize, UInt32 compLevel, UInt32 nBlocks, Media::AudioFormat *format, Text::CString name) : Media::LPCMSource(fd, 0, fd->GetDataSize(), format, name)
{
	this->sampleCount = sampleCount;
	this->blockSize = blockSize;
	this->compLevel = compLevel;
	this->nBlocks = nBlocks;
	this->blockOfsts = MemAlloc(UInt32, this->nBlocks);
	this->blockBuff = MemAlloc(UInt8, blockSize * (this->format.bitpersample / 8) * 2);
	this->currBlock = 0;
	fd->GetRealData(0x2c, this->nBlocks * 4, (UInt8*)this->blockOfsts);
}

Media::NWASource::~NWASource()
{
	MemFree(this->blockOfsts);
	MemFree(this->blockBuff);
}

Int32 Media::NWASource::GetStreamTime()
{
	return (Int32)(this->sampleCount * 1000 / this->format.frequency / this->format.nChannels);
}

UInt32 Media::NWASource::SeekToTime(UInt32 time)
{
	UInt32 blkSample = this->blockSize / this->format.nChannels;
	this->currBlock = time * this->format.frequency / blkSample / 1000;
	return this->currBlock * blkSample * 1000 / this->format.frequency;
}

UOSInt Media::NWASource::ReadBlock(UInt8 *buff, UOSInt blkSize)
{
	UInt32 byps = (this->format.bitpersample / 8);
	UInt32 retSize = 0;
	while (blkSize > 0)
	{
		if (blkSize < this->blockSize * byps)
			return retSize;
		UInt32 decSize;
		UInt32 compBlockSize;
		if (this->currBlock == this->nBlocks - 1)
		{
			UInt32 restSize = this->sampleCount - (this->nBlocks - 1) * this->blockSize;
			decSize = restSize * byps;
			compBlockSize = this->blockSize * byps * 2;
		}
		else if (this->currBlock >= this->nBlocks)
		{
			return 0;
		}
		else
		{
			decSize = this->blockSize * byps;
			compBlockSize = this->blockOfsts[this->currBlock + 1] - this->blockOfsts[this->currBlock];
			if (compBlockSize > this->blockSize * byps * 2)
			{
				return 0;
			}
		}
		
		this->data->GetRealData(this->blockOfsts[this->currBlock], compBlockSize, this->blockBuff);
		this->currBlock++;
		Int32 d[2];
		UInt32 currOfst = 0;
		if (this->format.bitpersample == 8)
		{
			d[0] = (Int8)this->blockBuff[0];
			if (this->format.nChannels == 2)
			{
				d[1] = (Int8)this->blockBuff[1];
				currOfst = 2;
			}
			else
			{
				currOfst = 1;
			}
		}
		else
		{
			d[0] = ReadInt16(&this->blockBuff[0]);
			if (this->format.nChannels == 2)
			{
				d[1] = ReadInt16(&this->blockBuff[2]);
				currOfst = 4;
			}
			else
			{
				currOfst = 2;
			}
		}
		UInt32 bps = this->format.bitpersample;
		UInt32 dSize = decSize / (bps / 8);
		Int32 flipFlag = 0;
		UInt32 aType;
		UInt32 i;
		IO::BitReaderLSB rdr(&this->blockBuff[currOfst], compBlockSize - currOfst);

		UInt32 bits8 = 8 - this->compLevel;
		UInt32 mask81 = (UInt32)(1 << (bits8 - 1));
		UInt32 mask82 = mask81 - 1;
		UInt32 bits5 = 5 - this->compLevel;
		UInt32 mask51 = (UInt32)(1 << (bits5 - 1));
		UInt32 mask52 = mask51 - 1;
		UInt32 sh;
		UInt32 v;
		i = 0;
		while (i < dSize)
		{
			if (!rdr.ReadBits(&aType, 3))
				break;
			if (aType == 7)
			{
				if (rdr.ReadBits(&v, 1))
				{
					if (v == 1)
					{
						d[flipFlag] = 0;
					}
					else
					{
						sh = 2 + 7 + this->compLevel;
						if (rdr.ReadBits(&v, bits8))
						{
							if (v & mask81)
								d[flipFlag] -= (Int32)((v & mask82) << sh);
							else
								d[flipFlag] += (Int32)((v & mask82) << sh);
						}
						else
						{
							break;
						}
					}
				}
				else
				{
					break;
				}
			}
			else if (aType != 0)
			{
				sh = 2 + aType + this->compLevel;
				if (rdr.ReadBits(&v, bits5))
				{
					if (v & mask51)
						d[flipFlag] -= (Int32)((v & mask52) << sh);
					else
						d[flipFlag] += (Int32)((v & mask52) << sh);
				}
				else
				{
					break;
				}
			}
			else
			{
				if (false)
				{
					if (!rdr.ReadBits(&v, 1))
						break;
					if (v == 1)
					{
						if (!rdr.ReadBits(&v, 2))
							break;
						if (v == 3)
						{
							if (!rdr.ReadBits(&v, 8))
								break;
						}
					}

					v = i + v;
					if (v > dSize)
						v = dSize;
					while (i < v)
					{
						if (bps == 8)
						{
							*buff++ = (UInt8)d[flipFlag];
						}
						else
						{
							WriteInt16(buff, d[flipFlag]);
							buff += 2;
						}
						if (this->format.nChannels == 2)
							flipFlag ^= 1;
						i++;
					}
				}
			}


			if (bps == 8)
			{
				*buff++ = (UInt8)d[flipFlag];
			}
			else
			{
				WriteInt16(buff, d[flipFlag]);
				buff += 2;
			}
			if (this->format.nChannels == 2)
				flipFlag ^= 1;
			i++;
		}
		retSize += i * byps;
		blkSize -= i * byps;
		if (i != dSize)
			break;
	}
	return retSize;
}

UOSInt Media::NWASource::GetMinBlockSize()
{
	return this->blockSize * (this->format.bitpersample / 8);
}

UInt32 Media::NWASource::GetCurrTime()
{
	UInt32 blkSample = this->blockSize / this->format.nChannels;
	return this->currBlock * blkSample * 1000 / this->format.frequency;
}
