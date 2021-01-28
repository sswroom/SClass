#ifndef _SM_MEDIA_IREALTIMEVIDEOSOURCE
#define _SM_MEDIA_IREALTIMEVIDEOSOURCE
#include "Media/IVideoSource.h"

namespace Media
{
	class IRealtimeVideoSource : public IVideoSource
	{
	protected:
		OSInt cropLeft;
		OSInt cropTop;
		OSInt cropRight;
		OSInt cropBottom;
	public:
		IRealtimeVideoSource();
		virtual ~IRealtimeVideoSource();

		virtual UTF8Char *GetSourceName(UTF8Char *buff) = 0;

		virtual void SetBorderCrop(OSInt cropLeft, OSInt cropTop, OSInt cropRight, OSInt cropBottom);
		virtual void GetBorderCrop(OSInt *cropLeft, OSInt *cropTop, OSInt *cropRight, OSInt *cropBottom);

		virtual Bool GetVideoInfo(Media::FrameInfo *info, Int32 *frameRateNorm, Int32 *frameRateDenorm, UOSInt *maxFrameSize) = 0;
		virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, void *userData) = 0;
		virtual Bool Start() = 0; //true = succeed
		virtual void Stop() = 0;
		virtual Bool IsVideoCapture();

		virtual Int32 GetStreamTime();
		virtual Bool CanSeek();
		virtual Int32 SeekToTime(Int32 time);
		virtual Bool IsRealTimeSrc();
		virtual Bool TrimStream(Int32 trimTimeStart, Int32 trimTimeEnd, Int32 *syncTime);

		virtual OSInt GetFrameCount(); //-1 = unknown;
		virtual UInt32 GetFrameTime(UOSInt frameIndex);
		virtual void EnumFrameInfos(FrameInfoCallback cb, void *userData);

		virtual OSInt ReadNextFrame(UInt8 *frameBuff, Int32 *frameTime, Media::FrameType *ftype); //ret 0 = no more frames
	};
}
#endif
