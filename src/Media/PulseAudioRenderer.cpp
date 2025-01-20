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
//http://fossies.org/dox/alsa-util-1.0.28/aplay_8c_source.html


void __stdcall Media::PulseAudioRenderer::PlayThread(NN<Sync::Thread> thread)
{
	//Media::PulseAudioRenderer *me = (Media::PulseAudioRenderer *)thread->GetUserObj();
/*	Media::AudioFormat af;
	Int32 i;
	UInt32 refStart;
	UInt32 audStartTime;	
	UOSInt readBuffLeng = BUFFLENG;
	UOSInt outBuffLeng;
	UOSInt outBitPerSample;
	UOSInt outNChannels;
	UOSInt minLeng;
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

		UOSInt buffSize[2];
		UOSInt outSize[2];
		UInt8 *outBuff[2];
		OSInt nextBlock;
		UOSInt readSize = 0;
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
				i = (Int32)(i * (OSInt)(outBitPerSample >> 3) * (OSInt)outNChannels);
				while (i > 0)
				{
					if (buffSize[nextBlock] == 0)
					{
						me->stopPlay = true;
						me->audsrc->Stop();
						break;
					}
					if ((UOSInt)i >= buffSize[nextBlock] - outSize[nextBlock])
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

UInt32 Media::PulseAudioRenderer::GetCurrTime(void *hand)
{
/*	UInt32 ret = 0;
	Int32 err;
	snd_pcm_status_t *status;
	snd_pcm_status_alloca(&status);
	err = snd_pcm_status((snd_pcm_t*)hand, status);
	if (err < 0)
	{
		printf("Error: snd_pcm_status, %d\r\n", err);
	}
	else
	{
		snd_timestamp_t tscurr;
		snd_timestamp_t tsstart;
		snd_pcm_status_get_tstamp(status, &tscurr);
		snd_pcm_status_get_trigger_tstamp(status, &tsstart);
		ret = (UInt32)(tscurr.tv_sec * 1000LL + (tscurr.tv_usec / 1000LL));
	}
	return ret;*/
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

UOSInt Media::PulseAudioRenderer::GetDeviceCount()
{
	DeviceInfo devInfo;
	devInfo.sbuff = 0;
	devInfo.devNo = 0;
	if (GetDeviceInfo(devInfo))
		return devInfo.count;
	return 0;
}

UnsafeArrayOpt<UTF8Char> Media::PulseAudioRenderer::GetDeviceName(UnsafeArray<UTF8Char> buff, UOSInt devNo)
{
	DeviceInfo devInfo;
	devInfo.sbuff = buff;
	devInfo.devNo = devNo;
	if (GetDeviceInfo(devInfo) && devInfo.count > devNo)
		return devInfo.sbuff;
	return 0;
}

void Media::PulseAudioRenderer::OnEvent()
{
	this->thread.Notify();
}

Media::PulseAudioRenderer::PulseAudioRenderer(const UTF8Char *devName) : thread(PlayThread, this, CSTR("PulseAudio"))
{
	this->devName = Text::String::NewOrNullSlow(devName);
	this->audsrc = 0;
	this->resampler = 0;
	this->endHdlr = 0;
	this->buffTime = 500;
	this->hand = 0;
	this->nonBlock = false;
	this->dataConv = false;
	this->dataBits = 0;
	this->dataNChannel = 0;
}

Media::PulseAudioRenderer::~PulseAudioRenderer()
{
	if (this->audsrc)
	{
		BindAudio(0);
	}
	OPTSTR_DEL(this->devName);
}

Bool Media::PulseAudioRenderer::IsError()
{
	return false;
}

Bool Media::PulseAudioRenderer::BindAudio(Media::AudioSource *audsrc)
{
	Media::AudioFormat fmt;
	if (this->thread.IsRunning())
	{
		Stop();
	}
	if (this->audsrc)
	{
		this->audsrc = 0;
		SDEL_CLASS(this->resampler);
		this->hand = 0;
	}
	if (audsrc == 0)
		return false;

	audsrc->GetFormat(fmt);
	if (fmt.formatId != 1 && fmt.formatId != 3)
	{
		return false;
	}

	this->resampleFreq = 0;
	this->dataConv = false;
	this->dataBits = 16;
	this->dataNChannel = 2;
	
	return true;
}

void Media::PulseAudioRenderer::AudioInit(Media::RefClock *clk)
{
	if (this->thread.IsRunning())
		return;
	if (this->audsrc == 0)
		return;
	this->clk = clk;
}

void Media::PulseAudioRenderer::Start()
{
	if (this->thread.IsRunning())
		return;
	if (this->audsrc == 0)
		return;
	this->thread.Start();
}

void Media::PulseAudioRenderer::Stop()
{
	if (!this->thread.IsRunning())
		return;
	this->thread.BeginStop();
	if (this->audsrc)
	{
		this->audsrc->Stop();
	}
	this->thread.WaitForEnd();
}

Bool Media::PulseAudioRenderer::IsPlaying()
{
	return this->thread.IsRunning();
}

void Media::PulseAudioRenderer::SetEndNotify(EndNotifier endHdlr, void *endHdlrObj)
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
