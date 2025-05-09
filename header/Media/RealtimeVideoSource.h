#ifndef _SM_MEDIA_REALTIMEVIDEOSOURCE
#define _SM_MEDIA_REALTIMEVIDEOSOURCE
#include "AnyType.h"
#include "Media/VideoSource.h"

namespace Media
{
	class RealtimeVideoSource : public VideoSource
	{
	protected:
		UOSInt cropLeft;
		UOSInt cropTop;
		UOSInt cropRight;
		UOSInt cropBottom;
	public:
		RealtimeVideoSource();
		virtual ~RealtimeVideoSource();

		virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff) = 0;

		virtual void SetBorderCrop(UOSInt cropLeft, UOSInt cropTop, UOSInt cropRight, UOSInt cropBottom);
		virtual void GetBorderCrop(OutParam<UOSInt> cropLeft, OutParam<UOSInt> cropTop, OutParam<UOSInt> cropRight, OutParam<UOSInt> cropBottom);

		virtual Bool GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize) = 0;
		virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, AnyType userData) = 0;
		virtual Bool Start() = 0; //true = succeed
		virtual void Stop() = 0;
		virtual Bool IsVideoCapture();

		virtual Data::Duration GetStreamTime();
		virtual Bool CanSeek();
		virtual Data::Duration SeekToTime(Data::Duration time);
		virtual Bool IsRealTimeSrc();
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime);

		virtual Bool HasFrameCount();
		virtual UOSInt GetFrameCount();
		virtual Data::Duration GetFrameTime(UOSInt frameIndex);
		virtual void EnumFrameInfos(FrameInfoCallback cb, AnyType userData);

		virtual UOSInt ReadNextFrame(UnsafeArray<UInt8> frameBuff, OutParam<UInt32> frameTime, OutParam<Media::FrameType> ftype); //ret 0 = no more frames
	};
}
#endif
