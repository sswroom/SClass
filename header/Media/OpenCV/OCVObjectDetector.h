#ifndef _SM_MEDIA_OPENCV_OCVOBJECTDETECTOR
#define _SM_MEDIA_OPENCV_OCVOBJECTDETECTOR
#include "Media/FrameInfo.h"
#include "Media/OpenCV/OCVFrame.h"

namespace Media
{
	namespace OpenCV
	{
		class OCVObjectDetector
		{
		public:
			typedef struct
			{
				IntOS left;
				IntOS top;
				IntOS right;
				IntOS bottom;
			} ObjectRect;
			typedef void (CALLBACKFUNC DetectResultFunc)(AnyType userObj, UIntOS objCnt, UnsafeArrayOpt<const ObjectRect> objRects, NN<Media::FrameInfo> frInfo, UnsafeArray<UnsafeArray<UInt8>> imgData);
			
		private:
			void *cascade;
			DetectResultFunc detectResultHdlr;
			AnyType detectResultObj;
		public:
			OCVObjectDetector(UnsafeArray<const UTF8Char> path, UnsafeArray<const UTF8Char> dataFile);
			~OCVObjectDetector();

			Bool IsError();
			void NextFrame(NN<Media::OpenCV::OCVFrame> frame, NN<Media::FrameInfo> frInfo, UnsafeArray<UnsafeArray<UInt8>> imgData);

			void HandleDetectResult(DetectResultFunc func, AnyType userObj);
		};
	}
}
#endif
