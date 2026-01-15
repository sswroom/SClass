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
				UnsafeArrayOpt<Int64> weight;
				UnsafeArrayOpt<Int32> index;
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
				UnsafeArrayOpt<UInt8> csLineBuff;
				UnsafeArrayOpt<UInt8> csLineBuff2;
			} THREADSTAT;

			YVPARAMETER yvParamO;
			Int32 yvStepO;
			YVPARAMETER yvParamE;
			Int32 yvStepE;
			UnsafeArrayOpt<UInt8> uBuff;
			UnsafeArrayOpt<UInt8> vBuff;
			IntOS yvBuffSize;

			Int32 currId;
			UIntOS nThread;
			Sync::Event evtMain;
			UnsafeArray<THREADSTAT> stats;

			static Double Lanczos3Weight(Double phase);
			static void SetupInterpolationParameter(Int32 source_length, Int32 result_length, NN<YVPARAMETER> out, Int32 indexSep, Double offsetCorr);
			static void VerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, Int32 width, Int32 height, Int32 tap, UnsafeArray<Int32> index, UnsafeArray<Int64> weight, UInt32 sstep, UInt32 dstep);
			static void VerticalFilter16(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, Int32 width, Int32 height, Int32 tap, UnsafeArray<Int32> index, UnsafeArray<Int64> weight, UInt32 sstep, UInt32 dstep);

			void DoYV12RGB8(UnsafeArray<const UInt8> yPtr, UnsafeArray<const UInt8> uPtr, UnsafeArray<const UInt8> vPtr, UnsafeArray<UInt8> dest,
				Int32 width, Int32 height, Int32 dbpl, Int32 isFirst, Int32 isLast, UnsafeArray<UInt8> csLineBuff, UnsafeArray<UInt8> csLineBuff2, Int32 yBpl, Int32 uvBpl);
			void DoYV12RGB2(UnsafeArray<const UInt8> yPtr, UnsafeArray<const UInt8> uPtr, UnsafeArray<const UInt8> vPtr, UnsafeArray<UInt8> dest,
				Int32 width, Int32 height, Int32 dbpl, Int32 isFirst, Int32 isLast, UnsafeArray<UInt8> csLineBuff, UnsafeArray<UInt8> csLineBuff2, Int32 yBpl, Int32 uvBpl);
			void DoYV12RGB8vc2(UnsafeArray<const UInt8> yPtr, UnsafeArray<const UInt8> uPtr, UnsafeArray<const UInt8> vPtr, UnsafeArray<UInt8> dest,
				Int32 width, Int32 height, Int32 dbpl, Int32 isFirst, Int32 isLast, UnsafeArray<UInt8> csLineBuff, UnsafeArray<UInt8> csLineBuff2, Int32 yBpl, Int32 uvBpl);
			void DoYV12RGB8vc16(UnsafeArray<const UInt8> yPtr, UnsafeArray<const UInt8> uPtr, UnsafeArray<const UInt8> vPtr, UnsafeArray<UInt8> dest,
				Int32 width, Int32 height, Int32 dbpl, Int32 isFirst, Int32 isLast, UnsafeArray<UInt8> csLineBuff, UnsafeArray<UInt8> csLineBuff2, Int32 yBpl, Int32 uvBpl);
			static UInt32 __stdcall WorkerThread(AnyType obj);
			void WaitForWorker(Int32 jobStatus);
		public:
			CSYV12_LRGB(NN<const Media::ColorProfile> srcColor, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess);
			virtual ~CSYV12_LRGB();
			virtual void ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UIntOS dispWidth, UIntOS dispHeight, UIntOS srcStoreWidth, UIntOS srcStoreHeight, IntOS destLineAdd, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual Int32 GetSrcFrameSize(Int32 width, Int32 height);
		};
	}
}
#endif
