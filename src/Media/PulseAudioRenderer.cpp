#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/WSConfigFile.h"
#include "Media/MediaSource.h"
#include "Media/AudioSource.h"
#include "Media/LPCMConverter.h"
#include "Media/PulseAudioRenderer.h"
#include "Media/RefClock.h"
#include "Media/SOXRFilter.h"
#include "Sync/Event.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include <pulse/pulseaudio.h>
#include <pulse/simple.h>
#include <stdio.h>

#define BUFFLENG 16384
// https://gist.github.com/toroidal-code/8798775

struct Media::PulseAudioRenderer::ClassData
{
	pa_mainloop *mainloop;
	pa_context *context;
	pa_sample_spec sampleSpec;
	pa_stream *stream;
};

static Bool PulseAudioRenderer_WaitContextReady(pa_mainloop *mainloop, pa_context *context)
{
	while (true)
	{
		pa_context_state_t st = pa_context_get_state(context);
		if (st == PA_CONTEXT_READY)
		{
			return true;
		}
		if (!PA_CONTEXT_IS_GOOD(st))
		{
			return false;
		}
		if (pa_mainloop_iterate(mainloop, 1, 0) < 0)
		{
			return false;
		}
	}
}

void PulseAudioRenderer_WriteFunc(pa_stream *s, size_t length, void *userdata) {
	Media::PulseAudioRenderer *me = (Media::PulseAudioRenderer*)userdata;

    me->WriteStream(length);
}

void __stdcall Media::PulseAudioRenderer::PlayThread(NN<Sync::Thread> thread)
{
	NN<Media::PulseAudioRenderer> me = thread->GetUserObj().GetNN<Media::PulseAudioRenderer>();
	Media::AudioFormat af;
	UIntOS readBuffLeng = BUFFLENG;
	UIntOS minLeng;
	Bool needNotify = false;
	NN<Media::AudioSource> audsrc;
	NN<Media::RefClock> clk;

	if (!me->audsrc.SetTo(audsrc))
	{
		return;
	}

	Sync::Event evt;
	audsrc->GetFormat(af);
	if (me->buffTime)
	{
		readBuffLeng = (me->buffTime * af.frequency / 1000) * af.align;
	}
	minLeng = audsrc->GetMinBlockSize();
	if (minLeng > readBuffLeng)
		readBuffLeng = minLeng;

	if (me->clk.SetTo(clk))
	{
		clk->Start(audsrc->GetCurrTime());
	}
	audsrc->Start(&evt, readBuffLeng);

	while (!thread->IsStopping())
	{
		int ret;
		if (pa_mainloop_iterate(me->clsData->mainloop, 1, &ret) < 0)
		{
			break;
		}
		if (me->clsData->stream)
		{
			pa_stream_state_t st = pa_stream_get_state(me->clsData->stream);
			if (st == PA_STREAM_FAILED || st == PA_STREAM_TERMINATED)
			{
				break;
			}
		}
	}

	audsrc->Stop();
	if (audsrc->IsEnd())
	{
		needNotify = true;
	}

	if (needNotify && me->endHdlr)
	{
		me->endHdlr(me->endHdlrObj);
	}

}

UInt32 Media::PulseAudioRenderer::GetCurrTime(void *stream)
{
	pa_usec_t t;
	int err = pa_stream_get_time((pa_stream*)stream, &t);
	if (err == 0)
	{
		return (UInt32)(t / 1000);
	}
	printf("PulseAudioRenderer: pa_stream_get_time failed, return %d\r\n", err);
	return 0;
}

void PulseAudioRenderer_StateCb(pa_context *c, void *userdata)
{
	pa_context_state_t state;
	Media::PulseAudioRenderer::DeviceInfo *devInfo = (Media::PulseAudioRenderer::DeviceInfo*)userdata;

	state = pa_context_get_state(c);
	switch  (state) {
		// There are just here for reference
		case PA_CONTEXT_UNCONNECTED:
		case PA_CONTEXT_CONNECTING:
		case PA_CONTEXT_AUTHORIZING:
		case PA_CONTEXT_SETTING_NAME:
		default:
			break;
		case PA_CONTEXT_FAILED:
		case PA_CONTEXT_TERMINATED:
			devInfo->state = 2;
			break;
		case PA_CONTEXT_READY:
			devInfo->state = 1;
			break;
	}	
}
void PulseAudioRenderer_SinkCb(pa_context *c, const pa_sink_info *i, int eol, void *userdata)
{
	if (eol > 0)
		return;
	Media::PulseAudioRenderer::DeviceInfo *devInfo = (Media::PulseAudioRenderer::DeviceInfo*)userdata;
	UnsafeArray<UTF8Char> nnsbuff;
	if (devInfo->sbuff.SetTo(nnsbuff) && devInfo->devNo == devInfo->count)
	{
		devInfo->sbuff = Text::StrConcat(nnsbuff, (const UTF8Char*)i->name);
	}
	devInfo->count++;
}

Bool Media::PulseAudioRenderer::GetDeviceInfo(NN<DeviceInfo> devInfo)
{
	pa_mainloop *paMl = pa_mainloop_new();
	pa_mainloop_api *paMlAPI = pa_mainloop_get_api(paMl);
	pa_context *paContext = pa_context_new(paMlAPI, "test");
	pa_operation *paOp;
	devInfo->count = 0;
	devInfo->state = 0;
	pa_context_connect(paContext, NULL, PA_CONTEXT_NOFLAGS, NULL);
	pa_context_set_state_callback(paContext, PulseAudioRenderer_StateCb, devInfo.Ptr());
	while (true)
	{
		pa_mainloop_iterate(paMl, 1, NULL);
		if (devInfo->state == 2)
		{
		    pa_context_disconnect(paContext);
		    pa_context_unref(paContext);
		    pa_mainloop_free(paMl);
			return false;
		}
		else if (devInfo->state == 1)
			break;
	}
	paOp = pa_context_get_sink_info_list(paContext, PulseAudioRenderer_SinkCb, devInfo.Ptr());
	if (paOp == 0)
	{
		pa_context_disconnect(paContext);
		pa_context_unref(paContext);
		pa_mainloop_free(paMl);
		return false;
	}
	while (true)
	{
		pa_mainloop_iterate(paMl, 1, NULL);
		if (pa_operation_get_state(paOp) == PA_OPERATION_DONE)
		{
			pa_operation_unref(paOp);
			break;
		}
	}
	pa_context_disconnect(paContext);
	pa_context_unref(paContext);
	pa_mainloop_free(paMl);
	return true;
}

UIntOS Media::PulseAudioRenderer::GetDeviceCount()
{
	DeviceInfo devInfo;
	devInfo.sbuff = nullptr;
	devInfo.devNo = 0;
	if (GetDeviceInfo(devInfo))
		return devInfo.count;
	return 0;
}

UnsafeArrayOpt<UTF8Char> Media::PulseAudioRenderer::GetDeviceName(UnsafeArray<UTF8Char> buff, UIntOS devNo)
{
	DeviceInfo devInfo;
	devInfo.sbuff = buff;
	devInfo.devNo = devNo;
	if (GetDeviceInfo(devInfo) && devInfo.count > devNo)
		return devInfo.sbuff;
	return nullptr;
}

void Media::PulseAudioRenderer::OnEvent()
{
	this->thread.Notify();
}

Media::PulseAudioRenderer::PulseAudioRenderer(UnsafeArrayOpt<const UTF8Char> devName, Text::CStringNN appName) : thread(PlayThread, this, CSTR("PulseAudio"))
{
	this->devName = Text::String::NewOrNullSlow(devName);
	this->audsrc = nullptr;
	this->resampler = nullptr;
	this->endHdlr = 0;
	this->buffTime = 100;
	this->clsData = MemAllocNN(ClassData);
	this->nonBlock = false;
	this->dataConv = false;
	this->dataBits = 0;
	this->dataNChannel = 0;
	this->clsData->mainloop = pa_mainloop_new();
	this->clsData->context = pa_context_new(pa_mainloop_get_api(this->clsData->mainloop), (const Char*)appName.v.Ptr());
	this->clsData->stream = 0;
	pa_context_connect(this->clsData->context, NULL, PA_CONTEXT_NOFLAGS, NULL);
}

Media::PulseAudioRenderer::~PulseAudioRenderer()
{
	if (this->audsrc.NotNull())
	{
		BindAudio(nullptr);
	}
	OPTSTR_DEL(this->devName);
	pa_context_disconnect(this->clsData->context);
	pa_context_unref(this->clsData->context);
	pa_mainloop_free(this->clsData->mainloop);
	MemFreeNN(this->clsData);
}

Bool Media::PulseAudioRenderer::IsError()
{
	return false;
}

Bool Media::PulseAudioRenderer::BindAudio(Optional<Media::AudioSource> audsrc)
{
	printf("PulseAudioRenderer: BindAudio called\r\n");
	Media::AudioFormat fmt;
	if (this->thread.IsRunning())
	{
		Stop();
	}
	if (this->audsrc.NotNull())
	{
		this->audsrc = nullptr;
		this->resampler.Delete();
	}
	if (this->clsData->stream)
	{
		pa_stream_disconnect(this->clsData->stream);
		pa_stream_unref(this->clsData->stream);
		this->clsData->stream = 0;
	}
	NN<Media::AudioSource> nnaudsrc;
	if (!audsrc.SetTo(nnaudsrc))
		return false;

	if (!PulseAudioRenderer_WaitContextReady(this->clsData->mainloop, this->clsData->context))
	{
		printf("PulseAudioRenderer: Context not ready in BindAudio: %s\r\n", pa_strerror(pa_context_errno(this->clsData->context)));
		return false;
	}

	nnaudsrc->GetFormat(fmt);
	if (fmt.formatId == 1)
	{
		if (fmt.bitpersample == 8)
		{
			this->clsData->sampleSpec.format = PA_SAMPLE_U8;
		}
		else if (fmt.bitpersample == 16)
		{
			if (fmt.intType == Media::AudioFormat::IT_BIGENDIAN)
			{
				this->clsData->sampleSpec.format = PA_SAMPLE_S16BE;
			}
			else
			{
				this->clsData->sampleSpec.format = PA_SAMPLE_S16LE;
			}
		}
		else if (fmt.bitpersample == 24)
		{
			if (fmt.intType == Media::AudioFormat::IT_BIGENDIAN)
			{
				this->clsData->sampleSpec.format = PA_SAMPLE_S24BE;
			}
			else
			{
				this->clsData->sampleSpec.format = PA_SAMPLE_S24LE;
			}
		}
		else if (fmt.bitpersample == 32)
		{
			if (fmt.intType == Media::AudioFormat::IT_BIGENDIAN)
			{
				this->clsData->sampleSpec.format = PA_SAMPLE_S32BE;
			}
			else
			{
				this->clsData->sampleSpec.format = PA_SAMPLE_S32LE;
			}
		}
		else
		{
			return false;
		}
	}
	else if (fmt.formatId == 3)
	{
		if (fmt.intType == Media::AudioFormat::IT_BIGENDIAN)
		{
			this->clsData->sampleSpec.format = PA_SAMPLE_FLOAT32BE;
		}
		else
		{
			this->clsData->sampleSpec.format = PA_SAMPLE_FLOAT32LE;
		}
	}
	else
	{
		return false;
	}
	this->clsData->sampleSpec.rate = fmt.frequency;
	this->clsData->sampleSpec.channels = (UInt8)fmt.nChannels;
	if (!pa_sample_spec_valid(&this->clsData->sampleSpec))
	{
		return false;
	}
	this->clsData->stream = pa_stream_new(this->clsData->context, "Stream", &this->clsData->sampleSpec, 0);
	if (this->clsData->stream == 0)
	{
		return false;
	}
	pa_stream_set_write_callback(this->clsData->stream, PulseAudioRenderer_WriteFunc, this);
	this->audsrc = audsrc;
	this->resampler = nullptr;

	this->resampleFreq = 0;
	this->dataConv = false;
	this->dataBits = fmt.bitpersample;
	this->dataNChannel = fmt.nChannels;
	return true;
}

void Media::PulseAudioRenderer::AudioInit(Optional<Media::RefClock> clk)
{
	if (this->thread.IsRunning())
		return;
	if (this->audsrc.IsNull())
		return;
	this->clk = clk;
}

void Media::PulseAudioRenderer::Start()
{
	if (this->thread.IsRunning())
		return;
	if (this->audsrc.IsNull())
		return;
	if (this->clsData->stream == 0)
		return;
	if (!PulseAudioRenderer_WaitContextReady(this->clsData->mainloop, this->clsData->context))
	{
		printf("PulseAudioRenderer: Context not ready in Start: %s\r\n", pa_strerror(pa_context_errno(this->clsData->context)));
		return;
	}

	pa_buffer_attr attr;
	MemClear(&attr, sizeof(attr));
	UIntOS frameSizeOS = pa_frame_size(&this->clsData->sampleSpec);
	UInt32 frameSize;
	if (frameSizeOS > 0xffffffffU)
	{
		frameSize = 0xffffffffU;
	}
	else
	{
		frameSize = (UInt32)frameSizeOS;
	}
	if (frameSize == 0)
	{
		frameSize = 4;
	}
	UInt64 targetUS = ((this->buffTime ? this->buffTime : 100) * 1000ULL);
	attr.maxlength = (UInt32)-1;
	attr.tlength = (UInt32)pa_usec_to_bytes(targetUS, &this->clsData->sampleSpec);
	if (attr.tlength < frameSize * 4)
	{
		attr.tlength = frameSize * 4;
	}
	attr.minreq = attr.tlength >> 2;
	if (attr.minreq < frameSize)
	{
		attr.minreq = frameSize;
	}
	attr.prebuf = 0;
	attr.fragsize = (UInt32)-1;

	pa_stream_flags_t streamFlags = (pa_stream_flags_t)(PA_STREAM_ADJUST_LATENCY | PA_STREAM_INTERPOLATE_TIMING | PA_STREAM_AUTO_TIMING_UPDATE);

	const Char *sinkName = (const Char*)OPTSTR_CSTR(this->devName).v.Ptr();
	if (pa_stream_connect_playback(this->clsData->stream, sinkName, &attr, streamFlags, 0, 0) == 0)
	{
		this->thread.Start();
	}
	else
	{
		if (sinkName != 0)
		{
			printf("PulseAudioRenderer: Playback failed on sink %s: %s, retrying default sink\r\n", sinkName, pa_strerror(pa_context_errno(this->clsData->context)));
			if (pa_stream_connect_playback(this->clsData->stream, 0, &attr, streamFlags, 0, 0) == 0)
			{
				printf("PulseAudioRenderer: Playback started on default sink\r\n");
				this->thread.Start();
				return;
			}
		}
		printf("PulseAudioRenderer: Playback failed: %s\r\n", pa_strerror(pa_context_errno(this->clsData->context)));
	}
}

void Media::PulseAudioRenderer::Stop()
{
	if (!this->thread.IsRunning())
		return;
	this->thread.BeginStop();
	NN<Media::AudioSource> audsrc;
	if (this->audsrc.SetTo(audsrc))
	{
		audsrc->Stop();
	}
	if (this->clsData->stream)
	{
		pa_stream_disconnect(this->clsData->stream);
	}
	this->thread.WaitForEnd();
}

Bool Media::PulseAudioRenderer::IsPlaying()
{
	return this->thread.IsRunning();
}

void Media::PulseAudioRenderer::SetEndNotify(EndNotifier endHdlr, AnyType endHdlrObj)
{
	this->endHdlr = endHdlr;
	this->endHdlrObj = endHdlrObj;
}

Int32 Media::PulseAudioRenderer::GetDeviceVolume()
{
	return 0;
}

void Media::PulseAudioRenderer::SetDeviceVolume(Int32 volume)
{

}

void Media::PulseAudioRenderer::SetBufferTime(UInt32 ms)
{
	this->buffTime = ms;
/*	if (!this->playing && this->hand && this->audsrc)
	{
		this->SetHWParams(this->audsrc, this->hand);
	}*/
}

void Media::PulseAudioRenderer::WriteStream(UIntOS length)
{
	NN<Media::AudioSource> audsrc;
	if (!this->audsrc.SetTo(audsrc) || this->clsData->stream == 0)
	{
		return;
	}

	Media::AudioFormat af;
	audsrc->GetFormat(af);
	UInt8 *outBuff = MemAlloc(UInt8, length);
	UIntOS outSize = 0;

	if (this->dataConv)
	{
		UIntOS readBuffLeng = length;
		if (this->dataBits > 0 && this->dataNChannel > 0)
		{
			readBuffLeng = length * af.bitpersample * af.nChannels / this->dataBits / this->dataNChannel;
			if (readBuffLeng == 0)
			{
				readBuffLeng = af.align;
			}
		}
		if (readBuffLeng < af.align)
		{
			readBuffLeng = af.align;
		}
		else
		{
			readBuffLeng = readBuffLeng / af.align * af.align;
		}

		UInt8 *readBuff = MemAlloc(UInt8, readBuffLeng);
		UIntOS readSize = audsrc->ReadBlockLPCM(Data::ByteArray(readBuff, readBuffLeng), af);
		if (af.bitpersample == this->dataBits && af.formatId == 1)
		{
			outSize = Media::LPCMConverter::ChannelReduce(this->dataBits, af.nChannels, readBuff, readSize, this->dataNChannel, outBuff);
		}
		else
		{
			outSize = Media::LPCMConverter::Convert(af.formatId, af.bitpersample, readBuff, readSize, 1, this->dataBits, outBuff);
			if (af.nChannels != this->dataNChannel)
			{
				outSize = Media::LPCMConverter::ChannelReduce(this->dataBits, af.nChannels, outBuff, outSize, this->dataNChannel, outBuff);
			}
		}
		MemFree(readBuff);
	}
	else
	{
		outSize = audsrc->ReadBlockLPCM(Data::ByteArray(outBuff, length), af);
	}

	if (outSize > length)
	{
		outSize = length;
	}
	if (outSize < length)
	{
		MemClear(&outBuff[outSize], length - outSize);
	}

	if (pa_stream_write(this->clsData->stream, outBuff, length, MemFree, 0, PA_SEEK_RELATIVE) != 0)
	{
		MemFree(outBuff);
	}

	if (outSize == 0)
	{
		audsrc->Stop();
		this->thread.BeginStop();
	}
}
