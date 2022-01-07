#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ALSARenderer.h"
#include "Media/AudioDevice.h"
#include "Text/MyString.h"

UOSInt Media::AudioDevice::GetDeviceCount()
{
	return Media::ALSARenderer::GetDeviceCount();
}

UTF8Char *Media::AudioDevice::GetDeviceName(UTF8Char *buff, UOSInt devNo)
{
	return Media::ALSARenderer::GetDeviceName(Text::StrConcatC(buff, UTF8STRC("ALSA: ")), devNo);
}

Media::IAudioRenderer *Media::AudioDevice::CreateRenderer(const UTF8Char *devName)
{
	Media::IAudioRenderer *renderer = 0;
	if (Text::StrStartsWith(devName, (const UTF8Char*)"ALSA: "))
	{
		NEW_CLASS(renderer, Media::ALSARenderer(devName + 6));
	}
	return renderer;
}

Media::AudioDevice::AudioDevice()
{
	NEW_CLASS(this->rendererList, Data::ArrayList<Media::IAudioRenderer*>());
	this->currRenderer = 0;
}

Media::AudioDevice::~AudioDevice()
{
	UOSInt i;
	Media::IAudioRenderer *renderer;

	BindAudio(0);
	i = this->rendererList->GetCount();
	while (i-- > 0)
	{
		renderer = this->rendererList->GetItem(i);
		DEL_CLASS(renderer);
	}
	DEL_CLASS(this->rendererList);
}

Bool Media::AudioDevice::AddDevice(const UTF8Char *devName)
{
	Media::IAudioRenderer *renderer;
	Bool ret = false;
	if (Text::StrStartsWith(devName, (const UTF8Char*)"ALSA: "))
	{
		NEW_CLASS(renderer, Media::ALSARenderer(devName + 6));
		if (renderer->IsError())
		{
			DEL_CLASS(renderer);
		}
		else
		{
			this->rendererList->Add(renderer);
			ret = true;
		}
	}
	return ret;
}


Media::IAudioRenderer *Media::AudioDevice::BindAudio(Media::IAudioSource *audsrc)
{
	UOSInt i;
	UOSInt j;
	Media::IAudioRenderer *renderer;
	if (this->rendererList->GetCount() == 0)
	{
		NEW_CLASS(renderer, Media::ALSARenderer(0));
		if (renderer->IsError())
		{
			DEL_CLASS(renderer);
		}
		else
		{
			this->rendererList->Add(renderer);
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
	j = this->rendererList->GetCount();
	while (i < j)
	{
		renderer = this->rendererList->GetItem(i);
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

