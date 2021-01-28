#ifndef _SM_MEDIA_VIDEOSOURCEBASE
#define _SM_MEDIA_VIDEOSOURCEBASE
#include "Media/IVideoSource.h"

namespace Media
{
	class VideoSourceBase : public Media::IVideoSource
	{
	protected:
		OSInt cropLeft;
		OSInt cropTop;
		OSInt cropRight;
		OSInt cropBottom;

	public:
		VideoSourceBase();
		virtual ~VideoSourceBase();

		virtual void SetBorderCrop(OSInt cropLeft, OSInt cropTop, OSInt cropRight, OSInt cropBottom);
		virtual void GetBorderCrop(OSInt *cropLeft, OSInt *cropTop, OSInt *cropRight, OSInt *cropBottom);
	};
};
#endif
