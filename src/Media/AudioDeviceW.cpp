#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ASIOOutRenderer.h"
#include "Media/AudioDevice.h"
#include "Media/KSRenderer.h"
#include "Media/WaveOutRenderer.h"
#include "Text/MyString.h"

UIntOS Media::AudioDevice::GetDeviceCount()
{
	UIntOS asioCnt = Media::ASIOOutRenderer::GetDeviceCount();
	UIntOS woCnt = Media::WaveOutRenderer::GetDeviceCount();
#ifndef _WIN32_WCE
	UIntOS ksCnt = Media::KSRenderer::GetDeviceCount();
#else
	UIntOS ksCnt = 0;
#endif
	return asioCnt + woCnt + ksCnt;
}

UnsafeArrayOpt<UTF8Char> Media::AudioDevice::GetDeviceName(UnsafeArray<UTF8Char> buff, UIntOS devNo)
{
	UIntOS asioCnt = Media::ASIOOutRenderer::GetDeviceCount();
	UIntOS woCnt = Media::WaveOutRenderer::GetDeviceCount();
#ifndef _WIN32_WCE
	UIntOS ksCnt = Media::KSRenderer::GetDeviceCount();
#else
	IntOS ksCnt = 0;
#endif
	if (devNo < asioCnt)
	{
		return Media::ASIOOutRenderer::GetDeviceName(Text::StrConcatC(buff, UTF8STRC("ASIO: ")), devNo);
	}
	else if (devNo < asioCnt + woCnt)
	{
		return Media::WaveOutRenderer::GetDeviceName(Text::StrConcatC(buff, UTF8STRC("WO: ")), devNo - asioCnt);
	}
#if !defined(_WIN32_WCE)
	else if (devNo < asioCnt + woCnt + ksCnt)
	{
		return Media::KSRenderer::GetDeviceName(Text::StrConcatC(buff, UTF8STRC("KS: ")), devNo - asioCnt - woCnt);
	}
#endif
	return 0;
}

Optional<Media::AudioRenderer> Media::AudioDevice::CreateRenderer(Text::CStringNN devName)
{
Media::AudioRenderer *renderer = 0;
#ifndef _WIN32_WCE
	if (devName.StartsWith(UTF8STRC("KS: ")))
	{
		NEW_CLASS(renderer, Media::KSRenderer(Media::KSRenderer::GetDeviceId(devName.v + 4)));
	}
#endif
	if (devName.StartsWith(UTF8STRC("ASIO: ")))
	{
		NEW_CLASS(renderer, Media::ASIOOutRenderer(devName.v + 6));
	}
	else if (devName.StartsWith(UTF8STRC("WO: ")))
	{
		NEW_CLASS(renderer, Media::WaveOutRenderer(devName.v + 4));
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
#ifndef _WIN32_WCE
	if (devName.StartsWith(UTF8STRC("KS: ")))
	{
		NEW_CLASSNN(renderer, Media::KSRenderer(Media::KSRenderer::GetDeviceId(devName.v + 4)));
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
#endif
	if (devName.StartsWith(UTF8STRC("ASIO: ")))
	{
		NEW_CLASSNN(renderer, Media::ASIOOutRenderer(devName.v + 6));
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
	else if (devName.StartsWith(UTF8STRC("WO: ")))
	{
		NEW_CLASSNN(renderer, Media::WaveOutRenderer(devName.v + 4));
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

void Media::AudioDevice::ClearDevices()
{
	BindAudio(0);
	this->rendererList.DeleteAll();
}

Optional<Media::AudioRenderer> Media::AudioDevice::BindAudio(Optional<Media::AudioSource> audsrc)
{
	UIntOS i;
	UIntOS j;
	NN<Media::AudioRenderer> renderer;
	if (this->rendererList.GetCount() == 0)
	{
		NEW_CLASSNN(renderer, Media::WaveOutRenderer(0));
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
	if (audsrc.IsNull())
		return 0;
	i = 0;
	j = this->rendererList.GetCount();
	while (i < j)
	{
		renderer = this->rendererList.GetItemNoCheck(i);
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
