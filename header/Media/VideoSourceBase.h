#ifndef _SM_MEDIA_VIDEOSOURCEBASE
#define _SM_MEDIA_VIDEOSOURCEBASE
#include "Media/VideoSource.h"

namespace Media
{
	class VideoSourceBase : public Media::VideoSource
	{
	protected:
		UIntOS cropLeft;
		UIntOS cropTop;
		UIntOS cropRight;
		UIntOS cropBottom;

	public:
		VideoSourceBase();
		virtual ~VideoSourceBase();

		virtual void SetBorderCrop(UIntOS cropLeft, UIntOS cropTop, UIntOS cropRight, UIntOS cropBottom);
		virtual void GetBorderCrop(OutParam<UIntOS> cropLeft, OutParam<UIntOS> cropTop, OutParam<UIntOS> cropRight, OutParam<UIntOS> cropBottom);
	};
}
#endif
