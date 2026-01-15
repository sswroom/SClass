#ifndef _SM_MEDIA_CS_CSY416_RGB32C
#define _SM_MEDIA_CS_CSY416_RGB32C
#include "AnyType.h"
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
				NN<Sync::Event> evt;
				Int32 status; // 0 = not running, 1 = idling, 2 = toExit, 3 = converting, 4 = finished
				UnsafeArray<UInt8> yPtr;
				UIntOS yBpl;
				UnsafeArray<UInt8> dest;
				UIntOS width;
				UIntOS height;
				IntOS dbpl;
			} THREADSTAT;

			UIntOS currId;
			UIntOS nThread;
			Sync::Event evtMain;
			UnsafeArray<THREADSTAT> stats;

			static UInt32 __stdcall WorkerThread(AnyType obj);
			void WaitForWorker(Int32 jobStatus);
		public:
			CSY416_RGB32C(NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess, Media::PixelFormat destPF);
			virtual ~CSY416_RGB32C();

			virtual void ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UIntOS dispWidth, UIntOS dispHeight, UIntOS srcStoreWidth, UIntOS srcStoreHeight, IntOS destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual UIntOS GetSrcFrameSize(UIntOS width, UIntOS height);
		};
	}
}
#endif
