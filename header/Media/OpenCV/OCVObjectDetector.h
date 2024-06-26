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
				OSInt left;
				OSInt top;
				OSInt right;
				OSInt bottom;
			} ObjectRect;
			typedef void (CALLBACKFUNC DetectResultFunc)(void *userObj, UOSInt objCnt, const ObjectRect *objRects, Media::FrameInfo *frInfo, UnsafeArray<UnsafeArray<UInt8>> imgData);
			
		private:
			void *cascade;
			DetectResultFunc detectResultHdlr;
			void *detectResultObj;
		public:
			OCVObjectDetector(const UTF8Char *path, const UTF8Char *dataFile);
			~OCVObjectDetector();

			Bool IsError();
			void NextFrame(NN<Media::OpenCV::OCVFrame> frame, Media::FrameInfo *frInfo, UnsafeArray<UnsafeArray<UInt8>> imgData);

			void HandleDetectResult(DetectResultFunc func, void *userObj);
		};
	}
}
#endif
