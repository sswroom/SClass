#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/StmData/FileData.h"
#include "Math/Math.h"
#include "Media/AudioFormat.h"
#include "Media/MediaFile.h"
#include "Media/AudioFilter/FileMixFilter.h"
#include "Text/MyString.h"

Media::AudioFilter::FileMixFilter::FileMixFilter(NotNullPtr<IAudioSource> sourceAudio, NotNullPtr<Parser::ParserList> parsers) : Media::IAudioFilter(sourceAudio)
{
	sourceAudio->GetFormat(this->format);
	this->parsers = parsers;
	this->fileSrc = 0;
	this->mixing = false;
}

Media::AudioFilter::FileMixFilter::~FileMixFilter()
{
	if (this->fileSrc)
	{
		DEL_CLASS(this->fileSrc);
		this->fileSrc = 0;
	}
}

void Media::AudioFilter::FileMixFilter::GetFormat(AudioFormat *format)
{
	format->FromAudioFormat(this->format);
}

UOSInt Media::AudioFilter::FileMixFilter::ReadBlock(Data::ByteArray blk)
{
	UOSInt readSize = this->sourceAudio->ReadBlock(blk);
	if (this->mixing)
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
			readCnt = this->fileSrc->ReadSample(this->mixOfst, sampleCnt, fileBuff);
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
			readCnt = this->fileSrc->ReadSample(this->mixOfst, sampleCnt, fileBuff);
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

Bool Media::AudioFilter::FileMixFilter::LoadFile(NotNullPtr<Text::String> fileName)
{
	Media::MediaFile *mf;
	{
		IO::StmData::FileData fd(fileName, false);
		mf = (Media::MediaFile*)this->parsers->ParseFileType(fd, IO::ParserType::MediaFile);
	}
	if (mf)
	{
		this->StopMix();
		Media::IMediaSource *mediaSrc;
		Media::IAudioSource *audSrc;
		Int32 syncTime;
		UOSInt i;
		Bool found = false;
		i = 0;
		while (true)
		{
			mediaSrc = mf->GetStream(i, &syncTime);
			if (mediaSrc == 0)
			{
				break;
			}
			else if (mediaSrc->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
			{
				Media::AudioFormat fmt;
				audSrc = (Media::IAudioSource*)mediaSrc;
				audSrc->GetFormat(fmt);
				if (audSrc->SupportSampleRead() && fmt.frequency == this->format.frequency && this->format.bitpersample == fmt.bitpersample)
				{
					if (this->format.nChannels == fmt.nChannels || fmt.nChannels == 1)
					{
						found = true;
						this->chMix = (this->format.nChannels != fmt.nChannels);
						mf->KeepStream(i, true);
						DEL_CLASS(mf);
						if (this->fileSrc)
						{
							DEL_CLASS(this->fileSrc);
						}
						this->fileSrc = (Media::IAudioSource*)mediaSrc;
						return true;
					}
				}
			}
			i++;
		}
		if (!found)
		{
			DEL_CLASS(mf);
		}
		return false;
	}
	else
	{
		return false;
	}
}

Bool Media::AudioFilter::FileMixFilter::StartMix()
{
	if (this->fileSrc)
	{
		this->mixOfst = 0;
		this->mixing = true;
		return true;
	}
	return false;
}

Bool Media::AudioFilter::FileMixFilter::StopMix()
{
	this->mixing = false;
	return true;
}
