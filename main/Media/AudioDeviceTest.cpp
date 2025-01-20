#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Media/AudioDevice.h"
#include "Media/SilentSource.h"
#include "Media/AFilter/AudioSweepFilter.h"
#include "Sync/SimpleThread.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	Media::AudioDevice *dev;
	NN<Media::SilentSource> audSrc;
	Media::AFilter::AudioSweepFilter *sweepFilter;
	NN<Media::AudioRenderer> renderer;
	NEW_CLASS(dev, Media::AudioDevice());
	NEW_CLASSNN(audSrc, Media::SilentSource(48000, 2, 16, CSTR("Silent"), 0));
	NEW_CLASS(sweepFilter, Media::AFilter::AudioSweepFilter(audSrc));
	sweepFilter->SetVolume(0.3);
	sweepFilter->StartSweep(10, 24000, 10);
	if (dev->BindAudio(sweepFilter).SetTo(renderer))
	{
		Media::RefClock *clk;
		NEW_CLASS(clk, Media::RefClock());
		renderer->AudioInit(clk);
		renderer->Start();
		if (renderer->IsPlaying())
		{
			Data::Duration t;
			console.WriteLine(CSTR("Start Playing"));
			while (true)
			{
				t = clk->GetCurrTime();
				if (t.GetTotalMS() >= 10000)
				{
					break;
				}
				Sync::SimpleThread::Sleep(10000 - (UOSInt)t.GetTotalMS());
			}
			console.WriteLine(CSTR("End Playing"));
			renderer->Stop();
		}
		renderer->BindAudio(0);
		DEL_CLASS(clk);
	}
	else
	{
		console.WriteLine(CSTR("No supported audio device found"));
	}
	DEL_CLASS(sweepFilter);
	audSrc.Delete();
	DEL_CLASS(dev);
	return 0;
}
