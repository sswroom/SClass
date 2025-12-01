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

			virtual void DoHorizontalVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt sheight, UOSInt dheight, NN<HoriFilter> hfilter, NN<VertFilter> vfilter, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType);
			virtual void DoHorizontalFilterCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt sheight, NN<HoriFilter> hfilter, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType);
			virtual void DoVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt swidth, UOSInt sheight, UOSInt dheight, NN<VertFilter> vfilter, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType);
			virtual void DoCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt swidth, UOSInt sheight, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType);

			virtual void UpdateRGBTable(UnsafeArray<UInt8> rgbTable);
			virtual NN<HoriFilter> CreateHoriFilter(OSInt htap, UnsafeArray<OSInt> hIndex, UnsafeArray<Int64> hWeight, UOSInt length);
			virtual void DestroyHoriFilter(NN<HoriFilter> hfilter);
			virtual NN<VertFilter> CreateVertFilter(OSInt vtap, UnsafeArray<OSInt> vIndex, UnsafeArray<Int64> vWeight, UOSInt length);
			virtual void DestroyVertFilter(NN<VertFilter> vfilter);

			virtual Double GetHAvgTime();
			virtual Double GetVAvgTime();
		};
	}
}
#endif
