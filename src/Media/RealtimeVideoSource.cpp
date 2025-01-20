#include "Stdafx.h"
#include "Media/RealtimeVideoSource.h"

Media::RealtimeVideoSource::RealtimeVideoSource()
{
	this->cropLeft = 0;
	this->cropTop = 0;
	this->cropRight = 0;
	this->cropBottom = 0;
}

Media::RealtimeVideoSource::~RealtimeVideoSource()
{
}

void Media::RealtimeVideoSource::SetBorderCrop(UOSInt cropLeft, UOSInt cropTop, UOSInt cropRight, UOSInt cropBottom)
{
	this->cropLeft = cropLeft;
	this->cropTop = cropTop;
	this->cropRight = cropRight;
	this->cropBottom = cropBottom;
}

void Media::RealtimeVideoSource::GetBorderCrop(OutParam<UOSInt> cropLeft, OutParam<UOSInt> cropTop, OutParam<UOSInt> cropRight, OutParam<UOSInt> cropBottom)
{
	cropLeft.Set(this->cropLeft);
	cropTop.Set(this->cropTop);
	cropRight.Set(this->cropRight);
	cropBottom.Set(this->cropBottom);
}

Bool Media::RealtimeVideoSource::IsVideoCapture()
{
	return false;
}

Data::Duration Media::RealtimeVideoSource::GetStreamTime()
{
	return Data::Duration::Infinity();
}

Bool Media::RealtimeVideoSource::CanSeek()
{
	return false;
}

Data::Duration Media::RealtimeVideoSource::SeekToTime(Data::Duration time)
{
	return 0;
}

Bool Media::RealtimeVideoSource::IsRealTimeSrc()
{
	return true;
}

Bool Media::RealtimeVideoSource::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime)
{
	return false;
}

Bool Media::RealtimeVideoSource::HasFrameCount()
{
	return false;
}

UOSInt Media::RealtimeVideoSource::GetFrameCount()
{
	return 0;
}

Data::Duration Media::RealtimeVideoSource::GetFrameTime(UOSInt frameIndex)
{
	return 0;
}

void Media::RealtimeVideoSource::EnumFrameInfos(FrameInfoCallback cb, AnyType userData)
{
}

UOSInt Media::RealtimeVideoSource::ReadNextFrame(UnsafeArray<UInt8> frameBuff, OutParam<UInt32> frameTime, OutParam<Media::FrameType> ftype)
{
	///////////////////////////////////////
	return 0;
}
