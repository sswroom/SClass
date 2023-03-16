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

			virtual void DoHorizontalVerticalFilter(const UInt8 *inPt, UInt8 *outPt, UOSInt dwidth, UOSInt sheight, UOSInt dheight, HoriFilter *hfilter, VertFilter *vfilter, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType) = 0;
			virtual void DoHorizontalFilterCollapse(const UInt8 *inPt, UInt8 *outPt, UOSInt dwidth, UOSInt sheight, HoriFilter *hfilter, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType) = 0;
			virtual void DoVerticalFilter(const UInt8 *inPt, UInt8 *outPt, UOSInt swidth, UOSInt sheight, UOSInt dheight, VertFilter *vfilter, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType) = 0;
			virtual void DoCollapse(const UInt8 *inPt, UInt8 *outPt, UOSInt swidth, UOSInt sheight, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType) = 0;

			virtual void UpdateRGBTable(UInt8 *rgbTable) = 0;
			virtual HoriFilter *CreateHoriFilter(UOSInt htap, OSInt *hIndex, Int64 *hWeight, UOSInt length) = 0;
			virtual void DestroyHoriFilter(HoriFilter *hfilter) = 0;
			virtual VertFilter *CreateVertFilter(UOSInt vtap, OSInt *vIndex, Int64 *vWeight, UOSInt length) = 0;
			virtual void DestroyVertFilter(VertFilter *vfilter) = 0;

			virtual Double GetHAvgTime() = 0;
			virtual Double GetVAvgTime() = 0;
		};
	}
}
#endif
