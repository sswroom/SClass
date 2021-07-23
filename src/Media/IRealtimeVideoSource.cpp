#include "Stdafx.h"
#include "Media/IRealtimeVideoSource.h"

Media::IRealtimeVideoSource::IRealtimeVideoSource()
{
	this->cropLeft = 0;
	this->cropTop = 0;
	this->cropRight = 0;
	this->cropBottom = 0;
}

Media::IRealtimeVideoSource::~IRealtimeVideoSource()
{
}

void Media::IRealtimeVideoSource::SetBorderCrop(UOSInt cropLeft, UOSInt cropTop, UOSInt cropRight, UOSInt cropBottom)
{
	this->cropLeft = cropLeft;
	this->cropTop = cropTop;
	this->cropRight = cropRight;
	this->cropBottom = cropBottom;
}

void Media::IRealtimeVideoSource::GetBorderCrop(UOSInt *cropLeft, UOSInt *cropTop, UOSInt *cropRight, UOSInt *cropBottom)
{
	*cropLeft = this->cropLeft;
	*cropTop = this->cropTop;
	*cropRight = this->cropRight;
	*cropBottom = this->cropBottom;
}

Bool Media::IRealtimeVideoSource::IsVideoCapture()
{
	return false;
}

Int32 Media::IRealtimeVideoSource::GetStreamTime()
{
	return -1;
}

Bool Media::IRealtimeVideoSource::CanSeek()
{
	return false;
}

UInt32 Media::IRealtimeVideoSource::SeekToTime(UInt32 time)
{
	return 0;
}

Bool Media::IRealtimeVideoSource::IsRealTimeSrc()
{
	return true;
}

Bool Media::IRealtimeVideoSource::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime)
{
	return false;
}

Bool Media::IRealtimeVideoSource::HasFrameCount()
{
	return false;
}

UOSInt Media::IRealtimeVideoSource::GetFrameCount()
{
	return 0;
}

UInt32 Media::IRealtimeVideoSource::GetFrameTime(UOSInt frameIndex)
{
	return 0;
}

void Media::IRealtimeVideoSource::EnumFrameInfos(FrameInfoCallback cb, void *userData)
{
}

UOSInt Media::IRealtimeVideoSource::ReadNextFrame(UInt8 *frameBuff, UInt32 *frameTime, Media::FrameType *ftype)
{
	///////////////////////////////////////
	return 0;
}
