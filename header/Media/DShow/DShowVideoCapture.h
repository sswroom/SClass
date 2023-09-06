#ifndef _SM_MEDIA_DSHOW_DSHOWVIDEOCAPTURE
#define _SM_MEDIA_DSHOW_DSHOWVIDEOCAPTURE
#include "Media/FrameInfo.h"
#include "Media/IVideoCapture.h"
//#include "Media/IRealTimeVideoSource.h"

#if defined(__GNUC__)
class IPin;
class IGraphBuilder;
class IBaseFilter;
class IPropertyBag;
#else
__interface IPin;
__interface IGraphBuilder;
__interface IBaseFilter;
__interface IPropertyBag;
#endif

namespace Media
{
	namespace DShow
	{
		class DShowVideoFilter;

		class DShowVideoCapture : public Media::IVideoCapture
		{
		private:
			IBaseFilter *baseFilter;
			IPropertyBag *pPropBag;
			DShowVideoFilter *captureFilter;
			IGraphBuilder *graph;
			IPin *pin1;
			IPin *pin2;
			FrameCallback cb;
			FrameChangeCallback fcCb;
			const WChar *devName;
			const WChar *displayName;

		public:
			DShowVideoCapture(IBaseFilter *baseFilter, IPropertyBag *pPropBag, const WChar *devName, const WChar *displayName);
			virtual ~DShowVideoCapture();

			virtual UTF8Char *GetSourceName(UTF8Char *buff);
			virtual Text::CString GetFilterName();

			virtual Bool GetVideoInfo(NotNullPtr<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize);
			virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, void *userData);
			virtual Bool Start();
			virtual void Stop();
			virtual Bool IsRunning();

			virtual UOSInt GetDataSeekCount();

			virtual Bool HasFrameCount();
			virtual UOSInt GetFrameCount();
			virtual UInt32 GetFrameTime(UOSInt frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, void *userData);

			virtual void SetPreferSize(Math::Size2D<UOSInt> size, UInt32 fourcc, UInt32 bpp, UInt32 frameRateNumer, UInt32 fraemRateDenom);
			virtual UOSInt GetSupportedFormats(VideoFormat *fmtArr, UOSInt maxCnt);
			virtual void GetInfo(NotNullPtr<Text::StringBuilderUTF8> sb);
		};

		class DShowVideoCaptureMgr
		{
		private:
			void *pEnum;
		public:
			DShowVideoCaptureMgr();
			~DShowVideoCaptureMgr();
			UOSInt GetDeviceCount();
			UTF8Char *GetDeviceName(UTF8Char *buff, UOSInt devNo);
			WChar *GetDeviceId(WChar *buff, UOSInt devNo);
			DShowVideoCapture *GetDevice(UOSInt devNo);
		};
	}
}
#endif
