#ifndef _SM_MEDIA_CS_CSYV12_RGB8
#define _SM_MEDIA_CS_CSYV12_RGB8
#include "AnyType.h"
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
				NN<Sync::Event> evt;
				Int32 status; // 0 = not running, 1 = idling, 2 = toExit, 3 = converting, 4 = finished, 5 = vfilter, 6 = yv42 conv
				UnsafeArray<UInt8> yPtr;
				IntOS yBpl;
				UnsafeArray<UInt8> uPtr;
				UnsafeArray<UInt8> vPtr;
				IntOS uvBpl;
				UnsafeArray<UInt8> dest;
				IntOS width;
				IntOS height;
				IntOS isFirst;
				IntOS isLast;
				NN<YVPARAMETER> yvParam;
				Media::YCOffset ycOfst;
				IntOS dbpl;
				IntOS csLineSize;
				UInt8 *csLineBuff;
				UInt8 *csNALineBuff;
				UInt8 *csLineBuff2;
				UInt8 *csNALineBuff2;
			} THREADSTAT;

			YVPARAMETER yvParamO;
			IntOS yvStepO;
			YVPARAMETER yvParamE;
			IntOS yvStepE;
			UInt8 *uBuff;
			UInt8 *uaBuff;
			UInt8 *vBuff;
			UInt8 *vaBuff;
			IntOS yvBuffSize;

			IntOS currId;
			IntOS nThread;
			Sync::Event evtMain;
			UnsafeArray<THREADSTAT> stats;

			static Double lanczos3_weight(Double phase);
			static void SetupInterpolationParameter(IntOS source_length, IntOS result_length, NN<YVPARAMETER> out, IntOS indexSep, Double offsetCorr);
			static void VerticalFilter(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, IntOS tap, Int32 *index, Int64 *weight, IntOS sstep, IntOS dstep);

			void do_yv12rgb8(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, IntOS width, IntOS height, IntOS dbpl, IntOS isFirst, IntOS isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, IntOS yBpl, IntOS uvBpl);
			void do_yv12rgb2(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, IntOS width, IntOS height, IntOS dbpl, IntOS isFirst, IntOS isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, IntOS yBpl, IntOS uvBpl);
			void do_yv12rgb8vc(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, IntOS width, IntOS height, IntOS dbpl, IntOS isFirst, IntOS isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, IntOS yBpl, IntOS uvBpl);
			void do_yv12rgb8vc2(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, IntOS width, IntOS height, IntOS dbpl, IntOS isFirst, IntOS isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, IntOS yBpl, IntOS uvBpl);
			static UInt32 __stdcall WorkerThread(AnyType obj);
			void WaitForWorker(Int32 jobStatus);
		public:
			CSYV12_RGB8(NN<const Media::ColorProfile> srcColor, NN<const Media::ColorProfile> destColor, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess);
			virtual ~CSYV12_RGB8();
			virtual void ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, IntOS dispWidth, IntOS dispHeight, IntOS srcStoreWidth, IntOS srcStoreHeight, IntOS destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual Int32 GetSrcFrameSize(Int32 width, Int32 height);
		};
	}
}
#endif
