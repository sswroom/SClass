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
				UIntOS width;
				UIntOS height;
				UIntOS width0;
				UIntOS tap;
				UnsafeArray<IntOS> index;
				UnsafeArray<Int64> weight;
				UIntOS tap2;
				UnsafeArray<IntOS> index2;
				UnsafeArray<Int64> weight2;
				UnsafeArray<UInt8> buffPt;
				IntOS sstep;
				IntOS dstep;
			} TaskParam;

			typedef struct
			{
				UIntOS length;
				UnsafeArray<Int64> weight;
				UnsafeArray<IntOS> index;
				IntOS tap;
			} LRHPARAMETER;

		private:
			UnsafeArray<TaskParam> params;
			NN<Sync::ParallelTask> ptask;
			UIntOS nThread;

			UnsafeArrayOpt<UInt8> rgbTable;
			UIntOS buffSize;
			UnsafeArrayOpt<UInt8> buffPtr;

			Double hTotTime;
			UIntOS hTotCount;
			Double vTotTime;
			UIntOS vTotCount;

			void mt_horizontal_filter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, UIntOS tap, UnsafeArray<IntOS> index, UnsafeArray<Int64> weight, IntOS sstep, IntOS dstep);
			void mt_vertical_filter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, UIntOS tap, UnsafeArray<IntOS> index, UnsafeArray<Int64> weight, IntOS sstep, IntOS dstep, Media::AlphaType srcAlphaType);
			void mt_hv_filter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS dwidth, UIntOS dheight, UIntOS swidth, UIntOS htap, UnsafeArray<IntOS> hindex, UnsafeArray<Int64> hweight, UIntOS vtap, UnsafeArray<IntOS> vindex, UnsafeArray<Int64> vweight, IntOS sstep, IntOS dstep, UnsafeArray<UInt8> buffPt, Media::AlphaType srcAlphaType);
			void mt_collapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep, Media::AlphaType srcAlphaType);

			static void __stdcall DoTask(AnyType obj);
		public:
			LanczosResizerLR_C32_CPU();
			virtual ~LanczosResizerLR_C32_CPU();

			virtual void DoHorizontalVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS dwidth, UIntOS sheight, UIntOS dheight, NN<HoriFilter> hfilter, NN<VertFilter> vfilter, IntOS sstep, IntOS dstep, Media::AlphaType srcAlphaType);
			virtual void DoHorizontalFilterCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS dwidth, UIntOS sheight, NN<HoriFilter> hfilter, IntOS sstep, IntOS dstep, Media::AlphaType srcAlphaType);
			virtual void DoVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS swidth, UIntOS sheight, UIntOS dheight, NN<VertFilter> vfilter, IntOS sstep, IntOS dstep, Media::AlphaType srcAlphaType);
			virtual void DoCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS swidth, UIntOS sheight, IntOS sstep, IntOS dstep, Media::AlphaType srcAlphaType);

			virtual void UpdateRGBTable(UnsafeArray<UInt8> rgbTable);
			virtual NN<HoriFilter> CreateHoriFilter(UIntOS htap, UnsafeArray<IntOS> hIndex, UnsafeArray<Int64> hWeight, UIntOS length);
			virtual void DestroyHoriFilter(NN<HoriFilter> hfilter);
			virtual NN<VertFilter> CreateVertFilter(UIntOS vtap, UnsafeArray<IntOS> vIndex, UnsafeArray<Int64> vWeight, UIntOS length);
			virtual void DestroyVertFilter(NN<VertFilter> vfilter);

			virtual Double GetHAvgTime();
			virtual Double GetVAvgTime();
		};
	}
}
#endif
