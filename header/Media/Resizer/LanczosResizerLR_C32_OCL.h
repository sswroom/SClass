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
			void *clsData;

		public:
			LanczosResizerLR_C32_OCL();
			virtual ~LanczosResizerLR_C32_OCL();

			virtual void DoHorizontalVerticalFilter(UInt8 *inPt, UInt8 *outPt, UOSInt dwidth, UOSInt sheight, UOSInt dheight, NN<HoriFilter> hfilter, NN<VertFilter> vfilter, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType);
			virtual void DoHorizontalFilterCollapse(UInt8 *inPt, UInt8 *outPt, UOSInt dwidth, UOSInt sheight, NN<HoriFilter> hfilter, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType);
			virtual void DoVerticalFilter(UInt8 *inPt, UInt8 *outPt, UOSInt swidth, UOSInt sheight, UOSInt dheight, NN<VertFilter> vfilter, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType);
			virtual void DoCollapse(UInt8 *inPt, UInt8 *outPt, UOSInt swidth, UOSInt sheight, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType);

			virtual void UpdateRGBTable(UInt8 *rgbTable);
			virtual NN<HoriFilter> CreateHoriFilter(OSInt htap, OSInt *hIndex, Int64 *hWeight, UOSInt length);
			virtual void DestroyHoriFilter(NN<HoriFilter> hfilter);
			virtual NN<VertFilter> CreateVertFilter(OSInt vtap, OSInt *vIndex, Int64 *vWeight, UOSInt length);
			virtual void DestroyVertFilter(NN<VertFilter> vfilter);

			virtual Double GetHAvgTime();
			virtual Double GetVAvgTime();
		};
	}
}
#endif
