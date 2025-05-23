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
				UOSInt length;
				Int64 *weight;
				OSInt *index;
				UOSInt tap;
			} YVPARAMETER;

			typedef struct
			{
				NN<Sync::Event> evt;
				Int32 status; // 0 = not running, 1 = idling, 2 = toExit, 3 = converting, 4 = finished
				UnsafeArray<UInt8> yPtr;
				UOSInt yBpl;
				UnsafeArray<UInt8> uvPtr;
				UOSInt uvBpl;
				UnsafeArray<UInt8> dest;
				UOSInt width;
				UOSInt height;
				UOSInt isFirst;
				UOSInt isLast;
				NN<YVPARAMETER> yvParam;
				Media::YCOffset ycOfst;
				OSInt dbpl;
				UOSInt csLineSize;
				UInt8 *csLineBuff;
				UInt8 *csLineBuff2;
			} THREADSTAT;

			YVPARAMETER yvParamO;
			UOSInt yvStepO;
			YVPARAMETER yvParamE;
			UOSInt yvStepE;
			UInt8 *uBuff;
			UInt8 *vBuff;
			UOSInt yvBuffSize;
			UInt8 *uvBuff;
			UOSInt uvBuffSize;

			UOSInt currId;
			UOSInt nThread;
			Sync::Event evtMain;
			UnsafeArray<THREADSTAT> stats;

			static Double lanczos3_weight(Double phase);
			static void SetupInterpolationParameter(UOSInt source_length, UOSInt result_length, NN<YVPARAMETER> out, UOSInt indexSep, Double offsetCorr);

			static UInt32 __stdcall WorkerThread(AnyType obj);
			void WaitForWorker(Int32 jobStatus);
		public:
			CSNV12_LRGBC(NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess);
			virtual ~CSNV12_LRGBC();

			virtual void ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual UOSInt GetSrcFrameSize(UOSInt width, UOSInt height);
		};
	}
}
#endif
