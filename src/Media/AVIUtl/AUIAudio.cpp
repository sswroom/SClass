#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/AVIUtl/AUIAudio.h"
#include "Sync/Interlocked.h"
#include <windows.h>

Media::AVIUtl::AUIAudio::AUIAudio(NN<Media::AVIUtl::AUIPlugin> plugin, NN<Media::AVIUtl::AUIPlugin::AUIInput> input, NN<Media::AudioFormat> format, UIntOS nSamples)
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
		MemFreeNN(this->input);
	}
	this->plugin.Delete();
	this->format.Delete();
}

UnsafeArrayOpt<UTF8Char> Media::AVIUtl::AUIAudio::GetSourceName(UnsafeArray<UTF8Char> buff)
{
	return 0;//this->plugin->GetName(buff);
}

Bool Media::AVIUtl::AUIAudio::CanSeek()
{
	return true;
}

Data::Duration Media::AVIUtl::AUIAudio::GetStreamTime()
{
	return Data::Duration::FromRatioU64(this->nSamples, this->format->frequency);
}

Data::Duration Media::AVIUtl::AUIAudio::SeekToTime(Data::Duration time)
{
	this->currSample = (UIntOS)time.MultiplyU64(this->format->frequency);
	if (this->currSample > this->nSamples)
		this->currSample = this->nSamples;
	return Data::Duration::FromRatioU64(this->currSample, this->format->frequency);
}

Bool Media::AVIUtl::AUIAudio::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime)
{
	//////////////////////////////////////
	return false;
}

void Media::AVIUtl::AUIAudio::GetFormat(NN<AudioFormat> format)
{
	format->FromAudioFormat(this->format);
}

Bool Media::AVIUtl::AUIAudio::Start(Optional<Sync::Event> evt, UIntOS blkSize)
{
	NN<Sync::Event> playEvt;
	this->playEvt = evt;
	if (this->playEvt.SetTo(playEvt))
		playEvt->Set();
	return true;
}

void Media::AVIUtl::AUIAudio::Stop()
{
	this->playEvt = 0;
}

UIntOS Media::AVIUtl::AUIAudio::ReadBlock(Data::ByteArray blk)
{
	NN<Sync::Event> playEvt;
	UIntOS nSample = blk.GetSize() / this->format->align;
	UIntOS readCnt = this->plugin->GetAudioData(this->input->hand, this->currSample, nSample, blk.Arr().Ptr());
	this->currSample += readCnt;
	if (this->playEvt.SetTo(playEvt))
		playEvt->Set();
	return readCnt * this->format->align;
}

UIntOS Media::AVIUtl::AUIAudio::GetMinBlockSize()
{
	return this->format->align;
}

Data::Duration Media::AVIUtl::AUIAudio::GetCurrTime()
{
	return Data::Duration::FromRatioU64(this->currSample, this->format->frequency);
}

Bool Media::AVIUtl::AUIAudio::IsEnd()
{
	return this->currSample >= this->nSamples;
}
