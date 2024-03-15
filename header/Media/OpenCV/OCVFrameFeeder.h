#ifndef _SM_MEDIA_OPENCV_OCVFRAMEFEEDER
#define _SM_MEDIA_OPENCV_OCVFRAMEFEEDER
#include "Media/IVideoSource.h"
#include "Media/OpenCV/OCVObjectDetector.h"

namespace Media
{
	namespace OpenCV
	{
		class OCVFrameFeeder
		{
		private:
			Media::OpenCV::OCVObjectDetector *frameInput;
			NotNullPtr<Media::IVideoSource> src;
			Media::IVideoSource *decoder;
			Media::FrameInfo info;
			OSInt frameSkip;
			OSInt thisSkip;
			UInt32 preferedFormat;
			UOSInt preferedWidth;
			UOSInt preferedHeight;

			static void __stdcall OnFrame(Data::Duration frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
			static void __stdcall OnFrameChange(Media::IVideoSource::FrameChange frChg, AnyType userData);
		public:
			OCVFrameFeeder(Media::OpenCV::OCVObjectDetector *frameInput, NotNullPtr<Media::IVideoSource> src);
			~OCVFrameFeeder();

			Bool Start();
			void SetFrameSkip(OSInt frameSkip);
			void SetPreferedFormat(UInt32 preferedFormat, UOSInt preferedWidth, UOSInt preferedHeight);
		};
	}
}
#endif
