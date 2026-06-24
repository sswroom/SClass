#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/AudioDevice.h"
#include "Media/OpenSLESRenderer.h"
#include "Text/MyString.h"

UIntOS Media::AudioDevice::GetDeviceCount()
{
	return Media::OpenSLESRenderer::GetDeviceCount();
}

UnsafeArrayOpt<UTF8Char> Media::AudioDevice::GetDeviceName(UnsafeArray<UTF8Char> buff, UIntOS devNo)
{
	return Media::OpenSLESRenderer::GetDeviceName(Text::StrConcatC(buff, UTF8STRC("OpenSLES: ")), devNo);
}

Optional<Media::AudioRenderer> Media::AudioDevice::CreateRenderer(Text::CStringNN devName)
{
	Optional<Media::AudioRenderer> renderer = nullptr;
	if (devName.StartsWith(UTF8STRC("OpenSLES: ")))
	{
		NEW_CLASSOPT(renderer, Media::OpenSLESRenderer(devName.v + 10));
	}
	return renderer;
}

Media::AudioDevice::AudioDevice()
{
	this->currRenderer = nullptr;
}

Media::AudioDevice::~AudioDevice()
{
	BindAudio(nullptr);
	this->rendererList.DeleteAll();
}

Bool Media::AudioDevice::AddDevice(Text::CStringNN devName)
{
	NN<Media::AudioRenderer> renderer;
	Bool ret = false;
	if (devName.StartsWith(UTF8STRC("OpenSLES: ")))
	{
		NEW_CLASSNN(renderer, Media::OpenSLESRenderer(devName.v + 6));
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
	return ret;
}


Optional<Media::AudioRenderer> Media::AudioDevice::BindAudio(Optional<Media::AudioSource> audsrc)
{
	UIntOS i;
	UIntOS j;
	NN<Media::AudioRenderer> renderer;
	NN<Media::AudioSource> nnaudsrc;
	if (this->rendererList.GetCount() == 0)
	{
		NEW_CLASSNN(renderer, Media::OpenSLESRenderer(nullptr));
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
		renderer->BindAudio(nullptr);
		this->currRenderer = nullptr;
	}
	if (!audsrc.SetTo(nnaudsrc))
		return nullptr;
	i = 0;
	j = this->rendererList.GetCount();
	while (i < j)
	{
		renderer = this->rendererList.GetItemNoCheck(i);
		if (renderer->BindAudio(audsrc))
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

