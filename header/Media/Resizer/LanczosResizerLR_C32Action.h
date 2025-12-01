#ifndef _SM_MEDIA_RESIZER_LANCZOSRESIZERLR_C32ACTION
#define _SM_MEDIA_RESIZER_LANCZOSRESIZERLR_C32ACTION
#include "Media/FrameInfo.h"

namespace Media
{
	namespace Resizer
	{
		class LanczosResizerLR_C32Action
		{
		public:
			typedef struct
			{
			} HoriFilter;

			typedef struct
			{
			} VertFilter;
		public:
			virtual ~LanczosResizerLR_C32Action() {};

			virtual void DoHorizontalVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt sheight, UOSInt dheight, NN<HoriFilter> hfilter, NN<VertFilter> vfilter, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType) = 0;
			virtual void DoHorizontalFilterCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt sheight, NN<HoriFilter> hfilter, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType) = 0;
			virtual void DoVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt swidth, UOSInt sheight, UOSInt dheight, NN<VertFilter> vfilter, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType) = 0;
			virtual void DoCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt swidth, UOSInt sheight, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType) = 0;

			virtual void UpdateRGBTable(UnsafeArray<UInt8> rgbTable) = 0;
			virtual NN<HoriFilter> CreateHoriFilter(UOSInt htap, UnsafeArray<OSInt> hIndex, UnsafeArray<Int64> hWeight, UOSInt length) = 0;
			virtual void DestroyHoriFilter(NN<HoriFilter> hfilter) = 0;
			virtual NN<VertFilter> CreateVertFilter(UOSInt vtap, UnsafeArray<OSInt> vIndex, UnsafeArray<Int64> vWeight, UOSInt length) = 0;
			virtual void DestroyVertFilter(NN<VertFilter> vfilter) = 0;

			virtual Double GetHAvgTime() = 0;
			virtual Double GetVAvgTime() = 0;
		};
	}
}
#endif
