#ifndef _SM_MEDIA_OPENCV_OCVFRAMEFEEDER
#define _SM_MEDIA_OPENCV_OCVFRAMEFEEDER
#include "Media/VideoSource.h"
#include "Media/OpenCV/OCVObjectDetector.h"

namespace Media
{
	namespace OpenCV
	{
		class OCVFrameFeeder
		{
		private:
			Media::OpenCV::OCVObjectDetector *frameInput;
			NN<Media::VideoSource> src;
			Media::VideoSource *decoder;
			Media::FrameInfo info;
			OSInt frameSkip;
			OSInt thisSkip;
			UInt32 preferedFormat;
			UOSInt preferedWidth;
			UOSInt preferedHeight;

			static void __stdcall OnFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UOSInt dataSize, Media::VideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst);
			static void __stdcall OnFrameChange(Media::VideoSource::FrameChange frChg, AnyType userData);
		public:
			OCVFrameFeeder(Media::OpenCV::OCVObjectDetector *frameInput, NN<Media::VideoSource> src);
			~OCVFrameFeeder();

			Bool Start();
			void SetFrameSkip(OSInt frameSkip);
			void SetPreferedFormat(UInt32 preferedFormat, UOSInt preferedWidth, UOSInt preferedHeight);
		};
	}
}
#endif
