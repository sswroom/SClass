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

Media::AudioRenderer *Media::AudioDevice::CreateRenderer(Text::CString devName)
{
	Media::AudioRenderer *renderer = 0;
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

Bool Media::AudioDevice::AddDevice(Text::CString devName)
{
	Media::AudioRenderer *renderer;
	Bool ret = false;
	return ret;
}

void Media::AudioDevice::ClearDevices()
{
	UOSInt i;
	Media::AudioRenderer *renderer;

	BindAudio(0);
	i = this->rendererList.GetCount();
	while (i-- > 0)
	{
		renderer = this->rendererList.GetItem(i);
		DEL_CLASS(renderer);
	}
	this->rendererList.Clear();
}

Media::AudioRenderer *Media::AudioDevice::BindAudio(Media::AudioSource *audsrc)
{
	OSInt i;
	OSInt j;
	Media::AudioRenderer *renderer;
	if (this->rendererList.GetCount() == 0)
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

