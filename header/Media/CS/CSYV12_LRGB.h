#ifndef _SM_MEDIA_CS_CSYV12_LRGB
#define _SM_MEDIA_CS_CSYV12_LRGB
#include "AnyType.h"
#include "Media/CS/CSYUV_LRGBHQ.h"
#include "Sync/Event.h"

namespace Media
{
	namespace CS
	{
		class CSYV12_LRGB : public Media::CS::CSYUV_LRGBHQ
		{
		private:
			typedef struct
			{
				Int32 length;
				Int64 *weight;
				Int32 *index;
				Int32 tap;
			} YVPARAMETER;

			typedef struct
			{
				NN<Sync::Event> evt;
				Int32 status; // 0 = not running, 1 = idling, 2 = toExit, 3 = converting, 4 = finished
				UnsafeArray<UInt8> yPtr;
				Int32 yBpl;
				UnsafeArray<UInt8> uPtr;
				UnsafeArray<UInt8> vPtr;
				Int32 uvBpl;
				UnsafeArray<UInt8> dest;
				Int32 width;
				Int32 height;
				Int32 isFirst;
				Int32 isLast;
				NN<YVPARAMETER> yvParam;
				Media::YCOffset ycOfst;
				Int32 dbpl;
				Int32 csLineSize;
				UInt8 *csLineBuff;
				UInt8 *csLineBuff2;
			} THREADSTAT;

			YVPARAMETER yvParamO;
			Int32 yvStepO;
			YVPARAMETER yvParamE;
			Int32 yvStepE;
			UInt8 *uBuff;
			UInt8 *vBuff;
			OSInt yvBuffSize;

			Int32 currId;
			Int32 nThread;
			Sync::Event evtMain;
			UnsafeArray<THREADSTAT> stats;

			static Double lanczos3_weight(Double phase);
			static void SetupInterpolationParameter(Int32 source_length, Int32 result_length, NN<YVPARAMETER> out, Int32 indexSep, Double offsetCorr);
			static void VerticalFilter(UInt8 *inPt, UInt8 *outPt, Int32 width, Int32 height, Int32 tap, Int32 *index, Int64 *weight, UInt32 sstep, UInt32 dstep);
			static void VerticalFilter16(UInt8 *inPt, UInt8 *outPt, Int32 width, Int32 height, Int32 tap, Int32 *index, Int64 *weight, UInt32 sstep, UInt32 dstep);

			void do_yv12rgb8(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, Int32 width, Int32 height, Int32 dbpl, Int32 isFirst, Int32 isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int32 yBpl, Int32 uvBpl);
			void do_yv12rgb2(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, Int32 width, Int32 height, Int32 dbpl, Int32 isFirst, Int32 isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int32 yBpl, Int32 uvBpl);
			void do_yv12rgb8vc2(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, Int32 width, Int32 height, Int32 dbpl, Int32 isFirst, Int32 isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int32 yBpl, Int32 uvBpl);
			void do_yv12rgb8vc16(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, Int32 width, Int32 height, Int32 dbpl, Int32 isFirst, Int32 isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int32 yBpl, Int32 uvBpl);
			static UInt32 __stdcall WorkerThread(AnyType obj);
			void WaitForWorker(Int32 jobStatus);
		public:
			CSYV12_LRGB(NN<const Media::ColorProfile> srcColor, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess);
			virtual ~CSYV12_LRGB();
			virtual void ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, Int32 width, Int32 height, Int32 srcRGBBpl, Int32 destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual Int32 GetSrcFrameSize(Int32 width, Int32 height);
		};
	}
}
#endif
