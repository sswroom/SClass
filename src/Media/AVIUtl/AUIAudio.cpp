#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/AVIUtl/AUIAudio.h"
#include "Sync/Interlocked.h"
#include <windows.h>

Media::AVIUtl::AUIAudio::AUIAudio(Media::AVIUtl::AUIPlugin *plugin, Media::AVIUtl::AUIPlugin::AUIInput *input, NotNullPtr<Media::AudioFormat> format, UOSInt nSamples)
{
	this->plugin = plugin;
	this->input = input;
	this->format = format;
	this->nSamples = nSamples;
	this->currSample = 0;
}

Media::AVIUtl::AUIAudio::~AUIAudio()
{
	if (Sync::Interlocked::DecrementU32(this->input->useCnt) == 0)
	{
		this->plugin->CloseInput(this->input->hand);
		MemFree(this->input);
	}
	DEL_CLASS(this->plugin);
	this->format.Delete();
}

UTF8Char *Media::AVIUtl::AUIAudio::GetSourceName(UTF8Char *buff)
{
	return 0;//this->plugin->GetName(buff);
}

Bool Media::AVIUtl::AUIAudio::CanSeek()
{
	return true;
}

Int32 Media::AVIUtl::AUIAudio::GetStreamTime()
{
	return (Int32)MulDivU32((UInt32)this->nSamples, 1000, this->format->frequency);
}

UInt32 Media::AVIUtl::AUIAudio::SeekToTime(UInt32 time)
{
	this->currSample = MulDivU32(time, this->format->frequency, 1000);
	if (this->currSample > this->nSamples)
		this->currSample = this->nSamples;
	return MulDivU32((UInt32)this->currSample, 1000, this->format->frequency);
}

Bool Media::AVIUtl::AUIAudio::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime)
{
	//////////////////////////////////////
	return false;
}

void Media::AVIUtl::AUIAudio::GetFormat(NotNullPtr<AudioFormat> format)
{
	format->FromAudioFormat(this->format);
}

Bool Media::AVIUtl::AUIAudio::Start(Sync::Event *evt, UOSInt blkSize)
{
	this->playEvt = evt;
	if (this->playEvt)
		this->playEvt->Set();
	return true;
}

void Media::AVIUtl::AUIAudio::Stop()
{
	this->playEvt = 0;
}

UOSInt Media::AVIUtl::AUIAudio::ReadBlock(Data::ByteArray blk)
{
	UOSInt nSample = blk.GetSize() / this->format->align;
	UOSInt readCnt = this->plugin->GetAudioData(this->input->hand, this->currSample, nSample, blk.Ptr());
	this->currSample += readCnt;
	if (this->playEvt)
		this->playEvt->Set();
	return readCnt * this->format->align;
}

UOSInt Media::AVIUtl::AUIAudio::GetMinBlockSize()
{
	return this->format->align;
}

UInt32 Media::AVIUtl::AUIAudio::GetCurrTime()
{
	return MulDivU32((UInt32)this->currSample, 1000, this->format->frequency);
}

Bool Media::AVIUtl::AUIAudio::IsEnd()
{
	return this->currSample >= this->nSamples;
}
