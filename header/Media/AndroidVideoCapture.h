#ifndef _SM_MEDIA_ANDROIDVIDEOCAPTURE
#define _SM_MEDIA_ANDROIDVIDEOCAPTURE
#include "Data/ArrayList.hpp"
#include "Media/VideoCapturer.h"
#include "Text/StringBuilderUTF8.h"

namespace Media
{
	class AndroidVideoCapture : public Media::VideoCapturer
	{
	private:
		void *cameraMgr;
		const Char *cameraId;
		IntOS camWidth;
		IntOS camHeight;
		Int32 camFourcc;
	
		FrameCallback cb;
		FrameChangeCallback fcCb;
		void *userData;

		Bool started;
		void *session;
		void *captureRequest;
		void *outputs;
		void *output;
		void *device;
		void *reader;

		static UInt32 __stdcall PlayThread(void *userObj);
	public:
		AndroidVideoCapture(void *cameraMgr, const Char *cameraId);
		virtual ~AndroidVideoCapture();
		
		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Text::CString GetFilterName();
		virtual Bool GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UIntOS> maxFrameSize);
		virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, void *userData);
		virtual Bool Start();
		virtual void Stop();
		virtual Bool IsRunning();

		virtual void SetPreferSize(Math::Size2D<UIntOS> size, UInt32 fourcc, UInt32 bpp, UInt32 frameRateNumer, UInt32 frameRateDenom);
		virtual UIntOS GetSupportedFormats(VideoFormat *fmtArr, UIntOS maxCnt);
		virtual void GetInfo(NN<Text::StringBuilderUTF8> sb);
		virtual UIntOS GetDataSeekCount();
	};

	class AndroidVideoCaptureMgr
	{
	private:
		void *cameraMgr;
		void *cameraIdList;
	public:
		AndroidVideoCaptureMgr();
		~AndroidVideoCaptureMgr();

		UIntOS GetDeviceList(Data::ArrayList<UInt32> *devList);
		UTF8Char *GetDeviceName(UTF8Char *buff, UIntOS devId);

		Optional<Media::VideoCapturer> CreateDevice(UIntOS devId);
	};
}
#endif
