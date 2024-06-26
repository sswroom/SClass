#ifndef _SM_MEDIA_CS_CSYUY2_LRGB
#define _SM_MEDIA_CS_CSYUY2_LRGB
#include "AnyType.h"
#include "Media/CS/CSYUV_LRGB.h"
#include "Sync/Event.h"

namespace Media
{
	namespace CS
	{
		class CSYUY2_LRGB : public Media::CS::CSYUV_LRGB
		{
		private:
			typedef struct
			{
				NN<Sync::Event> evt;
				Int32 status; // 0 = not running, 1 = idling, 2 = toExit, 3 = converting, 4 = finished
				UnsafeArray<UInt8> yPtr;
				UnsafeArray<UInt8> dest;
				OSInt width;
				OSInt height;
				OSInt dbpl;
			} THREADSTAT;

			OSInt currId;
			OSInt nThread;
			Sync::Event evtMain;
			UnsafeArray<THREADSTAT> stats;

			void do_yuy2rgb(UnsafeArray<UInt8> src, UnsafeArray<UInt8> dest, OSInt width, OSInt height, OSInt dbpl);
			static UInt32 __stdcall WorkerThread(AnyType obj);
		public:
			CSYUY2_LRGB(NN<Media::ColorProfile> srcColor, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess);
			virtual ~CSYUY2_LRGB();
			virtual void ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, OSInt dispWidth, OSInt dispHeight, OSInt srcStoreWidth, OSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual Int32 GetSrcFrameSize(Int32 width, Int32 height);
		};
	}
}
#endif
