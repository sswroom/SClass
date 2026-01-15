#ifndef _SM_MEDIA_CS_CSNV12_RGB8
#define _SM_MEDIA_CS_CSNV12_RGB8
#include "AnyType.h"
#include "Media/CS/CSYUV_RGB8.h"
#include "Sync/Event.h"

namespace Media
{
	namespace CS
	{
		class CSNV12_RGB8 : public Media::CS::CSYUV_RGB8
		{
		private:
			typedef struct
			{
				NN<Sync::Event> evt;
				IntOS status; // 0 = not running, 1 = idling, 2 = toExit, 3 = converting, 4 = finished
				UnsafeArray<UInt8> yPtr;
				UnsafeArray<UInt8> uvPtr;
				UnsafeArray<UInt8> dest;
				UIntOS width;
				UIntOS height;
				UIntOS isFirst;
				UIntOS isLast;
				IntOS dbpl;
				UIntOS csLineSize;
				UInt8 *csLineBuff;
				UInt8 *csNALineBuff;
				UInt8 *csLineBuff2;
				UInt8 *csNALineBuff2;
			} THREADSTAT;

			UIntOS currId;
			UIntOS nThread;
			Sync::Event evtMain;
			UnsafeArray<THREADSTAT> stats;

			static UInt32 __stdcall WorkerThread(AnyType obj);
		public:
			CSNV12_RGB8(NN<const Media::ColorProfile> srcColor, NN<const Media::ColorProfile> destColor, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess);
			virtual ~CSNV12_RGB8();
			virtual void ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UIntOS dispWidth, UIntOS dispHeight, UIntOS srcStoreWidth, UIntOS srcStoreHeight, IntOS destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual UIntOS GetSrcFrameSize(UIntOS width, UIntOS height);
		};
	}
}
#endif
