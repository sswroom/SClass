#include "Stdafx.h"
#include "Text/MyString.h"
#include "Media/VFAudioStream.h"
#include <windows.h>
#include "Media/VFAPI.h"

Media::VFAudioStream::VFAudioStream(Media::VFMediaFile *mfile)
{
	this->mfile = mfile;
	this->mfile->mut->Lock();
	this->mfile->useCnt++;
	this->mfile->mut->Unlock();

	VF_StreamInfo_Audio ainfo;
	VF_PluginFunc *funcs = (VF_PluginFunc*)mfile->plugin->funcs;
	ainfo.dwSize = sizeof(ainfo);
	funcs->GetStreamInfo(mfile->file, VF_STREAM_AUDIO, &ainfo);
	this->fmt.formatId = 1;
	this->fmt.frequency = ainfo.dwRate / ainfo.dwScale;
	this->fmt.bitpersample = (Int16)ainfo.dwBitsPerSample;
	this->fmt.nChannels = (Int16)ainfo.dwChannels;
	this->fmt.bitRate = this->fmt.frequency * this->fmt.bitpersample * this->fmt.nChannels;
	this->fmt.align = this->fmt.frequency * this->fmt.nChannels * (this->fmt.bitpersample >> 3);
	this->fmt.other = 0;
	this->fmt.intType = Media::AudioFormat::IT_NORMAL;
	this->fmt.extraSize = 0;
	this->fmt.extra = 0;

	this->sampleCnt = *(Int64*)&ainfo.dwLengthL;
	this->currSample = 0;
}

Media::VFAudioStream::~VFAudioStream()
{
	OSInt useCnt;
	this->mfile->mut->Lock();
	useCnt = --this->mfile->useCnt;
	this->mfile->mut->Unlock();
	if (useCnt == 0)
	{
		Text::StrDelNew(this->mfile->fileName);
		VF_PluginFunc *funcs = (VF_PluginFunc*)this->mfile->plugin->funcs;
		funcs->CloseFile(this->mfile->file);
		this->mfile->vfpmgr->Release();
		MemFree(this->mfile);
	}
}

UTF8Char *Media::VFAudioStream::GetSourceName(UTF8Char *buff)
{
	return Text::StrWChar_UTF8(buff, this->mfile->fileName, -1);
}

Bool Media::VFAudioStream::CanSeek()
{
	return true;
}

Int32 Media::VFAudioStream::GetStreamTime()
{
	return (Int32)(this->sampleCnt * 1000 / this->fmt.frequency);
}

Int32 Media::VFAudioStream::SeekToTime(Int32 time)
{
	currSample = MulDiv(time, this->fmt.frequency, 1000);
	return (Int32)(currSample * 1000 / this->fmt.frequency);
}

Bool Media::VFAudioStream::TrimStream(Int32 trimTimeStart, Int32 trimTimeEnd, Int32 *syncTime)
{
	////////////////////////////////////////////
	return false;
}

void Media::VFAudioStream::GetFormat(AudioFormat *format)
{
	MemCopyNO(format, &this->fmt, sizeof(AudioFormat));
}

Bool Media::VFAudioStream::Start(Sync::Event *evt, UOSInt blkSize)
{
	this->readEvt = evt;
	this->currSample = 0;
	return true;
}

void Media::VFAudioStream::Stop()
{
	this->readEvt = 0;
}

UOSInt Media::VFAudioStream::ReadBlock(UInt8 *buff, UOSInt blkSize)
{
	VF_PluginFunc *funcs = (VF_PluginFunc*)mfile->plugin->funcs;
	UOSInt sampleCnt = blkSize / this->fmt.nChannels / (this->fmt.bitpersample >> 3);
	if (sampleCnt + this->currSample > this->sampleCnt)
	{
		sampleCnt = (OSInt)(this->sampleCnt - this->currSample);
	}
	if (sampleCnt <= 0)
	{
		if (this->readEvt)
			this->readEvt->Set();
		return 0;
	}
	UOSInt readSize = sampleCnt * this->fmt.nChannels * (this->fmt.bitpersample >> 3);
	VF_ReadData_Audio rd;
	rd.dwSize = sizeof(rd);
	*(Int64*)&rd.dwSamplePosL = this->currSample;
	rd.dwSampleCount = (DWORD)sampleCnt;
	rd.dwReadedSampleCount = 0;
	rd.dwBufSize = (DWORD)blkSize;
	rd.lpBuf = buff;
	funcs->ReadData(mfile->file, VF_STREAM_AUDIO, &rd);
	this->currSample += rd.dwReadedSampleCount;
	readSize = rd.dwReadedSampleCount * this->fmt.nChannels * (this->fmt.bitpersample >> 3);

	if (this->readEvt)
		this->readEvt->Set();
	return readSize;
}

UOSInt Media::VFAudioStream::GetMinBlockSize()
{
	return this->fmt.nChannels * (this->fmt.bitpersample >> 3);
}

Int32 Media::VFAudioStream::GetCurrTime()
{
	return (Int32)(currSample * 1000 / this->fmt.frequency);
}

Bool Media::VFAudioStream::IsEnd()
{
	return this->currSample >= this->sampleCnt;
}
