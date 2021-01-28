#ifndef _SM_MEDIA_CS_CSYV12_RGB8
#define _SM_MEDIA_CS_CSYV12_RGB8
#include "Media/CS/CSYUV_RGB8.h"
#include "Sync/Event.h"

namespace Media
{
	namespace CS
	{
		class CSYV12_RGB8 : public Media::CS::CSYUV_RGB8
		{
		private:
			typedef struct
			{
				Int32 length;
				Int64 *weight;
				Int64 *uaweight;
				Int32 *index;
				Int32 tap;
			} YVPARAMETER;

			typedef struct
			{
				Sync::Event *evt;
				Int32 status; // 0 = not running, 1 = idling, 2 = toExit, 3 = converting, 4 = finished, 5 = vfilter, 6 = yv42 conv
				UInt8 *yPtr;
				OSInt yBpl;
				UInt8 *uPtr;
				UInt8 *vPtr;
				OSInt uvBpl;
				UInt8 *dest;
				OSInt width;
				OSInt height;
				OSInt isFirst;
				OSInt isLast;
				YVPARAMETER *yvParam;
				Media::YCOffset ycOfst;
				OSInt dbpl;
				OSInt csLineSize;
				UInt8 *csLineBuff;
				UInt8 *csNALineBuff;
				UInt8 *csLineBuff2;
				UInt8 *csNALineBuff2;
			} THREADSTAT;

			YVPARAMETER yvParamO;
			OSInt yvStepO;
			YVPARAMETER yvParamE;
			OSInt yvStepE;
			UInt8 *uBuff;
			UInt8 *uaBuff;
			UInt8 *vBuff;
			UInt8 *vaBuff;
			OSInt yvBuffSize;

			OSInt currId;
			OSInt nThread;
			Sync::Event *evtMain;
			THREADSTAT *stats;

			static Double lanczos3_weight(Double phase);
			static void SetupInterpolationParameter(OSInt source_length, OSInt result_length, YVPARAMETER *out, OSInt indexSep, Double offsetCorr);
			static void VerticalFilter(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, Int32 *index, Int64 *weight, OSInt sstep, OSInt dstep);

			void do_yv12rgb8(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, OSInt yBpl, OSInt uvBpl);
			void do_yv12rgb2(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, OSInt yBpl, OSInt uvBpl);
			void do_yv12rgb8vc(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, OSInt yBpl, OSInt uvBpl);
			void do_yv12rgb8vc2(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, OSInt yBpl, OSInt uvBpl);
			static UInt32 __stdcall WorkerThread(void *obj);
			void WaitForWorker(Int32 jobStatus);
		public:
			CSYV12_RGB8(const Media::ColorProfile *srcColor, const Media::ColorProfile *destColor, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess);
			virtual ~CSYV12_RGB8();
			virtual void ConvertV2(UInt8 **srcPtr, UInt8 *destPtr, OSInt dispWidth, OSInt dispHeight, OSInt srcStoreWidth, OSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual Int32 GetSrcFrameSize(Int32 width, Int32 height);
		};
	};
};
#endif
