#ifndef _SM_MEDIA_CS_CSY416_RGB32C
#define _SM_MEDIA_CS_CSY416_RGB32C
#include "Media/CS/CSYUV16_RGB32C.h"
#include "Sync/Event.h"

namespace Media
{
	namespace CS
	{
		class CSY416_RGB32C : public Media::CS::CSYUV16_RGB32C
		{
		protected:
			typedef struct
			{
				Sync::Event *evt;
				Int32 status; // 0 = not running, 1 = idling, 2 = toExit, 3 = converting, 4 = finished
				UInt8 *yPtr;
				UOSInt yBpl;
				UInt8 *dest;
				UOSInt width;
				UOSInt height;
				OSInt dbpl;
			} THREADSTAT;

			UOSInt currId;
			UOSInt nThread;
			Sync::Event *evtMain;
			THREADSTAT *stats;

			static UInt32 __stdcall WorkerThread(void *obj);
			void WaitForWorker(Int32 jobStatus);
		public:
			CSY416_RGB32C(NotNullPtr<const Media::ColorProfile> srcProfile, NotNullPtr<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess, Media::PixelFormat destPF);
			virtual ~CSY416_RGB32C();

			virtual void ConvertV2(UInt8 *const*srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual UOSInt GetSrcFrameSize(UOSInt width, UOSInt height);
		};
	}
}
#endif
