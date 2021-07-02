#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/AudioDevice.h"
#include "Text/MyString.h"

UOSInt Media::AudioDevice::GetDeviceCount()
{
	return 0;
}

UTF8Char *Media::AudioDevice::GetDeviceName(UTF8Char *buff, UOSInt devNo)
{
	return 0;
}

Media::IAudioRenderer *Media::AudioDevice::CreateRenderer(const UTF8Char *devName)
{
	Media::IAudioRenderer *renderer = 0;
	return renderer;
}

Media::AudioDevice::AudioDevice()
{
	NEW_CLASS(this->rendererList, Data::ArrayList<Media::IAudioRenderer*>());
	this->currRenderer = 0;
}

Media::AudioDevice::~AudioDevice()
{
	OSInt i;
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
	return ret;
}


Media::IAudioRenderer *Media::AudioDevice::BindAudio(Media::IAudioSource *audsrc)
{
	OSInt i;
	OSInt j;
	Media::IAudioRenderer *renderer;
	if (this->rendererList->GetCount() == 0)
	{
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

