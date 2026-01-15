#ifndef _SM_MEDIA_RESIZER_LANCZOSRESIZERLR_C32_OCL
#define _SM_MEDIA_RESIZER_LANCZOSRESIZERLR_C32_OCL
#include "Media/Resizer/LanczosResizerLR_C32Action.h"

namespace Media
{
	namespace Resizer
	{
		class LanczosResizerLR_C32_OCL : public Media::Resizer::LanczosResizerLR_C32Action
		{
		private:
			struct ClassData;
			NN<ClassData> clsData;

		public:
			LanczosResizerLR_C32_OCL();
			virtual ~LanczosResizerLR_C32_OCL();

			virtual void DoHorizontalVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS dwidth, UIntOS sheight, UIntOS dheight, NN<HoriFilter> hfilter, NN<VertFilter> vfilter, IntOS sstep, IntOS dstep, Media::AlphaType srcAlphaType);
			virtual void DoHorizontalFilterCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS dwidth, UIntOS sheight, NN<HoriFilter> hfilter, IntOS sstep, IntOS dstep, Media::AlphaType srcAlphaType);
			virtual void DoVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS swidth, UIntOS sheight, UIntOS dheight, NN<VertFilter> vfilter, IntOS sstep, IntOS dstep, Media::AlphaType srcAlphaType);
			virtual void DoCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS swidth, UIntOS sheight, IntOS sstep, IntOS dstep, Media::AlphaType srcAlphaType);

			virtual void UpdateRGBTable(UnsafeArray<UInt8> rgbTable);
			virtual NN<HoriFilter> CreateHoriFilter(IntOS htap, UnsafeArray<IntOS> hIndex, UnsafeArray<Int64> hWeight, UIntOS length);
			virtual void DestroyHoriFilter(NN<HoriFilter> hfilter);
			virtual NN<VertFilter> CreateVertFilter(IntOS vtap, UnsafeArray<IntOS> vIndex, UnsafeArray<Int64> vWeight, UIntOS length);
			virtual void DestroyVertFilter(NN<VertFilter> vfilter);

			virtual Double GetHAvgTime();
			virtual Double GetVAvgTime();
		};
	}
}
#endif
