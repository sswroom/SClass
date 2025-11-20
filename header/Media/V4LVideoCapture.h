#ifndef _SM_MEDIA_V4LVIDEOCAPTURE
#define _SM_MEDIA_V4LVIDEOCAPTURE
#include "Data/ArrayList.hpp"
#include "Media/VideoCapturer.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

namespace Media
{
	class V4LVideoCapture : public Media::VideoCapturer
	{
	private:
		Int32 fd;
		UOSInt devId;
		Media::FrameInfo frameInfo;
		UInt8 *frameBuffs[4];
		UOSInt frameBuffSize;

		FrameCallback cb;
		FrameChangeCallback fcCb;
		AnyType userData;
		Sync::Thread thread;

		static void __stdcall PlayThread(NN<Sync::Thread> thread);
	public:
		V4LVideoCapture(UOSInt devId);
		virtual ~V4LVideoCapture();
		
		virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff);
		virtual Text::CStringNN GetFilterName();
		virtual Bool GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize);
		virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, AnyType userData);
		virtual Bool Start();
		virtual void Stop();
		virtual Bool IsRunning();

		virtual void SetPreferSize(Math::Size2D<UOSInt> size, UInt32 fourcc, UInt32 bpp, UInt32 frameRateNumer, UInt32 frameRateDenom);
		virtual UOSInt GetSupportedFormats(VideoFormat *fmtArr, UOSInt maxCnt);
		virtual void GetInfo(NN<Text::StringBuilderUTF8> sb);
		virtual UOSInt GetDataSeekCount();

		virtual UOSInt ReadFrame(UOSInt frameIndex, UnsafeArray<UInt8> buff);
		virtual Bool ReadFrameBegin();
		virtual Bool ReadFrameEnd();
	};

	class V4LVideoCaptureMgr
	{
	public:
		V4LVideoCaptureMgr();
		~V4LVideoCaptureMgr();

		UOSInt GetDeviceList(Data::ArrayList<UInt32> *devList);
		UnsafeArrayOpt<UTF8Char> GetDeviceName(UnsafeArray<UTF8Char> buff, UOSInt devId);

		NN<Media::VideoCapturer> CreateDevice(UOSInt devId);
	};
}
#endif
