#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ALSARenderer.h"
#include "Media/AudioDevice.h"
#include "Media/PulseAudioRenderer.h"
#include "Text/MyString.h"

UOSInt Media::AudioDevice::GetDeviceCount()
{
	return Media::PulseAudioRenderer::GetDeviceCount() + Media::ALSARenderer::GetDeviceCount();
}

UnsafeArrayOpt<UTF8Char> Media::AudioDevice::GetDeviceName(UnsafeArray<UTF8Char> buff, UOSInt devNo)
{
	UOSInt paCount = Media::PulseAudioRenderer::GetDeviceCount();
	if (devNo >= paCount)
	{
		return Media::ALSARenderer::GetDeviceName(Text::StrConcatC(buff, UTF8STRC("ALSA: ")), devNo - paCount);
	}
	else
	{
		return Media::ALSARenderer::GetDeviceName(Text::StrConcatC(buff, UTF8STRC("PA: ")), devNo);
	}
}

Optional<Media::IAudioRenderer> Media::AudioDevice::CreateRenderer(Text::CStringNN devName)
{
	Media::IAudioRenderer *renderer = 0;
	if (devName.StartsWith(UTF8STRC("ALSA: ")))
	{
		NEW_CLASS(renderer, Media::ALSARenderer(devName.v + 6));
	}
	else if (devName.StartsWith(UTF8STRC("PA: ")))
	{
//		NEW_CLASS(renderer, Media::ALSARenderer(devName.v + 6));
	}
	return renderer;
}

Media::AudioDevice::AudioDevice()
{
	this->currRenderer = 0;
}

Media::AudioDevice::~AudioDevice()
{
	this->ClearDevices();
}

Bool Media::AudioDevice::AddDevice(Text::CStringNN devName)
{
	Media::IAudioRenderer *renderer;
	Bool ret = false;
	if (devName.StartsWith(UTF8STRC("ALSA: ")))
	{
		NEW_CLASS(renderer, Media::ALSARenderer(devName.v + 6));
		if (renderer->IsError())
		{
			DEL_CLASS(renderer);
		}
		else
		{
			this->rendererList.Add(renderer);
			ret = true;
		}
	}
	else if (devName.StartsWith(UTF8STRC("PA: ")))
	{
/*		NEW_CLASS(renderer, Media::ALSARenderer(devName.v + 6));
		if (renderer->IsError())
		{
			DEL_CLASS(renderer);
		}
		else
		{
			this->rendererList.Add(renderer);
			ret = true;
		}*/
	}
	return ret;
}

void Media::AudioDevice::ClearDevices()
{
	UOSInt i;
	Media::IAudioRenderer *renderer;

	BindAudio(0);
	i = this->rendererList.GetCount();
	while (i-- > 0)
	{
		renderer = this->rendererList.GetItem(i);
		DEL_CLASS(renderer);
	}
	this->rendererList.Clear();
}

Media::IAudioRenderer *Media::AudioDevice::BindAudio(Media::IAudioSource *audsrc)
{
	UOSInt i;
	UOSInt j;
	Media::IAudioRenderer *renderer;
	if (this->rendererList.GetCount() == 0)
	{
		NEW_CLASS(renderer, Media::ALSARenderer(0));
		if (renderer->IsError())
		{
			DEL_CLASS(renderer);
		}
		else
		{
			this->rendererList.Add(renderer);
		}
		renderer = 0;
	}
	if (this->currRenderer)
	{
		this->currRenderer->BindAudio(0);
		this->currRenderer = 0;
	}
	if (audsrc == 0)
		return 0;
	i = 0;
	j = this->rendererList.GetCount();
	while (i < j)
	{
		renderer = this->rendererList.GetItem(i);
		if (renderer->BindAudio(audsrc))
		{
			if (renderer->IsError())
			{
				renderer->BindAudio(0);
			}
			else
			{
				this->currRenderer = renderer;
				break;
			}
		}
		i++;
	}
	return this->currRenderer;
}

