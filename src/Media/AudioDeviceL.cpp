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

Optional<Media::AudioRenderer> Media::AudioDevice::CreateRenderer(Text::CStringNN devName)
{
	Media::AudioRenderer *renderer = 0;
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
	NN<Media::AudioRenderer> renderer;
	Bool ret = false;
	if (devName.StartsWith(UTF8STRC("ALSA: ")))
	{
		NEW_CLASSNN(renderer, Media::ALSARenderer(devName.v + 6));
		if (renderer->IsError())
		{
			renderer.Delete();
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
	BindAudio(0);
	this->rendererList.DeleteAll();
}

Optional<Media::AudioRenderer> Media::AudioDevice::BindAudio(Optional<Media::AudioSource> audsrc)
{
	UOSInt i;
	UOSInt j;
	NN<Media::AudioRenderer> renderer;
	NN<Media::AudioSource> nnaudsrc;
	if (this->rendererList.GetCount() == 0)
	{
		NEW_CLASSNN(renderer, Media::ALSARenderer(0));
		if (renderer->IsError())
		{
			renderer.Delete();
		}
		else
		{
			this->rendererList.Add(renderer);
		}
	}
	if (this->currRenderer.SetTo(renderer))
	{
		renderer->BindAudio(0);
		this->currRenderer = 0;
	}
	if (!audsrc.SetTo(nnaudsrc))
		return 0;
	i = 0;
	j = this->rendererList.GetCount();
	while (i < j)
	{
		renderer = this->rendererList.GetItemNoCheck(i);
		if (renderer->BindAudio(nnaudsrc))
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

