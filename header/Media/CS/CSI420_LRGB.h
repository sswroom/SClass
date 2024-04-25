#ifndef _SM_MEDIA_CS_CSI420_LRGB
#define _SM_MEDIA_CS_CSI420_LRGB
#include "AnyType.h"
#include "Media/CS/CSYUV_LRGB.h"
#include "Sync/Event.h"

namespace Media
{
	namespace CS
	{
		class CSI420_LRGB : public Media::CS::CSYUV_LRGB
		{
		private:
			typedef struct
			{
				Sync::Event *evt;
				Int32 status; // 0 = not running, 1 = idling, 2 = toExit, 3 = converting, 4 = finished
				UInt8 *yPtr;
				UInt8 *uPtr;
				UInt8 *vPtr;
				UInt8 *dest;
				UOSInt width;
				UOSInt height;
				UOSInt isFirst;
				UOSInt isLast;
				OSInt dbpl;
				UOSInt csLineSize;
				UInt8 *csLineBuff;
				UInt8 *csLineBuff2;
			} THREADSTAT;

			UOSInt currId;
			UOSInt nThread;
			Sync::Event *evtMain;
			THREADSTAT *stats;

			static UInt32 __stdcall WorkerThread(AnyType obj);
		public:
			CSI420_LRGB(NN<const Media::ColorProfile> srcColor, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess);
			virtual ~CSI420_LRGB();
			virtual void ConvertV2(UInt8 *const*srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual UOSInt GetSrcFrameSize(UOSInt width, UOSInt height);
		};
	}
}
#endif
