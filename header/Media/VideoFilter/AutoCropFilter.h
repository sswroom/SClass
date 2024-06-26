#ifndef _SM_MEDIA_VIDEOFILTER_AUTOCROPFILTER
#define _SM_MEDIA_VIDEOFILTER_AUTOCROPFILTER
#include "Media/VideoFilter/VideoFilterBase.h"

namespace Media
{
	namespace VideoFilter
	{
		class AutoCropFilter : public Media::VideoFilter::VideoFilterBase
		{
		private:
			Bool hasCrop;
			Bool enabled;

			virtual void ProcessVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
			virtual void OnFrameChange(Media::IVideoSource::FrameChange fc);
		public:
			AutoCropFilter(Media::IVideoSource *srcVideo);
			virtual ~AutoCropFilter();

			virtual Text::CStringNN GetFilterName();

			void SetEnabled(Bool enabled);

			virtual void GetBorderCrop(OutParam<UOSInt> cropLeft, OutParam<UOSInt> cropTop, OutParam<UOSInt> cropRight, OutParam<UOSInt> cropBottom);
		};
	}
}
#endif
