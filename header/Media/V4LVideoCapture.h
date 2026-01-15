#ifndef _SM_MEDIA_V4LVIDEOCAPTURE
#define _SM_MEDIA_V4LVIDEOCAPTURE
#include "Data/ArrayListNative.hpp"
#include "Media/VideoCapturer.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

namespace Media
{
	class V4LVideoCapture : public Media::VideoCapturer
	{
	private:
		Int32 fd;
		UIntOS devId;
		Media::FrameInfo frameInfo;
		UInt8 *frameBuffs[4];
		UIntOS frameBuffSize;

		FrameCallback cb;
		FrameChangeCallback fcCb;
		AnyType userData;
		Sync::Thread thread;

		static void __stdcall PlayThread(NN<Sync::Thread> thread);
	public:
		V4LVideoCapture(UIntOS devId);
		virtual ~V4LVideoCapture();
		
		virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff);
		virtual Text::CStringNN GetFilterName();
		virtual Bool GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UIntOS> maxFrameSize);
		virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, AnyType userData);
		virtual Bool Start();
		virtual void Stop();
		virtual Bool IsRunning();

		virtual void SetPreferSize(Math::Size2D<UIntOS> size, UInt32 fourcc, UInt32 bpp, UInt32 frameRateNumer, UInt32 frameRateDenom);
		virtual UIntOS GetSupportedFormats(UnsafeArray<VideoFormat> fmtArr, UIntOS maxCnt);
		virtual void GetInfo(NN<Text::StringBuilderUTF8> sb);
		virtual UIntOS GetDataSeekCount();

		virtual UIntOS ReadFrame(UIntOS frameIndex, UnsafeArray<UInt8> buff);
		virtual Bool ReadFrameBegin();
		virtual Bool ReadFrameEnd();
	};

	class V4LVideoCaptureMgr
	{
	public:
		V4LVideoCaptureMgr();
		~V4LVideoCaptureMgr();

		UIntOS GetDeviceList(NN<Data::ArrayListNative<UInt32>> devList);
		UnsafeArrayOpt<UTF8Char> GetDeviceName(UnsafeArray<UTF8Char> buff, UIntOS devId);

		NN<Media::VideoCapturer> CreateDevice(UIntOS devId);
	};
}
#endif
