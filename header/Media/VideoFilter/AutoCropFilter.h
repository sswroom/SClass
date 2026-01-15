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

			virtual void ProcessVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst);
			virtual void OnFrameChange(Media::VideoSource::FrameChange fc);
		public:
			AutoCropFilter(Media::VideoSource *srcVideo);
			virtual ~AutoCropFilter();

			virtual Text::CStringNN GetFilterName();

			void SetEnabled(Bool enabled);

			virtual void GetBorderCrop(OutParam<UIntOS> cropLeft, OutParam<UIntOS> cropTop, OutParam<UIntOS> cropRight, OutParam<UIntOS> cropBottom);
		};
	}
}
#endif
