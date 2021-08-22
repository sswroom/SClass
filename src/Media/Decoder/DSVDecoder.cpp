#include "stdafx.h"
#include "MyMemory.h"
#include "Media/MPEG4V.h"
#include "Media/StaticImage.h"
#include "Media/Decoder/DSVDecoder.h"
#include "Media/DShow/DShowVideoFilter.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include <windows.h>

#include "Media/H264Parser.h"
#include "IO/DebugWriter.h"

void Media::Decoder::DSVDecoder::ProcVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 *imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
}

Media::Decoder::DSVDecoder::DSVDecoder(Media::IVideoSource *sourceVideo) : Media::Decoder::VDecoderBase(sourceVideo)
{
	IGraphBuilder *pGraph = NULL;
	HRESULT hr =  CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);
	if (SUCCEEDED(hr))
	{
		Media::DShow::DShowVideoFilter *captureFilter;
		NEW_CLASS(captureFilter, Media::DShow::DShowVideoFilter(true));
		pGraph->AddFilter(captureFilter, L"DShowVideoCapture");
/*		IEnumPins *enums;
		if (((IBaseFilter*)baseFilter)->EnumPins(&enums) == S_OK)
		{
			IPin *pin;
			enums->Next(1, &pin, 0);
			this->pin1 = pin;
			this->pin2 = ((DShowVideoFilter*)captureFilter)->GetPin(0);
			pGraph->Connect(pin1, pin2);
			enums->Release();
		}*/
		this->graph = pGraph;
	}
}

Media::Decoder::DSVDecoder::~DSVDecoder()
{
	this->Stop();
	if (this->bmihSrc)
	{
		MemFree(this->bmihSrc);
		this->bmihSrc = 0;
	}
	if (this->bmihDest)
	{
		MemFree(this->bmihDest);
		this->bmihDest = 0;
	}
	if (this->frameBuff)
	{
		MemFreeA64(this->frameBuff);
		this->frameBuff = 0;
	}
}

Bool Media::Decoder::DSVDecoder::CaptureImage(ImageCallback imgCb, void *userData)
{
	this->imgCb = imgCb;
	this->imgCbData = userData;
	return true;
}

Bool Media::Decoder::DSVDecoder::GetVideoInfo(Media::FrameInfo *info, Int32 *frameRateNorm, Int32 *frameRateDenorm, UOSInt *maxFrameSize)
{
	if (this->sourceVideo == 0)
		return false;
	if (!this->sourceVideo->GetVideoInfo(info, frameRateNorm, frameRateDenorm, maxFrameSize))
		return false;
	BITMAPINFOHEADER *bmih = (BITMAPINFOHEADER *)this->bmihDest;
	*frameRateNorm = this->frameRateNorm;
	*frameRateDenorm = this->frameRateDenorm;
	*maxFrameSize = this->maxFrameSize;
	info->dispWidth = bmih->biWidth;
	info->dispHeight = bmih->biHeight;
	info->storeWidth = bmih->biWidth;
	info->storeHeight = bmih->biHeight;
	info->storeBPP = bmih->biBitCount;
	info->fourcc = bmih->biCompression;
	info->pf = Media::PixelFormatGetDef(bmih->biCompression, bmih->biBitCount);
	info->byteSize = this->maxFrameSize;
	return true;
}

void Media::Decoder::DSVDecoder::Stop()
{
	if (this->sourceVideo == 0)
		return;

	this->started = false;
	this->sourceVideo->Stop();
	this->frameCb = 0;
	this->frameCbData = 0;
}


OSInt Media::Decoder::DSVDecoder::GetFrameCount()
{
	return this->sourceVideo->GetFrameCount();
}

UInt32 Media::Decoder::DSVDecoder::GetFrameTime(UOSInt frameIndex)
{
	return this->sourceVideo->GetFrameTime(frameIndex);
}

void Media::Decoder::DSVDecoder::EnumFrameInfos(FrameInfoCallback cb, void *userData)
{
	return this->sourceVideo->EnumFrameInfos(cb, userData);
}

void Media::Decoder::DSVDecoder::OnFrameChanged(Media::IVideoSource::FrameChange fc)
{
	if (fc == Media::IVideoSource::FC_PAR)
	{
	}
	else if (fc == Media::IVideoSource::FC_ENDPLAY)
	{
	}
}
