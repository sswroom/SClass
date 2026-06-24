#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/AudioDevice.h"
#include "Text/MyString.h"

UIntOS Media::AudioDevice::GetDeviceCount()
{
	return 0;
}

UnsafeArrayOpt<UTF8Char> Media::AudioDevice::GetDeviceName(UnsafeArray<UTF8Char> buff, UIntOS devNo)
{
	return nullptr;
}

Optional<Media::AudioRenderer> Media::AudioDevice::CreateRenderer(Text::CStringNN devName)
{
	Optional<Media::AudioRenderer> renderer = nullptr;
	return renderer;
}

Media::AudioDevice::AudioDevice()
{
	this->currRenderer = nullptr;
}

Media::AudioDevice::~AudioDevice()
{
	this->ClearDevices();
}

Bool Media::AudioDevice::AddDevice(Text::CStringNN devName)
{
	Bool ret = false;
	return ret;
}

void Media::AudioDevice::ClearDevices()
{
	BindAudio(nullptr);
	this->rendererList.DeleteAll();
}

Optional<Media::AudioRenderer> Media::AudioDevice::BindAudio(Optional<Media::AudioSource> audsrc)
{
	IntOS i;
	IntOS j;
	NN<Media::AudioRenderer> renderer;
	NN<Media::AudioRenderer> currRenderer;
	NN<Media::AudioSource> nnaudsrc;
	if (this->rendererList.GetCount() == 0)
	{
		return nullptr;
	}
	if (this->currRenderer.SetTo(currRenderer))
	{
		currRenderer->BindAudio(nullptr);
		this->currRenderer = nullptr;
	}
	if (!audsrc.SetTo(nnaudsrc))
		return nullptr;
	i = 0;
	j = this->rendererList.GetCount();
	while (i < j)
	{
		renderer = this->rendererList.GetItemNoCheck(i);
		if (renderer->BindAudio(nnaudsrc))
		{
			if (renderer->IsError())
			{
				renderer->BindAudio(nullptr);
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

