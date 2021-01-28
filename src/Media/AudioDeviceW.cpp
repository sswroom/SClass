#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ASIOOutRenderer.h"
#include "Media/AudioDevice.h"
#include "Media/KSRenderer.h"
#include "Media/WaveOutRenderer.h"
#include "Text/MyString.h"

OSInt Media::AudioDevice::GetDeviceCount()
{
	OSInt asioCnt = Media::ASIOOutRenderer::GetDeviceCount();
	OSInt woCnt = Media::WaveOutRenderer::GetDeviceCount();
#ifndef _WIN32_WCE
	OSInt ksCnt = Media::KSRenderer::GetDeviceCount();
#else
	OSInt ksCnt = 0;
#endif
	return asioCnt + woCnt + ksCnt;
}

UTF8Char *Media::AudioDevice::GetDeviceName(UTF8Char *buff, OSInt devNo)
{
	OSInt asioCnt = Media::ASIOOutRenderer::GetDeviceCount();
	OSInt woCnt = Media::WaveOutRenderer::GetDeviceCount();
#ifndef _WIN32_WCE
	OSInt ksCnt = Media::KSRenderer::GetDeviceCount();
#else
	OSInt ksCnt = 0;
#endif
	if (devNo < asioCnt)
	{
		return Media::ASIOOutRenderer::GetDeviceName(Text::StrConcat(buff, (const UTF8Char*)"ASIO: "), devNo);
	}
	else if (devNo < asioCnt + woCnt)
	{
		return Media::WaveOutRenderer::GetDeviceName(Text::StrConcat(buff, (const UTF8Char*)"WO: "), devNo - asioCnt);
	}
#if !defined(_WIN32_WCE)
	else if (devNo < asioCnt + woCnt + ksCnt)
	{
		return Media::KSRenderer::GetDeviceName(Text::StrConcat(buff, (const UTF8Char*)"KS: "), devNo - asioCnt - woCnt);
	}
#endif
	return 0;
}

Media::IAudioRenderer *Media::AudioDevice::CreateRenderer(const UTF8Char *devName)
{
Media::IAudioRenderer *renderer = 0;
#ifndef _WIN32_WCE
	if (Text::StrStartsWith(devName, (const UTF8Char*)"KS: "))
	{
		NEW_CLASS(renderer, Media::KSRenderer(Media::KSRenderer::GetDeviceId(devName + 4)));
	}
#endif
	if (Text::StrStartsWith(devName, (const UTF8Char*)"ASIO: "))
	{
		NEW_CLASS(renderer, Media::ASIOOutRenderer(devName + 6));
	}
	else if (Text::StrStartsWith(devName, (const UTF8Char*)"WO: "))
	{
		NEW_CLASS(renderer, Media::WaveOutRenderer(devName + 4));
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
#ifndef _WIN32_WCE
	if (Text::StrStartsWith(devName, (const UTF8Char*)"KS: "))
	{
		NEW_CLASS(renderer, Media::KSRenderer(Media::KSRenderer::GetDeviceId(devName + 4)));
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
#endif
	if (Text::StrStartsWith(devName, (const UTF8Char*)"ASIO: "))
	{
		NEW_CLASS(renderer, Media::ASIOOutRenderer(devName + 6));
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
	else if (Text::StrStartsWith(devName, (const UTF8Char*)"WO: "))
	{
		NEW_CLASS(renderer, Media::WaveOutRenderer(devName + 4));
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
	OSInt i;
	OSInt j;
	Media::IAudioRenderer *renderer;
	if (this->rendererList->GetCount() == 0)
	{
		NEW_CLASS(renderer, Media::WaveOutRenderer(0));
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
