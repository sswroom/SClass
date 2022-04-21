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
			typedef void (__stdcall *PossibleAreaFunc)(void *userObj, Media::OpenCV::OCVFrame *filteredFrame, UOSInt *rect, Double tileAngle, Double area);
		private:
			Double maxTileAngle;
			Double minArea;
			Double maxArea;
		public:
			OCVNumPlateFinder();
			~OCVNumPlateFinder();
			
			void SetMaxTileAngle(Double maxTileAngleDegree);
			void SetAreaRange(Double minArea, Double maxArea);
			void Find(Media::OpenCV::OCVFrame *frame, PossibleAreaFunc func, void *userObj);
		};
	}
}
#endif
