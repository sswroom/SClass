#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Media/AudioDevice.h"
#include "Media/SilentSource.h"
#include "Media/AudioFilter/AudioSweepFilter.h"
#include "Sync/Thread.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	Media::AudioDevice *dev;
	Media::SilentSource *audSrc;
	Media::AudioFilter::AudioSweepFilter *sweepFilter;
	Media::IAudioRenderer *renderer;
	NEW_CLASS(dev, Media::AudioDevice());
	NEW_CLASS(audSrc, Media::SilentSource(48000, 2, 16, (const UTF8Char*)"Silent", -1));
	NEW_CLASS(sweepFilter, Media::AudioFilter::AudioSweepFilter(audSrc));
	sweepFilter->SetVolume(0.3);
	sweepFilter->StartSweep(10, 24000, 10);
	renderer = dev->BindAudio(sweepFilter);
	if (renderer)
	{
		Media::RefClock *clk;
		NEW_CLASS(clk, Media::RefClock());
		renderer->AudioInit(clk);
		renderer->Start();
		if (renderer->IsPlaying())
		{
			Int32 t;
			console.WriteLine((const UTF8Char*)"Start Playing");
			while (true)
			{
				t = clk->GetCurrTime();
				if (t >= 10000)
				{
					break;
				}
				Sync::Thread::Sleep(10000 - t);
			}
			console.WriteLine((const UTF8Char*)"End Playing");
			renderer->Stop();
		}
		renderer->BindAudio(0);
		DEL_CLASS(clk);
	}
	else
	{
		console.WriteLine((const UTF8Char*)"No supported audio device found");
	}
	DEL_CLASS(sweepFilter);
	DEL_CLASS(audSrc);
	DEL_CLASS(dev);
	return 0;
}
