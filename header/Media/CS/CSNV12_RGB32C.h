#ifndef _SM_MEDIA_CS_CSNV12_RGB32C
#define _SM_MEDIA_CS_CSNV12_RGB32C
#include "AnyType.h"
#include "Media/CS/CSYUV_RGB32C.h"
#include "Sync/Event.h"

namespace Media
{
	namespace CS
	{
		class CSNV12_RGB32C : public Media::CS::CSYUV_RGB32C
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
				Sync::Event *evt;
				Int32 status; // 0 = not running, 1 = idling, 2 = toExit, 3 = converting, 4 = finished
				UInt8 *yPtr;
				UOSInt yBpl;
				UInt8 *uvPtr;
				UOSInt uvBpl;
				UInt8 *dest;
				UOSInt width;
				UOSInt height;
				UOSInt isFirst;
				UOSInt isLast;
				YVPARAMETER *yvParam;
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
			Sync::Event *evtMain;
			THREADSTAT *stats;

			static Double lanczos3_weight(Double phase);
			static void SetupInterpolationParameter(UOSInt source_length, UOSInt result_length, YVPARAMETER *out, UOSInt indexSep, Double offsetCorr);

			static UInt32 __stdcall WorkerThread(AnyType obj);
			void WaitForWorker(Int32 jobStatus);
		public:
			CSNV12_RGB32C(NotNullPtr<const Media::ColorProfile> srcProfile, NotNullPtr<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess, Media::PixelFormat destPF);
			virtual ~CSNV12_RGB32C();

			virtual void ConvertV2(UInt8 *const*srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual UOSInt GetSrcFrameSize(UOSInt width, UOSInt height);
		};
	}
}
#endif
