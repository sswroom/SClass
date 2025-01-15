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

UInt32 __stdcall Media::NullRenderer::PlayThread(AnyType obj)
{
	NN<Media::NullRenderer> me = obj.GetNN<Media::NullRenderer>();
	Media::AudioFormat af;
	Data::Duration audStartTime;
	UOSInt buffLeng = 16384;
	UOSInt minLeng;
	UOSInt readSize;
	Bool needNotify = false;
	NN<Media::IAudioSource> audsrc;
	NN<Media::RefClock> clk;

	me->playing = true;
	me->threadInit = true;
	if (me->audsrc.SetTo(audsrc))
	{
		audsrc->GetFormat(af);
		if (me->buffTime)
		{
			buffLeng = (me->buffTime * af.frequency / 1000) * af.align;
		}
		audStartTime = audsrc->GetCurrTime();
		minLeng = audsrc->GetMinBlockSize();
		if (minLeng > buffLeng)
			buffLeng = minLeng;

		Data::ByteBuffer tmpBuff(buffLeng);

		if (me->clk.SetTo(clk))
		{
			clk->Start(audStartTime);
		}
		audsrc->Start(me->playEvt, buffLeng);

		while (!me->stopPlay)
		{
			readSize = audsrc->ReadBlockLPCM(tmpBuff, af);
			if (readSize == 0)
			{
				if (audsrc->IsEnd())
				{
					needNotify = true;
					break;
				}
				me->playEvt.Wait(1000);
			}
			else
			{
				me->sampleCnt += readSize / af.align;
			}
		}
	}

	me->playing = false;

	if (needNotify)
	{
		if (me->endHdlr.func)
		{
			me->endHdlr.func(me->endHdlr.userObj);
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
	if (this->audsrc.NotNull())
	{
		BindAudio(0);
	}
}

Bool Media::NullRenderer::IsError()
{
	return false;
}

Bool Media::NullRenderer::BindAudio(Optional<Media::IAudioSource> audsrc)
{
	Media::AudioFormat fmt;
	if (playing)
	{
		Stop();
	}
	this->audsrc = 0;
	NN<Media::IAudioSource> nnaudsrc;
	if (!audsrc.SetTo(nnaudsrc))
		return false;

	nnaudsrc->GetFormat(fmt);
	if (fmt.formatId != 1 && fmt.formatId != 3)
	{
		return false;
	}

	this->audsrc = nnaudsrc;
	return true;
}

void Media::NullRenderer::AudioInit(Optional<Media::RefClock> clk)
{
	if (playing)
		return;
	if (this->audsrc.IsNull())
		return;
	this->clk = clk;
}

void Media::NullRenderer::Start()
{
	if (playing)
		return;
	if (this->audsrc.IsNull())
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
	this->playEvt.Set();
	NN<Media::IAudioSource> audsrc;
	if (this->audsrc.SetTo(audsrc))
	{
		audsrc->Stop();
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

void Media::NullRenderer::SetEndNotify(EndNotifier endHdlr, AnyType endHdlrObj)
{
	this->endHdlr = {endHdlr, endHdlrObj};
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