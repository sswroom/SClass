#ifndef _SM_MEDIA_CS_CSYUV420_LRGBC
#define _SM_MEDIA_CS_CSYUV420_LRGBC
#include "AnyType.h"
#include "Media/CS/CSYUV_LRGBC.h"
#include "Sync/Event.h"

namespace Media
{
	namespace CS
	{
		class CSYUV420_LRGBC : public Media::CS::CSYUV_LRGBC
		{
		protected:
			enum class ThreadState
			{
				NotRunning,
				Idling,
				ToExit,
				YV12_RGB,
				Finished,
				VFilter
			}; // 0 = not running, 1 = idling, 2 = toExit, 3 = converting, 4 = finished

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
				ThreadState status;
				UInt8 *yPtr;
				UOSInt yBpl;
				UInt8 *uPtr;
				UInt8 *vPtr;
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
			Sync::Event evtMain;
			THREADSTAT *stats;

			static Double lanczos3_weight(Double phase);
			static void SetupInterpolationParameter(UOSInt source_length, UOSInt result_length, YVPARAMETER *out, UOSInt indexSep, Double offsetCorr);

			static UInt32 __stdcall WorkerThread(AnyType obj);
			void WaitForWorker(ThreadState jobStatus);
		public:
			CSYUV420_LRGBC(NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess);
			virtual ~CSYUV420_LRGBC();
			virtual void ConvertV2(UInt8 *const*srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst) = 0;
			virtual UOSInt GetSrcFrameSize(UOSInt width, UOSInt height);
		};
	}
}
#endif
