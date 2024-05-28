#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ASIOOutRenderer.h"
#include "Media/AudioDevice.h"
#include "Media/KSRenderer.h"
#include "Media/WaveOutRenderer.h"
#include "Text/MyString.h"

UOSInt Media::AudioDevice::GetDeviceCount()
{
	UOSInt asioCnt = Media::ASIOOutRenderer::GetDeviceCount();
	UOSInt woCnt = Media::WaveOutRenderer::GetDeviceCount();
#ifndef _WIN32_WCE
	UOSInt ksCnt = Media::KSRenderer::GetDeviceCount();
#else
	UOSInt ksCnt = 0;
#endif
	return asioCnt + woCnt + ksCnt;
}

UTF8Char *Media::AudioDevice::GetDeviceName(UTF8Char *buff, UOSInt devNo)
{
	UOSInt asioCnt = Media::ASIOOutRenderer::GetDeviceCount();
	UOSInt woCnt = Media::WaveOutRenderer::GetDeviceCount();
#ifndef _WIN32_WCE
	UOSInt ksCnt = Media::KSRenderer::GetDeviceCount();
#else
	OSInt ksCnt = 0;
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

Optional<Media::IAudioRenderer> Media::AudioDevice::CreateRenderer(Text::CString devName)
{
Media::IAudioRenderer *renderer = 0;
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

Bool Media::AudioDevice::AddDevice(Text::CString devName)
{
	Media::IAudioRenderer *renderer;
	Bool ret = false;
#ifndef _WIN32_WCE
	if (devName.StartsWith(UTF8STRC("KS: ")))
	{
		NEW_CLASS(renderer, Media::KSRenderer(Media::KSRenderer::GetDeviceId(devName.v + 4)));
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
#endif
	if (devName.StartsWith(UTF8STRC("ASIO: ")))
	{
		NEW_CLASS(renderer, Media::ASIOOutRenderer(devName.v + 6));
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
	else if (devName.StartsWith(UTF8STRC("WO: ")))
	{
		NEW_CLASS(renderer, Media::WaveOutRenderer(devName.v + 4));
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
		NEW_CLASS(renderer, Media::WaveOutRenderer(0));
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
