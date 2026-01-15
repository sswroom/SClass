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

void PulseAudioRenderer_WriteFunc(pa_stream *s, size_t length, void *userdata) {
	Media::PulseAudioRenderer *me = (Media::PulseAudioRenderer*)userdata;

    me->WriteStream(length);
}

void __stdcall Media::PulseAudioRenderer::PlayThread(NN<Sync::Thread> thread)
{
	NN<Media::PulseAudioRenderer> me = thread->GetUserObj().GetNN<Media::PulseAudioRenderer>();
	int ret;
	pa_mainloop_run(me->clsData->mainloop, &ret);
/*	Media::AudioFormat af;
	Int32 i;
	UInt32 refStart;
	UInt32 audStartTime;	
	UIntOS readBuffLeng = BUFFLENG;
	UIntOS outBuffLeng;
	UIntOS outBitPerSample;
	UIntOS outNChannels;
	UIntOS minLeng;
	UInt32 thisT;
	UInt32 lastT;

	{
		Sync::Event evt;

		me->threadInit = true;
		me->audsrc->GetFormat(&af);
		if (me->buffTime)
		{
			readBuffLeng = (me->buffTime * af.frequency / 1000) * af.align;
		}
		i = 4;
		audStartTime = me->audsrc->GetCurrTime();
		minLeng = me->audsrc->GetMinBlockSize();
		if (minLeng > readBuffLeng)
			readBuffLeng = minLeng;

		me->clk->Start(audStartTime);
		me->playing = true;
		me->audsrc->Start(&evt, readBuffLeng);

		if (me->dataConv)
		{
			outBuffLeng = readBuffLeng * me->dataBits / af.bitpersample;
			outBitPerSample = me->dataBits;
			outNChannels = me->dataNChannel;
		}
		else
		{
			outBuffLeng = readBuffLeng;
			outBitPerSample = af.bitpersample;
			outNChannels = af.nChannels;
		}

		int err;
		err = snd_pcm_reset((snd_pcm_t*)me->hand);
		if (err < 0)
		{
			printf("Error: snd_pcm_reset, %d, %s\r\n", err, snd_strerror(err));
		}
		snd_async_handler_t *ahandler;
		err = snd_async_add_pcm_handler(&ahandler, (snd_pcm_t*)me->hand, ALSARenderer_Event, me);
		if (err < 0)
		{
			printf("Error: snd_async_add_pcm_handler, %d, %s\r\n", err, snd_strerror(err));
		}
		lastT = thisT = GetCurrTime(me->hand);
		refStart = thisT - audStartTime;

		UIntOS buffSize[2];
		UIntOS outSize[2];
		UInt8 *outBuff[2];
		IntOS nextBlock;
		UIntOS readSize = 0;
		UInt8 *readBuff = 0;
		Bool isFirst = true;
		if (me->dataConv)
		{
			readBuff = MemAlloc(UInt8, readBuffLeng);
		}
		nextBlock = 0;
		i = 2;
		while (i-- > 0)
		{
	//		dataExist[i] = false;
			buffSize[i] = 0;
			outSize[i] = 0;
			outBuff[i] = MemAlloc(UInt8, outBuffLeng);
		}
		err = snd_pcm_prepare((snd_pcm_t*)me->hand);
		if (err < 0)
		{
			printf("Error: snd_pcm_prepare, %d %s\r\n", err, snd_strerror(err));
		}
		i = 0;
		while (i < 2)
		{
			if (me->dataConv)
			{
				readSize = me->audsrc->ReadBlockLPCM(Data::ByteArray(readBuff, readBuffLeng), &af);
				if (af.bitpersample == me->dataBits && af.formatId == 1)
				{
					buffSize[i] = Media::LPCMConverter::ChannelReduce(me->dataBits, af.nChannels, readBuff, readSize, me->dataNChannel, outBuff[i]);
				}
				else
				{
					buffSize[i] = Media::LPCMConverter::Convert(af.formatId, af.bitpersample, readBuff, readSize, 1, me->dataBits, outBuff[i]);
					if (af.nChannels != me->dataNChannel)
					{
						buffSize[i] = Media::LPCMConverter::ChannelReduce(me->dataBits, af.nChannels, outBuff[i], buffSize[i], me->dataNChannel, outBuff[i]);
					}
				}
			}
			else
			{
				buffSize[i] = me->audsrc->ReadBlockLPCM(Data::ByteArray(outBuff[i], outBuffLeng), &af);
			}
			outSize[i] = 0;
	//		dataExist[i] = true;
			i++;
		}

		if (me->nonBlock)
		{
			while (!me->stopPlay)
			{
				if (isFirst)
				{
					isFirst = false;
					if (me->buffTime)
					{
						i = (Int32)(me->buffTime * af.frequency / 1000);
					}
					else
					{
						i = (Int32)(af.frequency >> 1);
					}
				}
				else
				{
					i = (Int32)snd_pcm_avail_update((snd_pcm_t*)me->hand);
				}
				if (i < 0)
				{
					printf("Error: snd_pcm_avail_update, %d %s\r\n", i, snd_strerror(i));
					snd_pcm_state_t state = ALSARenderer_GetState(me->hand);
					if (state == SND_PCM_STATE_XRUN)
					{
						err = snd_pcm_prepare((snd_pcm_t*)me->hand);
						if (err < 0)
						{
							printf("Error: snd_pcm_prepare, %d %s\r\n", err, snd_strerror(err));
						}
						
					}
				}
				i = (Int32)(i * (IntOS)(outBitPerSample >> 3) * (IntOS)outNChannels);
				while (i > 0)
				{
					if (buffSize[nextBlock] == 0)
					{
						me->stopPlay = true;
						me->audsrc->Stop();
						break;
					}
					if ((UIntOS)i >= buffSize[nextBlock] - outSize[nextBlock])
					{
						err = (int)snd_pcm_writei((snd_pcm_t *)me->hand, &outBuff[nextBlock][outSize[nextBlock]], (buffSize[nextBlock] - outSize[nextBlock]) / (outBitPerSample >> 3) / outNChannels);
						if (err < 0)
						{
							printf("snd_pcm_writei(%d) return %d\r\n", (Int32)((buffSize[nextBlock] - outSize[nextBlock]) / (outBitPerSample >> 3) / outNChannels), err);
						}
						i -= (Int32)(buffSize[nextBlock] - outSize[nextBlock]);

						if (me->dataConv)
						{
							readSize = me->audsrc->ReadBlockLPCM(Data::ByteArray(readBuff, readBuffLeng), &af);
							if (af.bitpersample == me->dataBits && af.formatId == 1)
							{
								buffSize[nextBlock] = Media::LPCMConverter::ChannelReduce(me->dataBits, af.nChannels, readBuff, readSize, me->dataNChannel, outBuff[nextBlock]);
							}
							else
							{
								buffSize[nextBlock] = Media::LPCMConverter::Convert(af.formatId, af.bitpersample, readBuff, readSize, 1, me->dataBits, outBuff[nextBlock]);
								if (af.nChannels != me->dataNChannel)
								{
									buffSize[nextBlock] = Media::LPCMConverter::ChannelReduce(me->dataBits, af.nChannels, outBuff[nextBlock], buffSize[nextBlock], me->dataNChannel, outBuff[nextBlock]);
								}
							}
						}
						else
						{
							buffSize[nextBlock] = me->audsrc->ReadBlockLPCM(Data::ByteArray(outBuff[nextBlock], outBuffLeng), &af);
						}
						outSize[nextBlock] = 0;
		//				dataExist[nextBlock] = true;

						nextBlock = (nextBlock + 1) & 1;
					}
					else
					{
						snd_pcm_writei((snd_pcm_t *)me->hand, &outBuff[nextBlock][outSize[nextBlock]], (UInt32)i / (outBitPerSample >> 3) / outNChannels);
		//				printf("snd_pcm_writei(%d) return %d\r\n", (Int32)(i / (outBitPerSample >> 3) / outNChannels), (Int32)ret);
						outSize[nextBlock] += (UInt32)i;
						i = 0;
						break;
					}
				}

				thisT = GetCurrTime(me->hand);
				if (thisT != 0)
				{
					if (lastT > thisT)
					{
			//			waveOutReset((HWAVEOUT)me->hwo);
			//			waveOutRestart((HWAVEOUT)me->hwo);
						lastT = thisT = GetCurrTime(me->hand);
						refStart = thisT - me->audsrc->GetCurrTime();
					}
					else
					{
						me->clk->Start(thisT - refStart);
						lastT = thisT;
					}
				}
				else
				{
					lastT = thisT;
				}
				
				me->playEvt->Wait(1000);
			}
		}
		else
		{
			while (!me->stopPlay)
			{
				if (buffSize[nextBlock] == 0)
				{
					me->stopPlay = true;
					me->audsrc->Stop();
					break;
				}
				snd_pcm_writei((snd_pcm_t *)me->hand, &outBuff[nextBlock][outSize[nextBlock]], (buffSize[nextBlock] - outSize[nextBlock]) / (outBitPerSample >> 3) / outNChannels);
	//			printf("snd_pcm_writei(%d) return %d\r\n", (Int32)((buffSize[nextBlock] - outSize[nextBlock]) / (outBitPerSample >> 3) / outNChannels), (Int32)ret);
				i -= (Int32)(buffSize[nextBlock] - outSize[nextBlock]);

				if (me->dataConv)
				{
					readSize = me->audsrc->ReadBlockLPCM(Data::ByteArray(readBuff, readBuffLeng), &af);
					if (af.bitpersample == me->dataBits && af.formatId == 1)
					{
						buffSize[nextBlock] = Media::LPCMConverter::ChannelReduce(me->dataBits, af.nChannels, readBuff, readSize, me->dataNChannel, outBuff[nextBlock]);
					}
					else
					{
						buffSize[nextBlock] = Media::LPCMConverter::Convert(af.formatId, af.bitpersample, readBuff, readSize, 1, me->dataBits, outBuff[nextBlock]);
						if (af.nChannels != me->dataNChannel)
						{
							buffSize[nextBlock] = Media::LPCMConverter::ChannelReduce(me->dataBits, af.nChannels, outBuff[nextBlock], buffSize[nextBlock], me->dataNChannel, outBuff[nextBlock]);
						}
					}
				}
				else
				{
					buffSize[nextBlock] = me->audsrc->ReadBlockLPCM(Data::ByteArray(outBuff[nextBlock], outBuffLeng), &af);
				}
				outSize[nextBlock] = 0;
	//				dataExist[nextBlock] = true;

				nextBlock = (nextBlock + 1) & 1;
			}
		}
		snd_pcm_drop((snd_pcm_t*)me->hand);
		snd_pcm_reset((snd_pcm_t*)me->hand);

		i = 2;
		while (i-- > 0)
		{
			MemFree(outBuff[i]);
		}

		if (readBuff)
		{
			MemFree(readBuff);
			readBuff = 0;
		}
	}
	me->playing = false;*/
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
	this->buffTime = 500;
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
	if (pa_stream_connect_playback(this->clsData->stream, (const Char*)OPTSTR_CSTR(this->devName).v.Ptr(), 0, PA_STREAM_NOFLAGS, 0, 0) == 0)
	{
		this->thread.Start();
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
	//////////////////////////////////////
}
