#ifndef _SM_MEDIA_MMALVIDEOCAPTURE
#define _SM_MEDIA_MMALVIDEOCAPTURE
#include "Data/ArrayList.hpp"
#include "Media/VideoCapturer.h"
#include "Text/StringBuilderUTF8.h"

namespace Media
{
	class MMALVideoCapture : public Media::VideoCapturer
	{
	public:
		struct ClassData;
	private:
		NN<ClassData> classData;

	public:
		MMALVideoCapture(Bool photoMode);
		virtual ~MMALVideoCapture();

		virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff);
		virtual Text::CString GetFilterName();

		virtual Bool GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UIntOS> maxFrameSize);
		virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, AnyType userData);
		virtual Bool Start();
		virtual void Stop();
		virtual Bool IsRunning();

		virtual void SetPreferSize(UIntOS width, UIntOS height, UInt32 fourcc, UInt32 bpp, UInt32 frameRateNumer, UInt32 frameRateDenom);
		virtual UIntOS GetSupportedFormats(UnsafeArray<VideoFormat> fmtArr, UIntOS maxCnt);
		virtual void GetInfo(NN<Text::StringBuilderUTF8> sb);

		virtual UIntOS GetDataSeekCount();

		static Bool IsAvailable();
	};
}
#endif
