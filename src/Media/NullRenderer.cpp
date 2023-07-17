#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Media/IMediaSource.h"
#include "Media/IAudioSource.h"
#include "Media/NullRenderer.h"
#include "Media/RefClock.h"
#include "Sync/Event.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

UInt32 __stdcall Media::NullRenderer::PlayThread(void *obj)
{
	Media::NullRenderer *me = (Media::NullRenderer *)obj;
	Media::AudioFormat af;
	UInt32 audStartTime;
	UOSInt buffLeng = 16384;
	UOSInt minLeng;
	UOSInt readSize;
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

	Data::ByteBuffer tmpBuff(buffLeng);

	if (me->clk)
	{
		me->clk->Start(audStartTime);
	}
	me->audsrc->Start(me->playEvt, buffLeng);

	while (!me->stopPlay)
	{
		readSize = me->audsrc->ReadBlockLPCM(tmpBuff, &af);
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
	NEW_CLASS(this->playEvt, Sync::Event());
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
	Sync::ThreadUtil::Create(PlayThread, this);
	while (!threadInit)
	{
		Sync::SimpleThread::Sleep(10);
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
		Sync::SimpleThread::Sleep(10);
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

void Media::NullRenderer::SetBufferTime(UInt32 ms)
{
	this->buffTime = ms;
}

UInt64 Media::NullRenderer::GetSampleCnt()
{
	return this->sampleCnt;
}