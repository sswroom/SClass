#ifndef _SM_MEDIA_DSHOW_DSHOWVIDEOCAPTURE
#define _SM_MEDIA_DSHOW_DSHOWVIDEOCAPTURE
#include "Media/FrameInfo.h"
#include "Media/VideoCapturer.h"
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

		class DShowVideoCapture : public Media::VideoCapturer
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
			UnsafeArray<const WChar> devName;
			UnsafeArrayOpt<const WChar> displayName;

		public:
			DShowVideoCapture(IBaseFilter *baseFilter, IPropertyBag *pPropBag, const WChar *devName, const WChar *displayName);
			virtual ~DShowVideoCapture();

			virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff);
			virtual Text::CStringNN GetFilterName();

			virtual Bool GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UIntOS> maxFrameSize);
			virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, AnyType userData);
			virtual Bool Start();
			virtual void Stop();
			virtual Bool IsRunning();

			virtual UIntOS GetDataSeekCount();

			virtual Bool HasFrameCount();
			virtual UIntOS GetFrameCount();
			virtual Data::Duration GetFrameTime(UIntOS frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, AnyType userData);

			virtual void SetPreferSize(Math::Size2D<UIntOS> size, UInt32 fourcc, UInt32 bpp, UInt32 frameRateNumer, UInt32 fraemRateDenom);
			virtual UIntOS GetSupportedFormats(UnsafeArray<VideoFormat> fmtArr, UIntOS maxCnt);
			virtual void GetInfo(NN<Text::StringBuilderUTF8> sb);
		};

		class DShowVideoCaptureMgr
		{
		private:
			void *pEnum;
		public:
			DShowVideoCaptureMgr();
			~DShowVideoCaptureMgr();
			UIntOS GetDeviceCount();
			UnsafeArrayOpt<UTF8Char> GetDeviceName(UnsafeArray<UTF8Char> buff, UIntOS devNo);
			UnsafeArrayOpt<WChar> GetDeviceId(UnsafeArray<WChar> buff, UIntOS devNo);
			DShowVideoCapture *GetDevice(UIntOS devNo);
		};
	}
}
#endif
