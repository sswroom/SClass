#ifndef _SM_MEDIA_CS_CSP216_LRGBC
#define _SM_MEDIA_CS_CSP216_LRGBC
#include "Media/CS/CSYUV16_LRGBC.h"
#include "Sync/Event.h"

namespace Media
{
	namespace CS
	{
		class CSP216_LRGBC : public Media::CS::CSYUV16_LRGBC
		{
		private:
			typedef struct
			{
				Sync::Event *evt;
				OSInt status; // 0 = not running, 1 = idling, 2 = toExit, 3 = converting, 4 = finished
				UInt8 *yPtr;
				UInt8 *uvPtr;
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
		public:
			CSP216_LRGBC(NotNullPtr<const Media::ColorProfile> srcProfile, NotNullPtr<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess);
			virtual ~CSP216_LRGBC();
			virtual void ConvertV2(UInt8 **srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual UOSInt GetSrcFrameSize(UOSInt width, UOSInt height);
		};
	}
}
#endif
