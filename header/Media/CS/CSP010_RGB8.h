#ifndef _SM_MEDIA_CS_CSP010_RGB8
#define _SM_MEDIA_CS_CSP010_RGB8
#include "AnyType.h"
#include "Media/CS/CSYUV10_RGB8.h"
#include "Sync/Event.h"

namespace Media
{
	namespace CS
	{
		class CSP010_RGB8 : public Media::CS::CSYUV10_RGB8
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
				UOSInt isFirst;
				UOSInt isLast;
				OSInt dbpl;
				UOSInt csLineSize;
				UInt8 *csLineBuff;
				UInt8 *csNALineBuff;
				UInt8 *csLineBuff2;
				UInt8 *csNALineBuff2;
			} THREADSTAT;

			UOSInt currId;
			UOSInt nThread;
			Sync::Event *evtMain;
			THREADSTAT *stats;

			static UInt32 __stdcall WorkerThread(AnyType obj);
		public:
			CSP010_RGB8(NotNullPtr<const Media::ColorProfile> srcColor, NotNullPtr<const Media::ColorProfile> destColor, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess);
			virtual ~CSP010_RGB8();
			virtual void ConvertV2(UInt8 *const*srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual UOSInt GetSrcFrameSize(UOSInt width, UOSInt height);
		};
	}
}
#endif
