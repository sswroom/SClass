#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Event.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Media/IMediaSource.h"
#include "Media/IAudioSource.h"
#include "Media/NullRenderer.h"
#include "Media/RefClock.h"

UInt32 __stdcall Media::NullRenderer::PlayThread(void *obj)
{
	Media::NullRenderer *me = (Media::NullRenderer *)obj;
	Media::AudioFormat af;
	UInt8 *tmpBuff;
	Int32 audStartTime;
	OSInt buffLeng = 16384;
	OSInt minLeng;
	OSInt readSize;
	Bool needNotify = false;

	me->playing = true;
	me->threadInit = true;
	me->audsrc->GetFormat(&af);
	if (me->buffTime)
	{
		buffLeng = (me->buffTime * af.frequency / 1000) * af.align;
	}
	audStartTime = me->audsrc->GetCurrTime();
	minLeng = me->audsrc->GetMinBlockSize();
	if (minLeng > buffLeng)
		buffLeng = minLeng;

	tmpBuff = MemAlloc(UInt8, buffLeng);

	if (me->clk)
	{
		me->clk->Start(audStartTime);
	}
	me->audsrc->Start(me->playEvt, (Int32)buffLeng);

	while (!me->stopPlay)
	{
		readSize = me->audsrc->ReadBlockLPCM(tmpBuff, buffLeng, &af);
		if (readSize == 0)
		{
			if (me->audsrc->IsEnd())
			{
				needNotify = true;
				break;
			}
			me->playEvt->Wait(1000);
		}
		else
		{
			me->sampleCnt += readSize / af.align;
		}
	}
	MemFree(tmpBuff);

	me->playing = false;

	if (needNotify)
	{
		if (me->endHdlr)
		{
			me->endHdlr(me->endHdlrObj);
		}
	}
	return 0;
}

Media::NullRenderer::NullRenderer()
{
	this->audsrc = 0;
	this->playing = false;
	this->endHdlr = 0;
	this->buffTime = 0;
	this->clk = 0;
	this->sampleCnt = 0;
}

Media::NullRenderer::~NullRenderer()
{
	if (this->audsrc)
	{
		BindAudio(0);
	}
}

Bool Media::NullRenderer::IsError()
{
	return false;
}

Bool Media::NullRenderer::BindAudio(Media::IAudioSource *audsrc)
{
	Media::AudioFormat fmt;
	if (playing)
	{
		Stop();
	}
	if (this->audsrc)
	{
		this->audsrc = 0;
		DEL_CLASS(playEvt);
	}
	if (audsrc == 0)
		return false;

	audsrc->GetFormat(&fmt);
	if (fmt.formatId != 1 && fmt.formatId != 3)
	{
		return false;
	}

	this->audsrc = audsrc;
	NEW_CLASS(this->playEvt, Sync::Event((const UTF8Char*)"Media.WaveOutRenderer.playEvt"));
	return true;
}

void Media::NullRenderer::AudioInit(Media::RefClock *clk)
{
	if (playing)
		return;
	if (this->audsrc == 0)
		return;
	this->clk = clk;
}

void Media::NullRenderer::Start()
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

void Media::NullRenderer::Stop()
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

Bool Media::NullRenderer::IsPlaying()
{
	return this->playing;
}

void Media::NullRenderer::SetEndNotify(EndNotifier endHdlr, void *endHdlrObj)
{
	this->endHdlr = endHdlr;
	this->endHdlrObj = endHdlrObj;
}

Int32 Media::NullRenderer::GetDeviceVolume()
{
	return 65535;
}

void Media::NullRenderer::SetDeviceVolume(Int32 volume)
{
}

void Media::NullRenderer::SetBufferTime(Int32 ms)
{
	this->buffTime = ms;
}

Int64 Media::NullRenderer::GetSampleCnt()
{
	return this->sampleCnt;
}