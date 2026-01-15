#ifndef _SM_MEDIA_CS_CSNV12_LRGBC
#define _SM_MEDIA_CS_CSNV12_LRGBC
#include "AnyType.h"
#include "Media/CS/CSYUV_LRGBC.h"
#include "Sync/Event.h"

namespace Media
{
	namespace CS
	{
		class CSNV12_LRGBC : public Media::CS::CSYUV_LRGBC
		{
		protected:
			typedef struct
			{
				UIntOS length;
				Int64 *weight;
				IntOS *index;
				UIntOS tap;
			} YVPARAMETER;

			typedef struct
			{
				NN<Sync::Event> evt;
				Int32 status; // 0 = not running, 1 = idling, 2 = toExit, 3 = converting, 4 = finished
				UnsafeArray<UInt8> yPtr;
				UIntOS yBpl;
				UnsafeArray<UInt8> uvPtr;
				UIntOS uvBpl;
				UnsafeArray<UInt8> dest;
				UIntOS width;
				UIntOS height;
				UIntOS isFirst;
				UIntOS isLast;
				NN<YVPARAMETER> yvParam;
				Media::YCOffset ycOfst;
				IntOS dbpl;
				UIntOS csLineSize;
				UInt8 *csLineBuff;
				UInt8 *csLineBuff2;
			} THREADSTAT;

			YVPARAMETER yvParamO;
			UIntOS yvStepO;
			YVPARAMETER yvParamE;
			UIntOS yvStepE;
			UInt8 *uBuff;
			UInt8 *vBuff;
			UIntOS yvBuffSize;
			UInt8 *uvBuff;
			UIntOS uvBuffSize;

			UIntOS currId;
			UIntOS nThread;
			Sync::Event evtMain;
			UnsafeArray<THREADSTAT> stats;

			static Double lanczos3_weight(Double phase);
			static void SetupInterpolationParameter(UIntOS source_length, UIntOS result_length, NN<YVPARAMETER> out, UIntOS indexSep, Double offsetCorr);

			static UInt32 __stdcall WorkerThread(AnyType obj);
			void WaitForWorker(Int32 jobStatus);
		public:
			CSNV12_LRGBC(NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess);
			virtual ~CSNV12_LRGBC();

			virtual void ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UIntOS dispWidth, UIntOS dispHeight, UIntOS srcStoreWidth, UIntOS srcStoreHeight, IntOS destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual UIntOS GetSrcFrameSize(UIntOS width, UIntOS height);
		};
	}
}
#endif
