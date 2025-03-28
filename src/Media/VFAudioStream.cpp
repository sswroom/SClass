#include "Stdafx.h"
#include "Media/VFAudioStream.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>
#include "Media/VFAPI.h"

Media::VFAudioStream::VFAudioStream(NN<Media::VFMediaFile> mfile)
{
	this->mfile = mfile;
	{
		Sync::MutexUsage mutUsage(this->mfile->mut);
		this->mfile->useCnt++;
	}

	VF_StreamInfo_Audio ainfo;
	VF_PluginFunc *funcs = (VF_PluginFunc*)mfile->plugin->funcs;
	ainfo.dwSize = sizeof(ainfo);
	funcs->GetStreamInfo(mfile->file, VF_STREAM_AUDIO, &ainfo);
	this->fmt.formatId = 1;
	this->fmt.frequency = ainfo.dwRate / ainfo.dwScale;
	this->fmt.bitpersample = (UInt16)ainfo.dwBitsPerSample;
	this->fmt.nChannels = (UInt16)ainfo.dwChannels;
	this->fmt.bitRate = this->fmt.frequency * this->fmt.bitpersample * this->fmt.nChannels;
	this->fmt.align = this->fmt.frequency * this->fmt.nChannels * ((UInt32)this->fmt.bitpersample >> 3);
	this->fmt.other = 0;
	this->fmt.intType = Media::AudioFormat::IT_NORMAL;
	this->fmt.extraSize = 0;
	this->fmt.extra = 0;

	this->sampleCnt = *(UInt64*)&ainfo.dwLengthL;
	this->currSample = 0;
}

Media::VFAudioStream::~VFAudioStream()
{
	UOSInt useCnt;
	{
		Sync::MutexUsage mutUsage(this->mfile->mut);
		useCnt = --this->mfile->useCnt;
	}
	if (useCnt == 0)
	{
		Text::StrDelNew(this->mfile->fileName);
		VF_PluginFunc *funcs = (VF_PluginFunc*)this->mfile->plugin->funcs;
		funcs->CloseFile(this->mfile->file);
		this->mfile->vfpmgr->Release();
		this->mfile.Delete();
	}
}

UnsafeArrayOpt<UTF8Char> Media::VFAudioStream::GetSourceName(UnsafeArray<UTF8Char> buff)
{
	return Text::StrWChar_UTF8(buff, this->mfile->fileName);
}

Bool Media::VFAudioStream::CanSeek()
{
	return true;
}

Data::Duration Media::VFAudioStream::GetStreamTime()
{
	return Data::Duration::FromRatioU64(this->sampleCnt, this->fmt.frequency);
}

Data::Duration Media::VFAudioStream::SeekToTime(Data::Duration time)
{
	currSample = time.MultiplyU64(this->fmt.frequency);
	return Data::Duration::FromRatioU64(currSample, this->fmt.frequency);
}

Bool Media::VFAudioStream::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime)
{
	////////////////////////////////////////////
	return false;
}

void Media::VFAudioStream::GetFormat(NN<AudioFormat> format)
{
	format->FromAudioFormat(this->fmt);
}

Bool Media::VFAudioStream::Start(Optional<Sync::Event> evt, UOSInt blkSize)
{
	this->readEvt = evt;
	this->currSample = 0;
	return true;
}

void Media::VFAudioStream::Stop()
{
	this->readEvt = 0;
}

UOSInt Media::VFAudioStream::ReadBlock(Data::ByteArray blk)
{
	VF_PluginFunc *funcs = (VF_PluginFunc*)mfile->plugin->funcs;
	NN<Sync::Event> readEvt;
	UOSInt sampleCnt = blk.GetSize() / this->fmt.nChannels / ((UOSInt)this->fmt.bitpersample >> 3);
	if (sampleCnt + this->currSample > this->sampleCnt)
	{
		sampleCnt = (UOSInt)(this->sampleCnt - this->currSample);
	}
	if (sampleCnt <= 0)
	{
		if (this->readEvt.SetTo(readEvt))
			readEvt->Set();
		return 0;
	}
	UOSInt readSize = sampleCnt * this->fmt.nChannels * ((UOSInt)this->fmt.bitpersample >> 3);
	VF_ReadData_Audio rd;
	rd.dwSize = sizeof(rd);
	*(UInt64*)&rd.dwSamplePosL = this->currSample;
	rd.dwSampleCount = (DWORD)sampleCnt;
	rd.dwReadedSampleCount = 0;
	rd.dwBufSize = (DWORD)blk.GetSize();
	rd.lpBuf = blk.Arr().Ptr();
	funcs->ReadData(mfile->file, VF_STREAM_AUDIO, &rd);
	this->currSample += rd.dwReadedSampleCount;
	readSize = rd.dwReadedSampleCount * this->fmt.nChannels * ((UOSInt)this->fmt.bitpersample >> 3);

	if (this->readEvt.SetTo(readEvt))
		readEvt->Set();
	return readSize;
}

UOSInt Media::VFAudioStream::GetMinBlockSize()
{
	return this->fmt.nChannels * ((UOSInt)this->fmt.bitpersample >> 3);
}

Data::Duration Media::VFAudioStream::GetCurrTime()
{
	return Data::Duration::FromRatioU64(currSample, this->fmt.frequency);
}

Bool Media::VFAudioStream::IsEnd()
{
	return this->currSample >= this->sampleCnt;
}
