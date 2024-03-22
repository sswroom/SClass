#ifndef _SM_IO_DEVICE_QQZMSERIALCAMERA
#define _SM_IO_DEVICE_QQZMSERIALCAMERA
#include "AnyType.h"
#include "IO/Stream.h"

namespace IO
{
	namespace Device
	{
		class QQZMSerialCamera
		{
		private:
			IO::Stream *stm;
			UInt8 cameraId;
			Bool toRelease;

			Bool threadToStop;
			Bool threadRunning;

			UInt8 *imgBuff;
			UInt32 imgSize;
			Int32 imgPackets;
			UInt32 imgNextOfst;
			Int32 imgNextPacket;
			Bool imgEnd;
			Int64 imgLastUpdateTime;

			static UInt32 __stdcall RecvThread(AnyType userObj);
		public:
			QQZMSerialCamera(IO::Stream *stm, UInt8 cameraId, Bool toRelease);
			~QQZMSerialCamera();

			Bool CapturePhoto(IO::Stream *outStm);
		};
	}
}

#endif
