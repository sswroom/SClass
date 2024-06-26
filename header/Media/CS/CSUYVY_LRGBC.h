#ifndef _SM_MEDIA_CS_CSUYVY_LRGBC
#define _SM_MEDIA_CS_CSUYVY_LRGBC
#include "AnyType.h"
#include "Media/CS/CSYUV_LRGBC.h"
#include "Sync/Event.h"

namespace Media
{
	namespace CS
	{
		class CSUYVY_LRGBC : public Media::CS::CSYUV_LRGBC
		{
		private:
			typedef struct
			{
				NN<Sync::Event> evt;
				OSInt status; // 0 = not running, 1 = idling, 2 = toExit, 3 = converting, 4 = finished
				UnsafeArray<UInt8> yPtr;
				UnsafeArray<UInt8> dest;
				UOSInt width;
				UOSInt height;
				OSInt dbpl;
			} THREADSTAT;

			UOSInt currId;
			UOSInt nThread;
			Sync::Event evtMain;
			UnsafeArray<THREADSTAT> stats;

			static UInt32 __stdcall WorkerThread(AnyType obj);
		public:
			CSUYVY_LRGBC(NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess);
			virtual ~CSUYVY_LRGBC();
			virtual void ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual UOSInt GetSrcFrameSize(UOSInt width, UOSInt height);
		};
	}
}
#endif
