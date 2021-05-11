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

			virtual void ProcessVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, void *userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
			virtual void OnFrameChange(Media::IVideoSource::FrameChange fc);
		public:
			AutoCropFilter(Media::IVideoSource *srcVideo);
			virtual ~AutoCropFilter();

			virtual const UTF8Char *GetFilterName();

			void SetEnabled(Bool enabled);

			virtual void GetBorderCrop(UOSInt *cropLeft, UOSInt *cropTop, UOSInt *cropRight, UOSInt *cropBottom);
		};
	}
}
#endif
