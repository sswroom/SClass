#ifndef _SM_MEDIA_MMALVIDEOCAPTURE
#define _SM_MEDIA_MMALVIDEOCAPTURE
#include "Data/ArrayList.h"
#include "Media/VideoCapturer.h"
#include "Text/StringBuilderUTF8.h"

namespace Media
{
	class MMALVideoCapture : public Media::VideoCapturer
	{
	private:
		void *classData;

	public:
		MMALVideoCapture(Bool photoMode);
		virtual ~MMALVideoCapture();

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Text::CString GetFilterName();

		virtual Bool GetVideoInfo(Media::FrameInfo *info, UInt32 *frameRateNorm, UInt32 *frameRateDenorm, UOSInt *maxFrameSize);
		virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, void *userData);
		virtual Bool Start();
		virtual void Stop();
		virtual Bool IsRunning();

		virtual void SetPreferSize(UOSInt width, UOSInt height, UInt32 fourcc, UInt32 bpp, UInt32 frameRateNumer, UInt32 frameRateDenom);
		virtual UOSInt GetSupportedFormats(VideoFormat *fmtArr, UOSInt maxCnt);
		virtual void GetInfo(NN<Text::StringBuilderUTF8> sb);

		virtual UOSInt GetDataSeekCount();

		static Bool IsAvailable();
	};
}
#endif
