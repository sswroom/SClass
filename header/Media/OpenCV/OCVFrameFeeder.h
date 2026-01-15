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
			NN<OCVObjectDetector> frameInput;
			NN<Media::VideoSource> src;
			Optional<Media::VideoSource> decoder;
			Media::FrameInfo info;
			IntOS frameSkip;
			IntOS thisSkip;
			UInt32 preferedFormat;
			UIntOS preferedWidth;
			UIntOS preferedHeight;

			static void __stdcall OnFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst);
			static void __stdcall OnFrameChange(Media::VideoSource::FrameChange frChg, AnyType userData);
		public:
			OCVFrameFeeder(NN<OCVObjectDetector> frameInput, NN<Media::VideoSource> src);
			~OCVFrameFeeder();

			Bool Start();
			void SetFrameSkip(IntOS frameSkip);
			void SetPreferedFormat(UInt32 preferedFormat, UIntOS preferedWidth, UIntOS preferedHeight);
		};
	}
}
#endif
