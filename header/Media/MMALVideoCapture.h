#ifndef _SM_MEDIA_MMALVIDEOCAPTURE
#define _SM_MEDIA_MMALVIDEOCAPTURE
#include "Data/ArrayList.h"
#include "Media/IVideoCapture.h"
#include "Text/StringBuilderUTF.h"

namespace Media
{
	class MMALVideoCapture : public Media::IVideoCapture
	{
	private:
		void *classData;

	public:
		MMALVideoCapture(Bool photoMode);
		virtual ~MMALVideoCapture();

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual const UTF8Char *GetFilterName();

		virtual Bool GetVideoInfo(Media::FrameInfo *info, Int32 *frameRateNorm, Int32 *frameRateDenorm, UOSInt *maxFrameSize);
		virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, void *userData);
		virtual Bool Start();
		virtual void Stop();
		virtual Bool IsRunning();

		virtual void SetPreferSize(UOSInt width, UOSInt height, UInt32 fourcc, UInt32 bpp, Int32 frameRateNumer, Int32 frameRateDenom);
		virtual UOSInt GetSupportedFormats(VideoFormat *fmtArr, UOSInt maxCnt);
		virtual void GetInfo(Text::StringBuilderUTF *sb);

		virtual OSInt GetDataSeekCount();

		static Bool IsAvailable();
	};
}
#endif
