#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "IO/StmData/FileData.h"
#include "Math/Math_C.h"
#include "Media/AudioFormat.h"
#include "Media/MediaFile.h"
#include "Media/AFilter/FileMixFilter.h"
#include "Text/MyString.h"

Media::AFilter::FileMixFilter::FileMixFilter(NN<AudioSource> sourceAudio, NN<Parser::ParserList> parsers) : Media::AudioFilter(sourceAudio)
{
	sourceAudio->GetFormat(this->format);
	this->parsers = parsers;
	this->fileSrc = nullptr;
	this->mixing = false;
}

Media::AFilter::FileMixFilter::~FileMixFilter()
{
	this->fileSrc.Delete();
}

void Media::AFilter::FileMixFilter::GetFormat(NN<AudioFormat> format)
{
	format->FromAudioFormat(this->format);
}

UOSInt Media::AFilter::FileMixFilter::ReadBlock(Data::ByteArray blk)
{
	UOSInt readSize = this->sourceAudio->ReadBlock(blk);
	NN<Media::AudioSource> fileSrc;
	if (this->mixing && this->fileSrc.SetTo(fileSrc))
	{
		UOSInt sampleCnt = readSize / this->format.align;
		UOSInt readCnt;
		OSInt i;
		OSInt j;
		UOSInt k;
		UOSInt l;
		Int32 v;
		if (this->chMix)
		{
			Data::ByteBuffer fileBuff(sampleCnt * this->format.bitpersample);
			readCnt = fileSrc->ReadSample(this->mixOfst, sampleCnt, fileBuff);
			if (this->format.bitpersample == 16)
			{
				i = 0;
				j = 0;
				k = readCnt;
				while (k-- > 0)
				{
					l = this->format.nChannels;
					while (l-- > 0)
					{
						v = ReadInt16(&blk[j]) + (Int32)ReadInt16(&fileBuff[i]);
						if (v > 32767)
						{
							v = 32767;
						}
						else if (v < -32768)
						{
							v = -32768;
						}
						WriteInt16(&blk[j], v);
						j += 2;
					}
					i += 2;
				}
			}
			if (sampleCnt != readCnt)
			{
				this->mixing = false;
			}
			else
			{
				this->mixOfst += readCnt;
			}
		}
		else
		{
			Data::ByteBuffer fileBuff(sampleCnt * this->format.align);
			readCnt = fileSrc->ReadSample(this->mixOfst, sampleCnt, fileBuff);
			if (this->format.bitpersample == 16)
			{
				i = 0;
				j = 0;
				k = readCnt * this->format.nChannels;
				while (k-- > 0)
				{
					v = ReadInt16(&blk[j]) + (Int32)ReadInt16(&fileBuff[i]);
					if (v > 32767)
					{
						v = 32767;
					}
					else if (v < -32768)
					{
						v = -32768;
					}
					WriteInt16(&blk[j], v);
					j += 2;
					i += 2;
				}
			}
			if (sampleCnt != readCnt)
			{
				this->mixing = false;
			}
			else
			{
				this->mixOfst += readCnt;
			}
		}
	}
	return readSize;
}

Bool Media::AFilter::FileMixFilter::LoadFile(NN<Text::String> fileName)
{
	NN<Media::MediaFile> mf;
	{
		IO::StmData::FileData fd(fileName, false);
		if (!Optional<Media::MediaFile>::ConvertFrom(this->parsers->ParseFileType(fd, IO::ParserType::MediaFile)).SetTo(mf))
			return false;
	}
	this->StopMix();
	NN<Media::MediaSource> mediaSrc;
	NN<Media::AudioSource> audSrc;
	Int32 syncTime;
	UOSInt i;
	Bool found = false;
	i = 0;
	while (true)
	{
		if (!mf->GetStream(i, syncTime).SetTo(mediaSrc))
		{
			break;
		}
		else if (mediaSrc->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
		{
			Media::AudioFormat fmt;
			audSrc = NN<Media::AudioSource>::ConvertFrom(mediaSrc);
			audSrc->GetFormat(fmt);
			if (audSrc->SupportSampleRead() && fmt.frequency == this->format.frequency && this->format.bitpersample == fmt.bitpersample)
			{
				if (this->format.nChannels == fmt.nChannels || fmt.nChannels == 1)
				{
					found = true;
					this->chMix = (this->format.nChannels != fmt.nChannels);
					mf->KeepStream(i, true);
					mf.Delete();
					this->fileSrc.Delete();
					this->fileSrc = NN<Media::AudioSource>::ConvertFrom(mediaSrc);
					return true;
				}
			}
		}
		i++;
	}
	if (!found)
	{
		mf.Delete();
	}
	return false;
}

Bool Media::AFilter::FileMixFilter::StartMix()
{
	if (this->fileSrc.NotNull())
	{
		this->mixOfst = 0;
		this->mixing = true;
		return true;
	}
	return false;
}

Bool Media::AFilter::FileMixFilter::StopMix()
{
	this->mixing = false;
	return true;
}
