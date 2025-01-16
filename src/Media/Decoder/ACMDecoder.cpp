#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Media/Decoder/ACMDecoder.h"
#include <windows.h>
#include <mmreg.h>
#include <mmsystem.h>
#include <msacm.h>

void Media::Decoder::ACMDecoder::FreeACM()
{
	if (this->acmFmt)
	{
		MemFree(this->acmFmt);
		this->acmFmt = 0;
	}
	if (this->decFmt)
	{
		MemFree(this->decFmt);
		this->decFmt = 0;
	}
	if (this->hAcmStream)
	{
		acmStreamClose((HACMSTREAM)this->hAcmStream, 0);
		this->hAcmStream = 0;
	}
	if (this->acmsh)
	{
		MemFree(this->acmsh);
		this->acmsh = 0;
	}
	if (this->acmInpBuff)
	{
		MemFree(this->acmInpBuff);
		this->acmInpBuff = 0;
	}
	if (this->acmOupBuff)
	{
		MemFree(this->acmOupBuff);
		this->acmOupBuff = 0;
	}
}

void Media::Decoder::ACMDecoder::InitACM()
{
	NN<Media::IAudioSource> sourceAudio;
	UInt32 i;
	Media::AudioFormat format;
	WAVEFORMATEX *fmt;
	WAVEFORMATEX *acmFmt;
	if (!this->sourceAudio.SetTo(sourceAudio))
		return;
	sourceAudio->GetFormat(format);
	fmt = (WAVEFORMATEX*)MAlloc(18 + format.extraSize);
	fmt->wFormatTag = (WORD)format.formatId;
	fmt->nChannels = format.nChannels;
	fmt->nSamplesPerSec = format.frequency;
	fmt->nBlockAlign = (WORD)format.align;
	fmt->nAvgBytesPerSec = format.bitRate >> 3;
	fmt->wBitsPerSample = format.bitpersample;
	fmt->cbSize = (WORD)format.extraSize;
	if (format.extraSize > 0)
	{
		MemCopyNO(18 + (UInt8*)fmt, format.extra, fmt->cbSize);
	}
	if (fmt->wFormatTag == 0x1610)
	{
		fmt->wFormatTag = 255;
		fmt->nSamplesPerSec = fmt->nSamplesPerSec >> 1;
	}


	if (acmMetrics(NULL, ACM_METRIC_MAX_SIZE_FORMAT, (LPVOID)&i))
	{
		MemFree(fmt);
		fmt = 0;
		return;
	}

	acmFmt = (WAVEFORMATEX*)MAlloc(i);
	acmFmt->wFormatTag = 1;
	acmFmt->nChannels = fmt->nChannels;
	acmFmt->wBitsPerSample = format.bitpersample;
	acmFmt->nSamplesPerSec = format.frequency;
	if (acmFormatSuggest(NULL, fmt, acmFmt, i, ACM_FORMATSUGGESTF_WFORMATTAG))
	{
		MemFree(fmt);
		MemFree(acmFmt);
		return;
	}

	HACMSTREAM hAcmStream;
	UInt32 acmOupBuffSize;
	ACMSTREAMHEADER *acmsh;
	if (acmStreamOpen(&hAcmStream, NULL, fmt, acmFmt, NULL, 0, 0, ACM_STREAMOPENF_NONREALTIME))
	{
		MemFree(acmFmt);
		MemFree(fmt);
		return;
	}

	this->acmInpBuffSize = sourceAudio->GetMinBlockSize();
	if (this->acmInpBuffSize < fmt->nAvgBytesPerSec)
	{
		this->acmInpBuffSize = fmt->nAvgBytesPerSec;
	}

	if (acmStreamSize(hAcmStream, (UInt32)this->acmInpBuffSize, (LPDWORD)&acmOupBuffSize, ACM_STREAMSIZEF_SOURCE))
	{
		acmStreamClose(hAcmStream, 0);
		hAcmStream = 0;
		MemFree(acmFmt);
		MemFree(fmt);
		return;
	}
	
	acmsh = MemAlloc(ACMSTREAMHEADER, 1);
	ZeroMemory(acmsh, sizeof(ACMSTREAMHEADER));
	acmsh->cbStruct = sizeof(ACMSTREAMHEADER);
	this->acmInpBuff = MemAlloc(UInt8, this->acmInpBuffSize);
	acmOupBuff = MemAlloc(UInt8, acmOupBuffSize);
	acmsh->pbSrc = (LPBYTE)acmInpBuff;
	acmsh->cbSrcLength = fmt->nAvgBytesPerSec;
	acmsh->pbDst = (LPBYTE)acmOupBuff;
	acmsh->cbDstLength = acmOupBuffSize;

	if (acmStreamPrepareHeader(hAcmStream, acmsh, 0))
	{
		acmStreamClose(hAcmStream, 0);
		hAcmStream = 0;
		MemFree(acmsh);
		MemFree(acmOupBuff);
		MemFree(acmInpBuff);
		MemFree(acmFmt);
		MemFree(fmt);
		return;
	}

	acmsh->cbSrcLength = 0;
	acmsh->cbSrcLengthUsed = 0;
	acmsh->cbDstLengthUsed = 0;

	this->acmFmt = acmFmt;
	this->hAcmStream = hAcmStream;
	this->acmsh = (UInt8*)acmsh;
	this->acmInpBuff = acmInpBuff;
	this->acmOupBuff = acmOupBuff;
	this->acmOupBuffSize = acmOupBuffSize;
	this->decFmt = MemAlloc(Media::AudioFormat, 1);
	this->decFmt->formatId = acmFmt->wFormatTag;
	this->decFmt->frequency = acmFmt->nSamplesPerSec;
	this->decFmt->nChannels = acmFmt->nChannels;
	this->decFmt->bitpersample = acmFmt->wBitsPerSample;
	this->decFmt->bitRate = acmFmt->nAvgBytesPerSec << 3;
	this->decFmt->align = acmFmt->nBlockAlign;
	this->decFmt->intType = Media::AudioFormat::IT_NORMAL;
	this->decFmt->other = 0;
	this->decFmt->extra = 0;
	this->decFmt->extraSize = 0;
	this->srcFormatTag = format.formatId;
	MemFree(fmt);
	if (format.formatId == 0x1610)
	{
		this->decFmt->frequency *= 2;
	}
}

Media::Decoder::ACMDecoder::ACMDecoder(NN<Media::IAudioSource> sourceAudio)
{
	this->hAcmStream = 0;
	this->acmFmt = 0;
	this->decFmt = 0;
	this->acmsh = 0;
	this->acmInpBuff = 0;
	this->acmOupBuff = 0;
	this->seeked = true;
	this->sourceAudio = sourceAudio;
	this->srcFormatTag = 0;
	InitACM();
}

Media::Decoder::ACMDecoder::~ACMDecoder()
{
	FreeACM();
}

void Media::Decoder::ACMDecoder::GetFormat(NN<AudioFormat> format)
{
	NN<Media::AudioFormat> decFmt;
	if (decFmt.Set(this->decFmt))
	{
		format->FromAudioFormat(decFmt);
	}
	else
	{
		format->formatId = 0;
	}
}

Data::Duration Media::Decoder::ACMDecoder::SeekToTime(Data::Duration time)
{
	NN<Media::IAudioSource> sourceAudio;
	if (this->sourceAudio.SetTo(sourceAudio))
	{
		this->seeked = true;
		this->acmOupBuffLeft = 0;
		return sourceAudio->SeekToTime(time);
	}
	return 0;
}

Bool Media::Decoder::ACMDecoder::Start(Optional<Sync::Event> evt, UOSInt blkSize)
{
	NN<Sync::Event> readEvt;
	NN<Media::IAudioSource> sourceAudio;
	if (this->sourceAudio.SetTo(sourceAudio))
	{
		this->seeked = true;
		this->acmOupBuffLeft = 0;
		sourceAudio->Start(0, blkSize);
		this->readEvt = evt;

		if (this->readEvt.SetTo(readEvt))
			readEvt->Set();
		return true;
	}
	return false;
}

void Media::Decoder::ACMDecoder::Stop()
{
	NN<Media::IAudioSource> sourceAudio;
	if (this->sourceAudio.SetTo(sourceAudio))
	{
		sourceAudio->Stop();
	}
	this->readEvt = 0;
}

UOSInt Media::Decoder::ACMDecoder::ReadBlock(Data::ByteArray buff)
{
	ACMSTREAMHEADER *acmsh = (ACMSTREAMHEADER*)this->acmsh;
	NN<Media::IAudioSource> sourceAudio;
	NN<Sync::Event> readEvt;
	UOSInt outSize = 0;
	UOSInt i;
	if (!this->sourceAudio.SetTo(sourceAudio))
		return 0;
	if (this->decFmt->align == 0)
	{
		if (this->decFmt->frequency == 0)
			return 0;
		else
		{
			this->decFmt->align = 1;
		}
	}
	i = buff.GetSize() % this->decFmt->align;
	if (i)
	{
		buff = buff.WithSize(buff.GetSize() - i);
	}

	while (buff.GetSize() > 0)
	{
		if (this->acmOupBuffLeft)
		{
			if (this->acmOupBuffLeft < buff.GetSize())
			{
				buff.CopyFrom(Data::ByteArrayR(this->acmOupBuff, this->acmOupBuffLeft));
				buff += this->acmOupBuffLeft;
				outSize += this->acmOupBuffLeft;
				this->acmOupBuffLeft = 0;
			}
			else
			{
				buff.CopyFrom(Data::ByteArrayR(this->acmOupBuff, buff.GetSize()));
				outSize += buff.GetSize();
				if (this->acmOupBuffLeft > buff.GetSize())
				{
					MemCopyO(this->acmOupBuff, &this->acmOupBuff[buff.GetSize()], this->acmOupBuffLeft - buff.GetSize());
					this->acmOupBuffLeft -= buff.GetSize();
					if (this->readEvt.SetTo(readEvt))
						readEvt->Set();
					return outSize;
				}
				else
				{
					this->acmOupBuffLeft = 0;
					if (this->readEvt.SetTo(readEvt))
						readEvt->Set();
					return outSize;
				}
			}
		}

		acmsh->cbDstLengthUsed = 0;
		UOSInt srcSize = sourceAudio->ReadBlock(Data::ByteArray(this->acmInpBuff, this->acmInpBuffSize));
		if (srcSize == 0)
		{
			if (this->readEvt.SetTo(readEvt))
				readEvt->Set();
			return outSize;
		}
		acmsh->cbSrcLength = (UInt32)srcSize;
		UInt32 convFlag;
		if (this->seeked)
		{
			convFlag = ACM_STREAMCONVERTF_BLOCKALIGN | ACM_STREAMCONVERTF_START;
			this->seeked = false;
		}
		else
		{
			convFlag = ACM_STREAMCONVERTF_BLOCKALIGN;
		}
		MMRESULT ret;
		if ((ret = acmStreamConvert((HACMSTREAM)hAcmStream, acmsh, convFlag)) != 0)
		{
			if (this->readEvt.SetTo(readEvt))
				readEvt->Set();
			return outSize;
		}
		if (acmsh->cbDstLengthUsed > 0)
		{
			this->acmOupBuffLeft = acmsh->cbDstLengthUsed;
		}
		this->acmOupBuffLeft = acmsh->cbDstLengthUsed;
	}
	if (this->readEvt.SetTo(readEvt))
		readEvt->Set();
	return outSize;
}

UOSInt Media::Decoder::ACMDecoder::GetMinBlockSize()
{
	return this->decFmt->align;
}
