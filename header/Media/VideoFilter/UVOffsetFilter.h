#ifndef _SM_MEDIA_VIDEOFILTER_UVOFFSETFILTER
#define _SM_MEDIA_VIDEOFILTER_UVOFFSETFILTER
#include "Media/VideoFilter/VideoFilterBase.h"

namespace Media
{
	namespace VideoFilter
	{
		class UVOffsetFilter : public Media::VideoFilter::VideoFilterBase
		{
		private:
			Int32 uOfst;
			Int32 vOfst;

			virtual void ProcessVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, void *userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		public:
			UVOffsetFilter(Media::IVideoSource *srcVideo);
			virtual ~UVOffsetFilter();

			virtual const UTF8Char *GetFilterName();

			void SetOffset(Int32 uOfst, Int32 vOfst);
		};
	}
}
#endif