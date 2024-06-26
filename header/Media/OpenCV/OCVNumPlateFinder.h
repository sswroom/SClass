#ifndef _SM_MEDIA_OPENCV_OCVNUMPLATEFINDER
#define _SM_MEDIA_OPENCV_OCVNUMPLATEFINDER
#include "AnyType.h"
#include "Media/OpenCV/OCVFrame.h"

namespace Media
{
	namespace OpenCV
	{
		class OCVNumPlateFinder
		{
		public:
			enum class PlateSize
			{
				SingleRow, //440x140
				DoubleRow //440x220
			};

			typedef void (CALLBACKFUNC PossibleAreaFunc)(AnyType userObj, NN<Media::OpenCV::OCVFrame> filteredFrame, UnsafeArray<Math::Coord2D<UOSInt>> rect, Double tileAngle, Double area, PlateSize psize);
		private:
			Double maxTiltAngle;
			Double minArea;
			Double maxArea;
		public:
			OCVNumPlateFinder();
			~OCVNumPlateFinder();
			
			void SetMaxTiltAngle(Double maxTiltAngleDegree);
			void SetAreaRange(Double minArea, Double maxArea);
			void Find(NN<Media::OpenCV::OCVFrame> frame, PossibleAreaFunc func, AnyType userObj);
		};
	}
}
#endif
