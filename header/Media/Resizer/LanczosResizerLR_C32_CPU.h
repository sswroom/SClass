#ifndef _SM_MEDIA_RESIZER_LANCZOSRESIZERLR_C32_CPU
#define _SM_MEDIA_RESIZER_LANCZOSRESIZERLR_C32_CPU
#include "AnyType.h"
#include "Media/Resizer/LanczosResizerLR_C32Action.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/ParallelTask.h"

namespace Media
{
	namespace Resizer
	{
		class LanczosResizerLR_C32_CPU : public Media::Resizer::LanczosResizerLR_C32Action
		{
		protected:
			typedef struct
			{
				NN<LanczosResizerLR_C32_CPU> me;
				Int32 funcType; // 3 = h filter, 5 = v filter, 7 = expand, 9 = collapse, 11 = copying, 12 = v filter na, 13 = collapse na
				UnsafeArray<const UInt8> inPt;
				UnsafeArray<UInt8> outPt;
				UOSInt width;
				UOSInt height;
				UOSInt width0;
				UOSInt tap;
				UnsafeArray<OSInt> index;
				UnsafeArray<Int64> weight;
				UOSInt tap2;
				UnsafeArray<OSInt> index2;
				UnsafeArray<Int64> weight2;
				UnsafeArray<UInt8> buffPt;
				OSInt sstep;
				OSInt dstep;
			} TaskParam;

			typedef struct
			{
				UOSInt length;
				UnsafeArray<Int64> weight;
				UnsafeArray<OSInt> index;
				OSInt tap;
			} LRHPARAMETER;

		private:
			UnsafeArray<TaskParam> params;
			NN<Sync::ParallelTask> ptask;
			UOSInt nThread;

			UnsafeArrayOpt<UInt8> rgbTable;
			UOSInt buffSize;
			UnsafeArrayOpt<UInt8> buffPtr;

			Double hTotTime;
			UOSInt hTotCount;
			Double vTotTime;
			UOSInt vTotCount;

			void mt_horizontal_filter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, UOSInt tap, UnsafeArray<OSInt> index, UnsafeArray<Int64> weight, OSInt sstep, OSInt dstep);
			void mt_vertical_filter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, UOSInt tap, UnsafeArray<OSInt> index, UnsafeArray<Int64> weight, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType);
			void mt_hv_filter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt dheight, UOSInt swidth, UOSInt htap, UnsafeArray<OSInt> hindex, UnsafeArray<Int64> hweight, UOSInt vtap, UnsafeArray<OSInt> vindex, UnsafeArray<Int64> vweight, OSInt sstep, OSInt dstep, UnsafeArray<UInt8> buffPt, Media::AlphaType srcAlphaType);
			void mt_collapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType);

			static void __stdcall DoTask(AnyType obj);
		public:
			LanczosResizerLR_C32_CPU();
			virtual ~LanczosResizerLR_C32_CPU();

			virtual void DoHorizontalVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt sheight, UOSInt dheight, NN<HoriFilter> hfilter, NN<VertFilter> vfilter, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType);
			virtual void DoHorizontalFilterCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt sheight, NN<HoriFilter> hfilter, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType);
			virtual void DoVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt swidth, UOSInt sheight, UOSInt dheight, NN<VertFilter> vfilter, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType);
			virtual void DoCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt swidth, UOSInt sheight, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType);

			virtual void UpdateRGBTable(UnsafeArray<UInt8> rgbTable);
			virtual NN<HoriFilter> CreateHoriFilter(UOSInt htap, UnsafeArray<OSInt> hIndex, UnsafeArray<Int64> hWeight, UOSInt length);
			virtual void DestroyHoriFilter(NN<HoriFilter> hfilter);
			virtual NN<VertFilter> CreateVertFilter(UOSInt vtap, UnsafeArray<OSInt> vIndex, UnsafeArray<Int64> vWeight, UOSInt length);
			virtual void DestroyVertFilter(NN<VertFilter> vfilter);

			virtual Double GetHAvgTime();
			virtual Double GetVAvgTime();
		};
	}
}
#endif
