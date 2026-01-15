#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "IO/StreamData.h"
#include "Media/MediaSource.h"
#include "Media/AudioSource.h"
#include "Media/LPCMSource.h"
#include "Sync/Event.h"
#include "Text/MyString.h"

Media::LPCMSource::LPCMSource(NN<Text::String> name)
{
	this->format.Clear();
	this->data = nullptr;
	this->name = name->Clone();
	this->readEvt = nullptr;
	this->readOfst = 0;
}

Media::LPCMSource::LPCMSource(Text::CStringNN name)
{
	this->format.Clear();
	this->data = nullptr;
	this->name = Text::String::New(name);
	this->readEvt = nullptr;
	this->readOfst = 0;
}

void Media::LPCMSource::SetData(NN<IO::StreamData> fd, UInt64 ofst, UInt64 length, NN<const Media::AudioFormat> format)
{
	this->data.Delete();
	this->format.FromAudioFormat(format);

	this->data = fd->GetPartialData(ofst, length);
}

Media::LPCMSource::LPCMSource(NN<IO::StreamData> fd, UInt64 ofst, UInt64 length, NN<const Media::AudioFormat> format, NN<Text::String> name)
{
	this->format.FromAudioFormat(format);
	this->data = fd->GetPartialData(ofst, length);
	this->name = name->Clone();
	this->readEvt = nullptr;
	this->readOfst = 0;
}

Media::LPCMSource::LPCMSource(NN<IO::StreamData> fd, UInt64 ofst, UInt64 length, NN<const Media::AudioFormat> format, Text::CStringNN name)
{
	this->format.FromAudioFormat(format);
	this->data = fd->GetPartialData(ofst, length);
	this->name = Text::String::New(name);
	this->readEvt = nullptr;
	this->readOfst = 0;
}

Media::LPCMSource::~LPCMSource()
{
	this->data.Delete();
	this->name->Release();
}

UnsafeArrayOpt<UTF8Char> Media::LPCMSource::GetSourceName(UnsafeArray<UTF8Char> buff)
{
	return this->name->ConcatTo(buff);
}

Bool Media::LPCMSource::CanSeek()
{
	return true;
}

Data::Duration Media::LPCMSource::GetStreamTime()
{
	NN<IO::StreamData> data;
	if (this->data.SetTo(data))
	{
		return Data::Duration::FromRatioU64(data->GetDataSize(), (this->format.nChannels * (UInt32)this->format.bitpersample >> 3) * this->format.frequency);
	}
	else
	{
		return 0;
	}
}

Data::Duration Media::LPCMSource::SeekToTime(Data::Duration time)
{
	UInt32 blk = (this->format.nChannels * (UInt32)this->format.bitpersample >> 3);
	UInt32 bytesPerSec = blk * this->format.frequency;
	this->readOfst = time.MultiplyU64(this->format.frequency) * blk;
	return Data::Duration::FromRatioU64(this->readOfst, bytesPerSec);
}

Bool Media::LPCMSource::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime)
{
	NN<IO::StreamData> data;
	if (!this->data.SetTo(data))
		return false;
	UInt32 blk = (this->format.nChannels * (UInt32)this->format.bitpersample >> 3);
	if (trimTimeEnd == (UInt32)-1)
	{
		if (trimTimeStart >= 0)
		{
			UInt64 ofst = trimTimeStart * (UInt64)this->format.frequency / 1000 * blk;
			NN<IO::StreamData> newData = data->GetPartialData(ofst, data->GetDataSize() - ofst);
			this->data.Delete();
			this->data = newData;
			syncTime.Set(0);
		}
		else
		{
			syncTime.Set((Int32)trimTimeStart);
		}
	}
	else
	{
		UInt64 ofst1 = trimTimeStart * (UInt64)this->format.frequency / 1000 * blk;
		UInt64 ofst2 = trimTimeEnd * (UInt64)this->format.frequency / 1000 * blk;
		UInt64 dataSize = data->GetDataSize();
		if (ofst2 > dataSize)
			ofst2 = dataSize;
		if (trimTimeStart >= 0)
		{
			NN<IO::StreamData> newData = data->GetPartialData(ofst1, ofst2 - ofst1);
			this->data.Delete();
			this->data = newData;
			syncTime.Set(0);
		}
		else
		{
			NN<IO::StreamData> newData = data->GetPartialData(0, ofst2);
			this->data.Delete();
			this->data = newData;
			syncTime.Set((Int32)trimTimeStart);
		}
	}
	return true;
}

void Media::LPCMSource::GetFormat(NN<AudioFormat> format)
{
	format->FromAudioFormat(this->format);
}

Bool Media::LPCMSource::Start(Optional<Sync::Event> evt, UIntOS blkSize)
{
	NN<Sync::Event> readEvt;
	this->readEvt = evt;
	if (this->readEvt.SetTo(readEvt))
		readEvt->Set();
	return true;
}

void Media::LPCMSource::Stop()
{
	this->readEvt = nullptr;
	this->readOfst = 0;
}

UIntOS Media::LPCMSource::ReadBlock(Data::ByteArray buff)
{
	NN<IO::StreamData> data;
	if (!this->data.SetTo(data))
		return 0;
	NN<Sync::Event> readEvt;
	UIntOS readSize = 0;
#ifndef HAS_ASM32
	UIntOS i;
#endif
	if (this->format.intType == Media::AudioFormat::IT_BIGENDIAN)
	{
		UInt32 blk = (this->format.nChannels * (UInt32)this->format.bitpersample >> 3);
		readSize = buff.GetSize() / blk;
		readSize = data->GetRealData(this->readOfst, readSize * blk, buff);
		if (this->format.bitpersample == 16)
		{
#ifdef HAS_ASM32
			_asm
			{
				mov esi,buff
				mov ecx,readSize
				shr ecx,1
				jz rblk1_16exit
rblk1_16lop:
				lodsw
				mov byte ptr [esi - 2],ah
				mov byte ptr [esi - 1],al
				dec ecx
				jnz rblk1_16lop
rblk1_16exit:
			}
#else
			if ((i = readSize >> 1) != 0)
			{
				UInt8 tmp;
				while (i-- > 0)
				{
					tmp = buff[0];
					buff[0] = buff[1];
					buff[1] = tmp;
					buff += 2;
				}
			}
#endif
		}
		else if (this->format.bitpersample == 24)
		{
#ifdef HAS_ASM32
			_asm
			{
				mov esi,buff
				mov eax,readSize
				mov edx,0
				mov ecx,3
				div ecx
				mov ecx,eax
				cmp ecx,0
				jz rblk1_24exit
rblk1_24lop:
				mov al, byte ptr [esi]
				mov dl, byte ptr [esi+2]
				mov byte ptr [esi],dl
				mov byte ptr [esi+2],al
				add esi,3
				dec ecx
				jnz rblk1_24lop
rblk1_24exit:
			}
#else
			if ((i = readSize / 3) != 0)
			{
				UInt8 tmp;
				while (i-- > 0)
				{
					tmp = buff[0];
					buff[0] = buff[2];
					buff[2] = tmp;
					buff += 3;
				}
			}
#endif
		}
	}
	else if (this->format.intType == Media::AudioFormat::IT_BIGENDIAN16)
	{
		if (this->format.bitpersample == 16)
		{
			UInt32 blk = (this->format.nChannels * (UInt32)this->format.bitpersample >> 3);
			readSize = buff.GetSize() / blk;
			readSize = data->GetRealData(this->readOfst, readSize * blk, buff);
#ifdef HAS_ASM32
			_asm
			{
				mov esi,buff
				mov ecx,readSize
				shr ecx,1
				jz rblk2_16exit
rblk2_16lop:
				lodsw
				mov byte ptr [esi - 2],ah
				mov byte ptr [esi - 1],al
				dec ecx
				jnz rblk2_16lop
rblk2_16exit:
			}
#else
			if ((i = readSize >> 1) != 0)
			{
				UInt8 tmp;
				while (i-- > 0)
				{
					tmp = buff[0];
					buff[0] = buff[1];
					buff[1] = tmp;
					buff += 2;
				}
			}
#endif
		}
		else if (this->format.bitpersample == 24)
		{
			if (this->format.nChannels == 2)
			{
				UInt32 blk = 12;//(this->format.nChannels * this->format.bitpersample >> 3);
				readSize = buff.GetSize() / blk;
				readSize = data->GetRealData(this->readOfst, readSize * blk, buff);
#ifdef HAS_ASM32
				_asm
				{
					mov esi,buff
					mov eax,readSize
					mov edx,0
					mov ecx,12
					div ecx
					mov ecx,eax
					cmp ecx,0
					jz rblk2_24_2exit
rblk2_24_2lop:
					mov ax,word ptr [esi]
					mov dx,word ptr [esi+2]
					mov byte ptr [esi+1],ah
					mov byte ptr [esi+2],al
					mov ax,word ptr [esi+4]
					mov byte ptr [esi+4],dh
					mov byte ptr [esi+5],dl
					mov dx,word ptr [esi+8]
					mov byte ptr [esi],dl
					mov byte ptr [esi+3],dh
					mov dx,word ptr [esi+6]
					mov byte ptr [esi+7],ah
					mov byte ptr [esi+8],al
					mov ax,word ptr [esi+10]
					mov byte ptr [esi+10],dh
					mov byte ptr [esi+11],dl
					mov byte ptr [esi+6],al
					mov byte ptr [esi+9],ah
					add esi,12
					dec ecx
					jnz rblk2_24_2lop
rblk2_24_2exit:
				}
#else
				if ((i = readSize / 12) != 0)
				{
					UInt8 tmpVal[4];
					while (i-- > 0)
					{
						*(Int32*)tmpVal = buff.ReadNI32(0);
						buff[1] = tmpVal[1];
						buff[2] = tmpVal[0];
						*(Int16*)tmpVal = *(Int16*)&buff[4];
						buff[4] = tmpVal[3];
						buff[5] = tmpVal[2];
						*(Int16*)&tmpVal[2] = *(Int16*)&buff[8];
						buff[0] = tmpVal[2];
						buff[3] = tmpVal[3];
						*(Int16*)&tmpVal[2] = *(Int16*)&buff[6];
						buff[7] = tmpVal[1];
						buff[8] = tmpVal[0];
						*(Int16*)&tmpVal[0] = *(Int16*)&buff[10];
						buff[10] = tmpVal[3];
						buff[11] = tmpVal[2];
						buff[6] = tmpVal[0];
						buff[9] = tmpVal[1];
						buff += 12;
					}
				}
#endif
			}
		}
	}
	else if (this->format.other)
	{
		UInt32 blk = this->format.nChannels * (UInt32)(this->format.bitpersample >> 3) * this->format.other;
		readSize = buff.GetSize() / blk;
		if (readSize > 0)
		{
			UInt32 ofstPC = (UInt32)(this->format.bitpersample >> 3) * this->format.other;
			Data::ByteBuffer tmpBuff(buff.GetSize() * blk);
			Data::ByteArray tmpPtr;
			Data::ByteArray tmpPtr2;
			UIntOS cnt;
			UIntOS cnt2;
			readSize = data->GetRealData(this->readOfst, readSize * blk, tmpBuff);
			UIntOS sizeLeft = readSize;
			tmpPtr = tmpBuff;
			while (sizeLeft >= blk)
			{
				cnt = this->format.other;
				while (cnt-- > 0)
				{
					tmpPtr2 = tmpPtr;
					cnt2 = this->format.nChannels;
					while (cnt2-- > 0)
					{
						buff.WriteNI16(0, tmpPtr2.ReadNI16(0));
						buff += 2;
						tmpPtr2 += ofstPC;
					}
					tmpPtr += 2;
				}
				tmpPtr += blk - ofstPC;
				sizeLeft -= blk;
			}
		}
	}
	else
	{
		UInt32 blk = (this->format.nChannels * (UInt32)this->format.bitpersample >> 3);
		readSize = buff.GetSize() / blk;
		readSize = data->GetRealData(this->readOfst, readSize * blk, buff);
	}
	this->readOfst += readSize;
	if (this->readEvt.SetTo(readEvt))
		readEvt->Set();
	return readSize;
}

UIntOS Media::LPCMSource::GetMinBlockSize()
{
	return this->format.nChannels * (UIntOS)(this->format.bitpersample >> 3);
}

Data::Duration Media::LPCMSource::GetCurrTime()
{
	return Data::Duration::FromRatioU64(this->readOfst, (this->format.nChannels * (UInt32)this->format.bitpersample >> 3) * this->format.frequency);
}

Bool Media::LPCMSource::IsEnd()
{
	NN<IO::StreamData> data;
	if (this->data.SetTo(data))
		return this->readOfst >= data->GetDataSize();
	return true;
}

Bool Media::LPCMSource::SupportSampleRead()
{
	return true;
}

UIntOS Media::LPCMSource::ReadSample(UInt64 sampleOfst, UIntOS sampleCount, Data::ByteArray buff)
{
	NN<IO::StreamData> data;
	if (!this->data.SetTo(data))
		return 0;
	UIntOS blk = (this->format.nChannels * (UInt32)this->format.bitpersample >> 3);
	if (sampleOfst < 0)
	{
		if (sampleOfst + sampleCount > 0)
		{
			buff.Clear(0, (UIntOS)-(Int64)sampleOfst * blk);
			return (UIntOS)((data->GetRealData(0, (UIntOS)(sampleCount + sampleOfst) * blk, buff - sampleOfst * blk) / blk) - sampleOfst);
		}
		else
		{
			buff.Clear(0, sampleCount * blk);
			return sampleCount;
		}
	}
	else
	{
		return data->GetRealData(sampleOfst * blk, sampleCount * blk, buff) / blk;
	}
}

Int64 Media::LPCMSource::GetSampleCount()
{
	NN<IO::StreamData> data;
	if (!this->data.SetTo(data))
		return 0;
	UInt32 blk = (this->format.nChannels * (UInt32)this->format.bitpersample >> 3);
	return (Int64)data->GetDataSize() / blk;
}
