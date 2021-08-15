#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/WSConfigFile.h"
#include "Media/ALSARenderer.h"
#include "Media/IMediaSource.h"
#include "Media/IAudioSource.h"
#include "Media/LPCMConverter.h"
#include "Media/RefClock.h"
#include "Sync/Event.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include <alsa/asoundlib.h>
#include <stdio.h>

#define BUFFLENG 16384
//http://fossies.org/dox/alsa-util-1.0.28/aplay_8c_source.html

static void ALSARenderer_Event(snd_async_handler_t *ahandler)
{
	Media::ALSARenderer *me = (Media::ALSARenderer *)snd_async_handler_get_callback_private(ahandler);
	me->OnEvent();
}

snd_pcm_state_t ALSARenderer_GetState(void *hand)
{
	snd_pcm_state_t state;
	state = snd_pcm_state((snd_pcm_t*)hand);
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
	case SND_PCM_STATE_PRIVATE1:
		printf("State: PRIVATE1\r\n");
		break;
	default:
		printf("State: Unknown\r\n");
		break;
	}
	return state;
}

UInt32 __stdcall Media::ALSARenderer::PlayThread(void *obj)
{
	Media::ALSARenderer *me = (Media::ALSARenderer *)obj;
	Media::AudioFormat af;
	Sync::Event *evt;
	Int32 i;
	UInt32 refStart;
	UInt32 audStartTime;	
	UOSInt readBuffLeng = BUFFLENG;
	UOSInt outBuffLeng;
	UOSInt outBitPerSample;
	UOSInt minLeng;
	UInt32 thisT;
	UInt32 lastT;

	NEW_CLASS(evt, Sync::Event((const UTF8Char*)"Media.ALSARenderer.PlayThread.evt"));

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
	me->audsrc->Start(evt, readBuffLeng);

	if (me->dataConv)
	{
		outBuffLeng = readBuffLeng * 16 / af.bitpersample;
		outBitPerSample = 16;
	}
	else
	{
		outBuffLeng = readBuffLeng;
		outBitPerSample = af.bitpersample;
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
		printf("Error: snd_pcm_reset, %d, %s\r\n", err, snd_strerror(err));
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
			readSize = me->audsrc->ReadBlockLPCM(readBuff, readBuffLeng, &af);
			buffSize[i] = Media::LPCMConverter::Convert(af.formatId, af.bitpersample, readBuff, readSize, 1, 16, outBuff[i]);
		}
		else
		{
			buffSize[i] = me->audsrc->ReadBlockLPCM(outBuff[i], outBuffLeng, &af);
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
			i = (Int32)(i * (OSInt)(outBitPerSample >> 3) * af.nChannels);
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
					snd_pcm_writei((snd_pcm_t *)me->hand, &outBuff[nextBlock][outSize[nextBlock]], (buffSize[nextBlock] - outSize[nextBlock]) / (outBitPerSample >> 3) / af.nChannels);
	//				printf("snd_pcm_writei(%d) return %d\r\n", (Int32)((buffSize[nextBlock] - outSize[nextBlock]) / (outBitPerSample >> 3) / af.nChannels), (Int32)ret);
					i -= (Int32)(buffSize[nextBlock] - outSize[nextBlock]);

					if (me->dataConv)
					{
						readSize = me->audsrc->ReadBlockLPCM(readBuff, readBuffLeng, &af);
						buffSize[nextBlock] = Media::LPCMConverter::Convert(af.formatId, af.bitpersample, readBuff, readSize, 1, 16, outBuff[nextBlock]);
					}
					else
					{
						buffSize[nextBlock] = me->audsrc->ReadBlockLPCM(outBuff[nextBlock], outBuffLeng, &af);
					}
					outSize[nextBlock] = 0;
	//				dataExist[nextBlock] = true;

					nextBlock = (nextBlock + 1) & 1;
				}
				else
				{
					snd_pcm_writei((snd_pcm_t *)me->hand, &outBuff[nextBlock][outSize[nextBlock]], (UInt32)i / (outBitPerSample >> 3) / af.nChannels);
	//				printf("snd_pcm_writei(%d) return %d\r\n", (Int32)(i / (outBitPerSample >> 3) / af.nChannels), (Int32)ret);
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
			snd_pcm_writei((snd_pcm_t *)me->hand, &outBuff[nextBlock][outSize[nextBlock]], (buffSize[nextBlock] - outSize[nextBlock]) / (outBitPerSample >> 3) / af.nChannels);
//			printf("snd_pcm_writei(%d) return %d\r\n", (Int32)((buffSize[nextBlock] - outSize[nextBlock]) / (outBitPerSample >> 3) / af.nChannels), (Int32)ret);
			i -= (Int32)(buffSize[nextBlock] - outSize[nextBlock]);

			if (me->dataConv)
			{
				readSize = me->audsrc->ReadBlockLPCM(readBuff, readBuffLeng, &af);
				buffSize[nextBlock] = Media::LPCMConverter::Convert(af.formatId, af.bitpersample, readBuff, readSize, 1, 16, outBuff[nextBlock]);
			}
			else
			{
				buffSize[nextBlock] = me->audsrc->ReadBlockLPCM(outBuff[nextBlock], outBuffLeng, &af);
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

	DEL_CLASS(evt);
	me->playing = false;
	return 0;
}

UInt32 Media::ALSARenderer::GetCurrTime(void *hand)
{
	UInt32 ret = 0;
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
	return ret;
}

Bool Media::ALSARenderer::SetHWParams(Media::IAudioSource *audsrc, void *h)
{
	snd_pcm_hw_params_t *params;
	snd_pcm_t *hand = (snd_pcm_t*)h;
	snd_pcm_format_t sndFmt = SND_PCM_FORMAT_U8;
	Media::AudioFormat fmt;

	audsrc->GetFormat(&fmt);
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
	
	int err;
	err = snd_pcm_hw_params_malloc(&params);
	if (err < 0)
	{
		printf("Error: snd_pcm_hw_params_malloc, err = %d\r\n", err);
		return false;
	}
	err = snd_pcm_hw_params_any(hand, params);
	if (err < 0)
	{
		printf("Error: snd_pcm_hw_params_any, err = %d\r\n", err);
		snd_pcm_hw_params_free(params);
		return false;
	}
	err = snd_pcm_hw_params_set_access(hand, params, SND_PCM_ACCESS_RW_INTERLEAVED);
	if (err < 0)
	{
		printf("Error: snd_pcm_hw_params_set_access, err = %d\r\n", err);
		snd_pcm_hw_params_free(params);
		return false;
	}
	err = snd_pcm_hw_params_set_format(hand, params, sndFmt);
	if (err < 0)
	{
		err = snd_pcm_hw_params_set_format(hand, params, SND_PCM_FORMAT_S16_LE);
		if (err < 0)
		{
			printf("Error: snd_pcm_hw_params_set_format(%d), err = %d\r\n", sndFmt, err);
			snd_pcm_hw_params_free(params);
			return false;
		}
		this->dataConv = true;
	}
	UInt32 rrate = fmt.frequency;
	err = snd_pcm_hw_params_set_rate_near(hand, params, &rrate, 0);
	if (err < 0)
	{
		printf("Error: snd_pcm_hw_params_set_rate_near, err = %d\r\n", err);
		snd_pcm_hw_params_free(params);
		return false;
	}
//	printf("ALSA: Actual sample rate:%d\r\n", rrate);
	err = snd_pcm_hw_params_set_channels(hand, params, fmt.nChannels);
	if (err < 0)
	{
		printf("Error: snd_pcm_hw_params_set_channels (%d), err = %d %s\r\n", fmt.nChannels, err, snd_strerror(err));
		snd_pcm_hw_params_free(params);
		return false;
	}
	if (this->buffTime)
	{
		snd_pcm_uframes_t usize = (this->buffTime * fmt.frequency / 1000);
		err = snd_pcm_hw_params_set_buffer_size_near(hand, params, &usize);
		if (err < 0)
		{
			printf("Error: snd_pcm_hw_params_set_buffer_size_near, err = %d\r\n", err);
		}
		usize = usize >> 1;
		err = snd_pcm_hw_params_set_period_size_near(hand, params, &usize, 0);
		if (err < 0)
		{
			printf("Error: snd_pcm_hw_params_set_period_size_near, err = %d\r\n", err);
		}
	}

	err = snd_pcm_hw_params(hand, params);
	if (err < 0)
	{
		printf("Error: snd_pcm_hw_params, err = %d\r\n", err);
		snd_pcm_hw_params_free(params);
		return false;
	}
//	snd_pcm_hw_params_free(params);
	return true;
}

UOSInt Media::ALSARenderer::GetDeviceCount()
{
	Int32 card = -1;
	UOSInt count = 0;
	card = -1;
	while (snd_card_next(&card) >= 0 && card >= 0)
	{
		count++;
	}
	return count;
}

UTF8Char *Media::ALSARenderer::GetDeviceName(UTF8Char *buff, UOSInt devNo)
{
	snd_ctl_t *handle;
	snd_ctl_card_info_t *info;
	Int32 card = -1;
	char name[32];
	const char *cardName;
	card = -1;
	snd_ctl_card_info_alloca(&info);
	while (snd_card_next(&card) >= 0 && card >= 0)
	{
		if (devNo <= 0)
		{
			sprintf(name, "hw:%d", card);
			if (snd_ctl_open(&handle, name, 0) < 0)
			{
				return 0;
			}
			if (snd_ctl_card_info(handle, info) < 0)
			{
				snd_ctl_close(handle);
				return 0;
			}
			cardName = snd_ctl_card_info_get_name(info);
			if (cardName)
			{
				buff = Text::StrConcat(buff, (const UTF8Char*)cardName);
			}
			else
			{
				buff = 0;
			}
			
			snd_ctl_close(handle);
			return buff;
		}
		else
		{
			devNo--;
		}
	}
	return 0;
}

void Media::ALSARenderer::OnEvent()
{
	this->playEvt->Set();
}

Media::ALSARenderer::ALSARenderer(const UTF8Char *devName)
{
	if (devName == 0)
	{
		this->devName = 0;
		IO::ConfigFile *cfg = IO::WSConfigFile::Parse((const UTF8Char*)"/etc/asound.conf");
		if (cfg)
		{
			const UTF8Char *csptr = cfg->GetValue((const UTF8Char*)"defaults.pcm.card");
			if (csptr)
			{
				UTF8Char sbuff[32];
				Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"hw:"), csptr);
				this->devName = Text::StrCopyNew(sbuff);
			}
			DEL_CLASS(cfg);
		}
		if (this->devName == 0)
		{
			this->devName = Text::StrCopyNew((const UTF8Char*)"hw:0");
		}
	}
	else
	{
		this->devName = Text::StrCopyNew(devName);
	}
	this->audsrc = 0;
	this->playing = false;
	this->endHdlr = 0;
	this->buffTime = 500;
	this->hand = 0;
	this->nonBlock = false;
}

Media::ALSARenderer::~ALSARenderer()
{
	if (this->audsrc)
	{
		BindAudio(0);
	}
	SDEL_TEXT(this->devName);
}

Bool Media::ALSARenderer::IsError()
{
	return false;
}

Bool Media::ALSARenderer::BindAudio(Media::IAudioSource *audsrc)
{
	Media::AudioFormat fmt;
	if (playing)
	{
		Stop();
	}
	if (this->audsrc)
	{
		snd_pcm_close((snd_pcm_t*)this->hand);
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

	this->dataConv = false;
	
	snd_pcm_t *hand;
	snd_pcm_sw_params_t *swparams;
	int err;
//	Char *cdevName;
	Char cbuff[256];


	if (this->devName)
	{
		if (Text::StrStartsWith(this->devName, (const UTF8Char*)"hw:"))
		{
			Text::StrConcat((UTF8Char*)cbuff, this->devName);
		}
		else
		{
			Bool found = false;
			snd_ctl_t *handle;
			snd_ctl_card_info_t *info;
			Int32 card = -1;
			const char *cardName;
			card = -1;
			snd_ctl_card_info_alloca(&info);
			while (snd_card_next(&card) >= 0 && card >= 0)
			{
				sprintf(cbuff, "hw:%d", card);
				if (snd_ctl_open(&handle, cbuff, 0) < 0)
				{
					
				}
				else if (snd_ctl_card_info(handle, info) < 0)
				{
					snd_ctl_close(handle);
				}
				else
				{
					cardName = snd_ctl_card_info_get_name(info);
					if (cardName)
					{
						if (Text::StrEquals((const UTF8Char*)cardName, this->devName))
						{
							found = true;
						}
					}
					snd_ctl_close(handle);

				}
				if (found)
				{
					break;
				}
			}
		}
	}
	int mode = 0;
	if (this->nonBlock)
	{
		mode = SND_PCM_NONBLOCK;
	}
	err = snd_pcm_open(&hand, cbuff, SND_PCM_STREAM_PLAYBACK, mode);
	if (err < 0)
	{
		printf("Error: snd_pcm_open(%s), err = %d %s, try default\r\n", cbuff, err, snd_strerror(err));
		err = snd_pcm_open(&hand, "default", SND_PCM_STREAM_PLAYBACK, mode);
		if (err < 0)
		{
			printf("Error: snd_pcm_open(default), err = %d %s\r\n", err, snd_strerror(err));
			return false;
		}
	}
	if (!this->SetHWParams(audsrc, hand))
	{
		snd_pcm_close(hand);
		return false;
	}

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
	err = snd_pcm_sw_params_set_avail_min(hand, swparams, (this->buffTime * fmt.frequency / 2000));
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
	NEW_CLASS(this->playEvt, Sync::Event((const UTF8Char*)"Media.ALSARenderer.playEvt"));
	return true;
}

void Media::ALSARenderer::AudioInit(Media::RefClock *clk)
{
	if (playing)
		return;
	if (this->audsrc == 0)
		return;
	this->clk = clk;
}

void Media::ALSARenderer::Start()
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

void Media::ALSARenderer::Stop()
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

Bool Media::ALSARenderer::IsPlaying()
{
	return this->playing;
}

void Media::ALSARenderer::SetEndNotify(EndNotifier endHdlr, void *endHdlrObj)
{
	this->endHdlr = endHdlr;
	this->endHdlrObj = endHdlrObj;
}

Int32 Media::ALSARenderer::GetDeviceVolume()
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

void Media::ALSARenderer::SetDeviceVolume(Int32 volume)
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

void Media::ALSARenderer::SetBufferTime(UInt32 ms)
{
	this->buffTime = ms;
	if (!this->playing && this->hand && this->audsrc)
	{
		this->SetHWParams(this->audsrc, this->hand);
	}
}
