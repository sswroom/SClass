//need vfw32.lib
#ifndef _SM_MEDIA_VFWVIDEOCAPTURE
#define _SM_MEDIA_VFWVIDEOCAPTURE
#include "Media/FrameInfo.h"

namespace Media
{
	class VFWVideoCapture
	{
	public:
		typedef void (__stdcall *FrameCallback)(UInt8 *imgData, UInt32 dataSize);
	private:
		Int32 devErr;
		void *hWnd;
		FrameCallback frameCB;

		static VFWVideoCapture *capObj;

		static Int32 __stdcall OnFrame(void *hWnd, void *lpVHdr);
	public:
		static Int32 GetDeviceCount();
		static WChar *GetDeviceName(WChar *buff, Int32 devNo);

		VFWVideoCapture(Int32 devNo);
		~VFWVideoCapture();
		Bool ShowDlgVideoSource();
		Bool ShowDlgVideoFormat();
		void GetCaptureFormat(Media::FrameInfo *info);
		void SetFrameRate(Int32 rate, Int32 scale);
		void StartCapture(FrameCallback cb);
		void StopCapture();
	};
};
#endif
