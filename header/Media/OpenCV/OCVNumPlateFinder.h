#ifndef _SM_MEDIA_OPENCV_OCVNUMPLATEFINDER
#define _SM_MEDIA_OPENCV_OCVNUMPLATEFINDER
#include "Media/OpenCV/OCVFrame.h"

namespace Media
{
	namespace OpenCV
	{
		class OCVNumPlateFinder
		{
		public:
			typedef void (__stdcall *PossibleAreaFunc)(void *userObj, Media::OpenCV::OCVFrame *filteredFrame, UOSInt *rect);
		public:
			static void Find(Media::OpenCV::OCVFrame *frame, PossibleAreaFunc func, void *userObj);
		};
	}
}
#endif
