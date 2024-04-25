#ifndef _SM_MEDIA_ANDROIDVIDEOCAPTURE
#define _SM_MEDIA_ANDROIDVIDEOCAPTURE
#include "Data/ArrayList.h"
#include "Media/IVideoCapture.h"
#include "Text/StringBuilderUTF8.h"

namespace Media
{
	class AndroidVideoCapture : public Media::IVideoCapture
	{
	private:
		void *cameraMgr;
		const Char *cameraId;
		OSInt camWidth;
		OSInt camHeight;
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
		virtual Bool GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize);
		virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, void *userData);
		virtual Bool Start();
		virtual void Stop();
		virtual Bool IsRunning();

		virtual void SetPreferSize(Math::Size2D<UOSInt> size, UInt32 fourcc, UInt32 bpp, UInt32 frameRateNumer, UInt32 frameRateDenom);
		virtual UOSInt GetSupportedFormats(VideoFormat *fmtArr, UOSInt maxCnt);
		virtual void GetInfo(NN<Text::StringBuilderUTF8> sb);
		virtual UOSInt GetDataSeekCount();
	};

	class AndroidVideoCaptureMgr
	{
	private:
		void *cameraMgr;
		void *cameraIdList;
	public:
		AndroidVideoCaptureMgr();
		~AndroidVideoCaptureMgr();

		UOSInt GetDeviceList(Data::ArrayList<UInt32> *devList);
		UTF8Char *GetDeviceName(UTF8Char *buff, UOSInt devId);

		Media::IVideoCapture *CreateDevice(UOSInt devId);
	};
}
#endif
