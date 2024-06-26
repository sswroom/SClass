#ifndef _SM_MEDIA_CS_CSAYUV_RGB8
#define _SM_MEDIA_CS_CSAYUV_RGB8
#include "AnyType.h"
#include "Media/CS/CSYUV_RGB8.h"
#include "Sync/Event.h"

namespace Media
{
	namespace CS
	{
		class CSAYUV_RGB8 : public Media::CS::CSYUV_RGB8
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
			CSAYUV_RGB8(NN<const Media::ColorProfile> srcColor, NN<const Media::ColorProfile> destColor, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess);
			virtual ~CSAYUV_RGB8();
			virtual void ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual UOSInt GetSrcFrameSize(UOSInt width, UOSInt height);
		};
	}
}
#endif
