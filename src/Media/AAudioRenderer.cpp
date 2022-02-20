#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/AAudioRenderer.h"
#include "Media/IMediaSource.h"
#include "Media/IAudioSource.h"
#include "Media/RefClock.h"
#include "Sync/Event.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include <aaudio/AAudio.h>
#include <stdio.h>

#define BUFFLENG 16384
typedef struct
{
	AAudioStreamBuilder *builder;
	AAudioStream *stream;
} ClassData;



/*static void AAudioRenderer_Event(snd_async_handler_t *ahandler)
{
	Media::AAudioRenderer *me = (Media::AAudioRenderer *)0;
	me->OnEvent();
}*/

UInt32 __stdcall Media::AAudioRenderer::PlayThread(void *obj)
{
	Media::AAudioRenderer *me = (Media::AAudioRenderer *)obj;
	Media::AudioFormat af;
	Sync::Event *evt;
	Int32 i;
	Int32 refStart;
	Int32 audStartTime;	
	OSInt buffLeng = BUFFLENG;
	OSInt minLeng;
	Int32 thisT;
	Int32 lastT;

	NEW_CLASS(evt, Sync::Event());

	me->threadInit = true;
	me->audsrc->GetFormat(&af);
	if (me->buffTime)
	{
		buffLeng = (me->buffTime * af.frequency / 1000) * af.align;
	}
	i = 4;
	audStartTime = me->audsrc->GetCurrTime();
	minLeng = me->audsrc->GetMinBlockSize();
	if (minLeng > buffLeng)
		buffLeng = minLeng;

	me->clk->Start(audStartTime);
	me->playing = true;
	me->audsrc->Start(evt, (Int32)buffLeng);

/*	int err;
	err = snd_pcm_reset((snd_pcm_t*)me->hand);
	if (err < 0)
	{
		printf("Error: snd_pcm_reset, %d, %s\r\n", err, snd_strerror(err));
	}
	snd_async_handler_t *ahandler;
	err = snd_async_add_pcm_handler(&ahandler, (snd_pcm_t*)me->hand, ALSARenderer_Event, me);
	if (err < 0)
	{
		printf("Error: snd_pcm_reset, %d, %s\r\n", err, snd_strerror(err));
	}
	lastT = thisT = GetCurrTime(me->hand);
	refStart = thisT - audStartTime;

	OSInt buffSize[2];
	OSInt readSize[2];
	UInt8 *readBuff[2];
//	Bool dataExist[2];
	OSInt nextBlock;
//	OSInt j;
	Bool isFirst = true;
	nextBlock = 0;
	i = 2;
	while (i-- > 0)
	{
//		dataExist[i] = false;
		buffSize[i] = 0;
		readSize[i] = 0;
		readBuff[i] = MemAlloc(UInt8, buffLeng);
	}
	err = snd_pcm_prepare((snd_pcm_t*)me->hand);
	if (err < 0)
	{
		printf("Error: snd_pcm_prepare, %d %s\r\n", err, snd_strerror(err));
	}
	i = 0;
	while (i < 2)
	{
		buffSize[i] = me->audsrc->ReadBlockLPCM(readBuff[i], buffLeng, &af);
		readSize[i] = 0;
//		dataExist[i] = true;
		i++;
	}

	while (!me->stopPlay)
	{
		if (isFirst)
		{
			isFirst = false;
			i = af.frequency >> 1;
		}
		else
		{
			i = snd_pcm_avail_update((snd_pcm_t*)me->hand);
		}
		if (i < 0)
		{
			printf("Error: snd_pcm_avail_update, %d %s\r\n", i, snd_strerror(i));
			snd_pcm_state_t state;
			state = snd_pcm_state((snd_pcm_t*)me->hand);
			switch (state)
			{
			case SND_PCM_STATE_OPEN:
				printf("State: OPEN\r\n");
				break;
			case SND_PCM_STATE_SETUP:
				printf("State: SETUP\r\n");
				break;
			case SND_PCM_STATE_PREPARED:
				printf("State: PREPARED\r\n");
				break;
			case SND_PCM_STATE_RUNNING:
				printf("State: RUNNING\r\n");
				break;
			case SND_PCM_STATE_XRUN:
				printf("State: XRUN\r\n");
				break;
			case SND_PCM_STATE_DRAINING:
				printf("State: DRAINING\r\n");
				break;
			case SND_PCM_STATE_PAUSED:
				printf("State: PAUSED\r\n");
				break;
			case SND_PCM_STATE_SUSPENDED:
				printf("State: SUSPENDED\r\n");
				break;
			case SND_PCM_STATE_DISCONNECTED:
				printf("State: DISCONNECTED\r\n");
				break;
			default:
				printf("State: Unknown\r\n");
				break;
			}
			if (state == SND_PCM_STATE_XRUN)
			{
				err = snd_pcm_prepare((snd_pcm_t*)me->hand);
				if (err < 0)
				{
					printf("Error: snd_pcm_prepare, %d %s\r\n", err, snd_strerror(err));
				}
			}
		}
		i = i * (af.bitpersample >> 3) * af.nChannels;
		while (i > 0)
		{
			if (buffSize[nextBlock] == 0)
			{
				me->stopPlay = true;
				me->audsrc->Stop();
				break;
			}
			if (i >= buffSize[nextBlock] - readSize[nextBlock])
			{
				snd_pcm_writei((snd_pcm_t *)me->hand, &readBuff[nextBlock][readSize[nextBlock]], (buffSize[nextBlock] - readSize[nextBlock]) / (af.bitpersample >> 3) / af.nChannels);
				i -= buffSize[nextBlock] - readSize[nextBlock];

				buffSize[nextBlock] = me->audsrc->ReadBlockLPCM(readBuff[nextBlock], buffLeng, &af);
				readSize[nextBlock] = 0;
//				dataExist[nextBlock] = true;

				nextBlock = (nextBlock + 1) & 1;
			}
			else
			{
				snd_pcm_writei((snd_pcm_t *)me->hand, &readBuff[nextBlock][readSize[nextBlock]], i / (af.bitpersample >> 3) / af.nChannels);
				readSize[nextBlock] += i;
				i = 0;
				break;
			}
		}

		thisT = GetCurrTime(me->hand);
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
		me->playEvt->Wait(1000);
	}
	snd_pcm_drop((snd_pcm_t*)me->hand);
	snd_pcm_reset((snd_pcm_t*)me->hand);

	i = 2;
	while (i-- > 0)
	{
		MemFree(readBuff[i]);
	}*/

	DEL_CLASS(evt);
	me->playing = false;
	return 0;
}

Int32 Media::AAudioRenderer::GetCurrTime(void *hand)
{
	Int32 ret = 0;
	Int32 err;
/*	snd_pcm_status_t *status;
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
		ret = (tscurr.tv_sec * 1000LL + (tscurr.tv_usec / 1000LL));
	}*/
	return ret;
}

Int32 Media::AAudioRenderer::GetDeviceCount()
{
	return 1;
}

UTF8Char *Media::AAudioRenderer::GetDeviceName(UTF8Char *buff, Int32 devNo)
{
	return Text::StrInt32(buff, devNo);
}

void Media::AAudioRenderer::OnEvent()
{
	this->playEvt->Set();
}

Media::AAudioRenderer::AAudioRenderer(const UTF8Char *devName)
{
	if (devName == 0)
	{
		UTF8Char sbuff[16];
		Text::StrInt32(sbuff, AAUDIO_UNSPECIFIED);
		this->devName = Text::StrCopyNew(sbuff);
	}
	else
	{
		this->devName = Text::StrCopyNew(devName);
	}
	this->audsrc = 0;
	this->playing = false;
	this->endHdlr = 0;
	this->buffTime = 0;
	ClassData *clsData = MemAlloc(ClassData, 1);
	clsData->stream = 0;
	clsData->builder = 0;
	aaudio_result_t res = AAudio_createStreamBuilder(&clsData->builder);
	this->hand = clsData;
	if (res != AAUDIO_OK)
	{
		printf("AAudio: createStreamBuilder Error: %d\r\n", res);
	}
}

Media::AAudioRenderer::~AAudioRenderer()
{
	if (this->audsrc)
	{
		BindAudio(0);
	}
	ClassData *clsData = (ClassData*)this->hand;
	if (clsData->builder)
	{
		AAudioStreamBuilder_delete(clsData->builder);
	}
	MemFree(clsData);
}

Bool Media::AAudioRenderer::IsError()
{
	ClassData *clsData = (ClassData*)this->hand;
	return clsData->builder == 0;
}

Bool Media::AAudioRenderer::BindAudio(Media::IAudioSource *audsrc)
{
	Media::AudioFormat fmt;
	if (playing)
	{
		Stop();
	}
	if (this->audsrc)
	{
/*		snd_pcm_close((snd_pcm_t*)this->hand);*/
		this->audsrc = 0;
		this->hand = 0;
		DEL_CLASS(playEvt);
	}
	if (audsrc == 0)
		return false;

	audsrc->GetFormat(&fmt);
	if (fmt.formatId != 1 && fmt.formatId != 3)
	{
		return false;
	}
	
/*	snd_pcm_t *hand;
	snd_pcm_hw_params_t *params;
	snd_pcm_sw_params_t *swparams;
	snd_pcm_format_t sndFmt = SND_PCM_FORMAT_U8;
	int err;
//	Char *cdevName;
	Char cbuff[256];
	
	if (fmt.formatId == 1)
	{
		switch (fmt.bitpersample)
		{
		case 8:
			sndFmt = SND_PCM_FORMAT_U8;
			break;
		case 16:
			sndFmt = SND_PCM_FORMAT_S16_LE;
			break;
		case 24:
			sndFmt = SND_PCM_FORMAT_S24_3LE;
			break;
		case 32:
			sndFmt = SND_PCM_FORMAT_S32_LE;
			break;
		default:
			return false;
		}
	}
	else if (fmt.formatId == 3)
	{
		switch (fmt.bitpersample)
		{
		case 32:
			sndFmt = SND_PCM_FORMAT_FLOAT_LE;
			break;
		default:
			return false;
		}
	}
	

	if (this->devName)
	{
		Text::StrConcat((UTF8Char*)cbuff, this->devName);
//		cdevName = cbuff;
	}
	else
	{
//		cdevName = 0;
	}
	err = snd_pcm_open(&hand, cbuff, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);
	if (err < 0)
	{
		printf("Error: snd_pcm_open, err = %d %s\r\n", err, snd_strerror(err));
		return false;
	}
	err = snd_pcm_hw_params_malloc(&params);
	if (err < 0)
	{
		printf("Error: snd_pcm_hw_params_malloc, err = %d\r\n", err);
		snd_pcm_close(hand);
		return false;
	}
	err = snd_pcm_hw_params_any(hand, params);
	if (err < 0)
	{
		printf("Error: snd_pcm_hw_params_any, err = %d\r\n", err);
		snd_pcm_hw_params_free(params);
		snd_pcm_close(hand);
		return false;
	}
	err = snd_pcm_hw_params_set_access(hand, params, SND_PCM_ACCESS_RW_INTERLEAVED);
	if (err < 0)
	{
		printf("Error: snd_pcm_hw_params_set_access, err = %d\r\n", err);
		snd_pcm_hw_params_free(params);
		snd_pcm_close(hand);
		return false;
	}
	err = snd_pcm_hw_params_set_format(hand, params, sndFmt);
	if (err < 0)
	{
		printf("Error: snd_pcm_hw_params_set_format, err = %d\r\n", err);
		snd_pcm_hw_params_free(params);
		snd_pcm_close(hand);
		return false;
	}
	UInt32 rrate = fmt.frequency;
	err = snd_pcm_hw_params_set_rate_near(hand, params, &rrate, 0);
	if (err < 0)
	{
		printf("Error: snd_pcm_hw_params_set_rate_near, err = %d\r\n", err);
		snd_pcm_hw_params_free(params);
		snd_pcm_close(hand);
		return false;
	}
	printf("Actual sample rate:%d\r\n", rrate);
	err = snd_pcm_hw_params_set_channels(hand, params, fmt.nChannels);
	if (err < 0)
	{
		printf("Error: snd_pcm_hw_params_set_channels (%d), err = %d %s\r\n", fmt.nChannels, err, snd_strerror(err));
		snd_pcm_hw_params_free(params);
		snd_pcm_close(hand);
		return false;
	}
	err = snd_pcm_hw_params(hand, params);
	if (err < 0)
	{
		printf("Error: snd_pcm_hw_params, err = %d\r\n", err);
		snd_pcm_hw_params_free(params);
		snd_pcm_close(hand);
		return false;
	}
//	snd_pcm_hw_params_free(params);

	err = snd_pcm_sw_params_malloc(&swparams);
	if (err < 0)
	{
		printf("Error: snd_pcm_sw_params_malloc, err = %d\r\n", err);
		snd_pcm_close(hand);
		return false;
	}

	err = snd_pcm_sw_params_current(hand, swparams);
	if (err < 0)
	{
		printf("Error: snd_pcm_sw_params_current, err = %d\r\n", err);
		snd_pcm_sw_params_free(swparams);
		snd_pcm_close(hand);
		return false;
	}
	err = snd_pcm_sw_params_set_avail_min(hand, swparams, fmt.frequency >> 1);
	if (err < 0)
	{
		printf("Error: snd_pcm_sw_params_set_avail_min, err = %d\r\n", err);
		snd_pcm_sw_params_free(swparams);
		snd_pcm_close(hand);
		return false;
	}
	err = snd_pcm_sw_params(hand, swparams);
	if (err < 0)
	{
		printf("Error: snd_sw_params_current, err = %d\r\n", err);
		snd_pcm_sw_params_free(swparams);
		snd_pcm_close(hand);
		return false;
	}

	this->hand = hand;
	this->audsrc = audsrc;
	NEW_CLASS(this->playEvt, Sync::Event());*/
	return true;
}

void Media::AAudioRenderer::AudioInit(Media::RefClock *clk)
{
	if (playing)
		return;
	if (this->audsrc == 0)
		return;
	this->clk = clk;
}

void Media::AAudioRenderer::Start()
{
	if (playing)
		return;
	if (this->audsrc == 0)
		return;
	threadInit = false;
	stopPlay = false;
	Sync::Thread::Create(PlayThread, this);
	while (!threadInit)
	{
		Sync::Thread::Sleep(10);
	}
}

void Media::AAudioRenderer::Stop()
{
	stopPlay = true;
	if (!playing)
		return;
	playEvt->Set();
	if (this->audsrc)
	{
		this->audsrc->Stop();
	}
	while (playing)
	{
		Sync::Thread::Sleep(10);
	}
}

Bool Media::AAudioRenderer::IsPlaying()
{
	return this->playing;
}

void Media::AAudioRenderer::SetEndNotify(EndNotifier endHdlr, void *endHdlrObj)
{
	this->endHdlr = endHdlr;
	this->endHdlrObj = endHdlrObj;
}

Int32 Media::AAudioRenderer::GetDeviceVolume()
{
/*	HMIXER hMxr;
	Int32 vol = -1;
	if (mixerOpen(&hMxr, 0, 0, 0, MIXER_OBJECTF_MIXER) != MMSYSERR_NOERROR)
	{
		hMxr = 0;
	}
	else
	{
		MIXERLINE ml;
		MIXERLINECONTROLS mlc;
		MIXERCONTROL mc;
		MIXERCONTROLDETAILS mcd;
		ZeroMemory(&mlc, sizeof(mlc));
		ZeroMemory(&ml, sizeof(ml));
		ml.cbStruct = sizeof(ml);
		ml.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
		if (mixerGetLineInfo((HMIXEROBJ)hMxr, &ml, MIXER_GETLINEINFOF_COMPONENTTYPE) == MMSYSERR_NOERROR)
		{
			mlc.cbStruct = sizeof(mlc);
			mlc.dwLineID = ml.dwLineID;
			mlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
			mlc.cControls = 1;
			mlc.cbmxctrl = sizeof(mc);
		    mlc.pamxctrl = &mc;
			mc.cbStruct = sizeof(mc);
			if (mixerGetLineControls((HMIXEROBJ)hMxr, &mlc, MIXER_GETLINECONTROLSF_ONEBYTYPE) == MMSYSERR_NOERROR)
			{
				mcd.cbStruct = sizeof(mcd);
				mcd.dwControlID = mc.dwControlID;
				mcd.cChannels = 1;
				mcd.hwndOwner = 0;
				mcd.cbDetails = sizeof(vol);
				mcd.paDetails = &vol;
				mixerGetControlDetails((HMIXEROBJ)hMxr, &mcd, MIXER_OBJECTF_HMIXER);
			}
		}
		mixerClose(hMxr);
	}
	return vol;*/
	return 0;
}

void Media::AAudioRenderer::SetDeviceVolume(Int32 volume)
{
/*	HMIXER hMxr;
	if (volume > 65535)
		volume = 65535;
	if (mixerOpen(&hMxr, 0, 0, 0, MIXER_OBJECTF_MIXER) != MMSYSERR_NOERROR)
	{
		hMxr = 0;
	}
	else
	{
		MIXERLINE ml;
		MIXERLINECONTROLS mlc;
		MIXERCONTROL mc;
		MIXERCONTROLDETAILS mcd;
		ZeroMemory(&mlc, sizeof(mlc));
		ZeroMemory(&ml, sizeof(ml));
		ml.cbStruct = sizeof(ml);
		ml.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
		if (mixerGetLineInfo((HMIXEROBJ)hMxr, &ml, MIXER_GETLINEINFOF_COMPONENTTYPE) == MMSYSERR_NOERROR)
		{
			mlc.cbStruct = sizeof(mlc);
			mlc.dwLineID = ml.dwLineID;
			mlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
			mlc.cControls = 1;
			mlc.cbmxctrl = sizeof(mc);
		    mlc.pamxctrl = &mc;
			mc.cbStruct = sizeof(mc);
			if (mixerGetLineControls((HMIXEROBJ)hMxr, &mlc, MIXER_GETLINECONTROLSF_ONEBYTYPE) == MMSYSERR_NOERROR)
			{
				mcd.cbStruct = sizeof(mcd);
				mcd.dwControlID = mc.dwControlID;
				mcd.cChannels = 1;
				mcd.hwndOwner = 0;
				mcd.cbDetails = sizeof(volume);
				mcd.paDetails = &volume;
				mixerSetControlDetails((HMIXEROBJ)hMxr, &mcd, MIXER_OBJECTF_HMIXER);
			}
		}
		mixerClose(hMxr);
	}*/
}

void Media::AAudioRenderer::SetBufferTime(Int32 ms)
{
	this->buffTime = ms;
}
