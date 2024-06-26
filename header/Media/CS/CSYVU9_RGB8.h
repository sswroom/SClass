#ifndef _SM_MEDIA_CS_CSYVU9_RGB8
#define _SM_MEDIA_CS_CSYVU9_RGB8
#include "AnyType.h"
#include "Media/CS/CSYUV_RGB8.h"
#include "Sync/Event.h"

namespace Media
{
	namespace CS
	{
		class CSYVU9_RGB8 : public Media::CS::CSYUV_RGB8
		{
		private:
			typedef struct
			{
				NN<Sync::Event> evt;
				Int32 status; // 0 = not running, 1 = idling, 2 = toExit, 3 = converting, 4 = finished
				UnsafeArray<UInt8> yPtr;
				UnsafeArray<UInt8> uPtr;
				UnsafeArray<UInt8> vPtr;
				UnsafeArray<UInt8> dest;
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
			Sync::Event evtMain;
			UnsafeArray<THREADSTAT> stats;

			static UInt32 __stdcall WorkerThread(AnyType obj);
		public:
			CSYVU9_RGB8(NN<const Media::ColorProfile> srcColor, NN<const Media::ColorProfile> destColor, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess);
			virtual ~CSYVU9_RGB8();
			virtual void ConvertV2(UnsafeArray<UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual UOSInt GetSrcFrameSize(UOSInt width, UOSInt height);
		};
	}
}
#endif
