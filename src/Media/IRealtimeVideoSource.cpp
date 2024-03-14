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

void Media::IRealtimeVideoSource::GetBorderCrop(OutParam<UOSInt> cropLeft, OutParam<UOSInt> cropTop, OutParam<UOSInt> cropRight, OutParam<UOSInt> cropBottom)
{
	cropLeft.Set(this->cropLeft);
	cropTop.Set(this->cropTop);
	cropRight.Set(this->cropRight);
	cropBottom.Set(this->cropBottom);
}

Bool Media::IRealtimeVideoSource::IsVideoCapture()
{
	return false;
}

Data::Duration Media::IRealtimeVideoSource::GetStreamTime()
{
	return Data::Duration::Infinity();
}

Bool Media::IRealtimeVideoSource::CanSeek()
{
	return false;
}

Data::Duration Media::IRealtimeVideoSource::SeekToTime(Data::Duration time)
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

Data::Duration Media::IRealtimeVideoSource::GetFrameTime(UOSInt frameIndex)
{
	return 0;
}

void Media::IRealtimeVideoSource::EnumFrameInfos(FrameInfoCallback cb, AnyType userData)
{
}

UOSInt Media::IRealtimeVideoSource::ReadNextFrame(UInt8 *frameBuff, UInt32 *frameTime, Media::FrameType *ftype)
{
	///////////////////////////////////////
	return 0;
}
