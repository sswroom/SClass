#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/MPEG4V.h"
#include "Media/StaticImage.h"
#include "Media/Decoder/VFWDecoder.h"
#include "Sync/SimpleThread.h"
#include "Text/MyString.h"

#include "Media/H264Parser.h"
#include "IO/DebugWriter.h"

Bool Media::Decoder::VFWDecoder::GetFCCHandlers(UInt32 fourcc, Data::ArrayListUInt32 *fccHdlrs, Data::ArrayListUInt32 *outFccs, EncodingType *encType)
{

	return true;
}

void Media::Decoder::VFWDecoder::ProcVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UOSInt dataSize, Media::VideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{

}

Media::Decoder::VFWDecoder::VFWDecoder(NN<Media::VideoSource> sourceVideo) : Media::Decoder::VDecoderBase(sourceVideo)
{
	this->sourceVideo = 0;
	this->frameRateNorm = 0;
	this->frameRateDenorm = 0;
	this->maxFrameSize = 0;
	this->bmihSrc = 0;
	this->bmihDest = 0;
	this->hic = 0;
	this->frameCb = 0;
	this->frameCbData = 0;
	this->frameBuff = MemAllocAArr(UInt8, 0);
	this->imgCb = 0;
	this->frameChg = false;
	this->endProcessing = false;

	this->bCnt = 0;
	this->bDiscard = 0;
	this->uOfst = 0;
	this->vOfst = 0;
	
	UInt32 frameRateNorm;
	UInt32 frameRateDenorm;
	UOSInt maxFrameSize;
	Media::FrameInfo frameInfo;
	if (!sourceVideo->GetVideoInfo(frameInfo, frameRateNorm, frameRateDenorm, maxFrameSize))
		return;
}

Media::Decoder::VFWDecoder::~VFWDecoder()
{
	this->Stop();
	MemFreeAArr(this->frameBuff);
}

Bool Media::Decoder::VFWDecoder::CaptureImage(ImageCallback imgCb, AnyType userData)
{
	this->imgCb = {imgCb, userData};
	return true;
}

Text::CStringNN Media::Decoder::VFWDecoder::GetFilterName()
{
	return CSTR("VFWDecoder");
}

Bool Media::Decoder::VFWDecoder::GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize)
{
	if (this->sourceVideo == 0)
		return false;
	if (!this->sourceVideo->GetVideoInfo(info, frameRateNorm, frameRateDenorm, maxFrameSize))
		return false;
	return false;
}

void Media::Decoder::VFWDecoder::Stop()
{
	if (this->sourceVideo == 0)
		return;

	this->started = false;
	this->sourceVideo->Stop();
	while (this->endProcessing)
	{
		Sync::SimpleThread::Sleep(10);
	}
	this->frameCb = 0;
	this->frameCbData = 0;
}

Bool Media::Decoder::VFWDecoder::HasFrameCount()
{
	return this->sourceVideo->HasFrameCount();
}

UOSInt Media::Decoder::VFWDecoder::GetFrameCount()
{
	return this->sourceVideo->GetFrameCount();
}

Data::Duration Media::Decoder::VFWDecoder::GetFrameTime(UOSInt frameIndex)
{
	return this->sourceVideo->GetFrameTime(frameIndex);
}

void Media::Decoder::VFWDecoder::EnumFrameInfos(FrameInfoCallback cb, AnyType userData)
{
	return this->sourceVideo->EnumFrameInfos(cb, userData);
}

void Media::Decoder::VFWDecoder::OnFrameChanged(Media::VideoSource::FrameChange fc)
{
	if (fc == Media::VideoSource::FC_PAR)
	{
		this->frameChg = true;
	}
	else if (fc == Media::VideoSource::FC_ENDPLAY)
	{
	}
}
