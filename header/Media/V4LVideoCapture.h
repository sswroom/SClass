#ifndef _SM_MEDIA_V4LVIDEOCAPTURE
#define _SM_MEDIA_V4LVIDEOCAPTURE
#include "Data/ArrayList.h"
#include "Media/IVideoCapture.h"
#include "Text/StringBuilderUTF8.h"

namespace Media
{
	class V4LVideoCapture : public Media::IVideoCapture
	{
	private:
		Int32 fd;
		UOSInt devId;
		Media::FrameInfo frameInfo;
		UInt8 *frameBuffs[4];
		UOSInt frameBuffSize;

		FrameCallback cb;
		FrameChangeCallback fcCb;
		void *userData;
		Bool threadStarted;
		Bool threadRunning;
		Bool threadToStop;

		static UInt32 __stdcall PlayThread(void *userObj);
	public:
		V4LVideoCapture(UOSInt devId);
		virtual ~V4LVideoCapture();
		
		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Text::CString GetFilterName();
		virtual Bool GetVideoInfo(Media::FrameInfo *info, UInt32 *frameRateNorm, UInt32 *frameRateDenorm, UOSInt *maxFrameSize);
		virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, void *userData);
		virtual Bool Start();
		virtual void Stop();
		virtual Bool IsRunning();

		virtual void SetPreferSize(Math::Size2D<UOSInt> size, UInt32 fourcc, UInt32 bpp, UInt32 frameRateNumer, UInt32 frameRateDenom);
		virtual UOSInt GetSupportedFormats(VideoFormat *fmtArr, UOSInt maxCnt);
		virtual void GetInfo(NotNullPtr<Text::StringBuilderUTF8> sb);
		virtual UOSInt GetDataSeekCount();

		virtual UOSInt ReadFrame(UOSInt frameIndex, UInt8 *buff);
		virtual Bool ReadFrameBegin();
		virtual Bool ReadFrameEnd();
	};

	class V4LVideoCaptureMgr
	{
	public:
		V4LVideoCaptureMgr();
		~V4LVideoCaptureMgr();

		UOSInt GetDeviceList(Data::ArrayList<UInt32> *devList);
		UTF8Char *GetDeviceName(UTF8Char *buff, UOSInt devId);

		Media::IVideoCapture *CreateDevice(UOSInt devId);
	};
}
#endif
