#ifndef _SM_MEDIA_VIDEOCAPTURE
#define _SM_MEDIA_VIDEOCAPTURE
#include "Media/RealtimeVideoSource.h"
#include "Text/StringBuilderUTF8.h"

namespace Media
{
	class VideoCapturer : public RealtimeVideoSource
	{
	public:
		typedef struct
		{
			Media::FrameInfo info;
			UInt32 frameRateNorm;
			UInt32 frameRateDenorm;
		} VideoFormat;
	public:
		virtual ~VideoCapturer() {}

		virtual void SetPreferSize(Math::Size2D<UOSInt> size, UInt32 fourcc, UInt32 bpp, UInt32 frameRateNumer, UInt32 frameRateDenom) = 0;
		virtual UOSInt GetSupportedFormats(VideoFormat *fmtArr, UOSInt maxCnt) = 0;
		virtual void GetInfo(NN<Text::StringBuilderUTF8> sb) = 0;
		virtual Bool IsVideoCapture() { return true; };
	};
}
#endif
