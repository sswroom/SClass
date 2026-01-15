#ifndef _SM_MEDIA_CS_CSY416_LRGBC
#define _SM_MEDIA_CS_CSY416_LRGBC
#include "AnyType.h"
#include "Media/CS/CSYUV16_LRGBC.h"
#include "Sync/Event.h"

namespace Media
{
	namespace CS
	{
		class CSY416_LRGBC : public Media::CS::CSYUV16_LRGBC
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
			CSY416_LRGBC(NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess);
			virtual ~CSY416_LRGBC();

			virtual void ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UIntOS dispWidth, UIntOS dispHeight, UIntOS srcStoreWidth, UIntOS srcStoreHeight, IntOS destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual UIntOS GetSrcFrameSize(UIntOS width, UIntOS height);
		};
	}
}
#endif
