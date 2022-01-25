#ifndef _SM_MEDIA_IVIDEOCAPTURE
#define _SM_MEDIA_IVIDEOCAPTURE
#include "Media/IRealtimeVideoSource.h"
#include "Text/StringBuilderUTF8.h"

namespace Media
{
	class IVideoCapture : public IRealtimeVideoSource
	{
	public:
		typedef struct
		{
			Media::FrameInfo info;
			UInt32 frameRateNorm;
			UInt32 frameRateDenorm;
		} VideoFormat;
	public:
		virtual ~IVideoCapture() {}

		virtual void SetPreferSize(UOSInt width, UOSInt height, UInt32 fourcc, UInt32 bpp, UInt32 frameRateNumer, UInt32 frameRateDenom) = 0;
		virtual UOSInt GetSupportedFormats(VideoFormat *fmtArr, UOSInt maxCnt) = 0;
		virtual void GetInfo(Text::StringBuilderUTF8 *sb) = 0;
		virtual Bool IsVideoCapture() { return true; };
	};
}
#endif
