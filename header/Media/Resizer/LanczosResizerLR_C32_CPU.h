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
				LanczosResizerLR_C32_CPU *me;
				Int32 funcType; // 3 = h filter, 5 = v filter, 7 = expand, 9 = collapse, 11 = copying, 12 = v filter na, 13 = collapse na
				const UInt8 *inPt;
				UInt8 *outPt;
				UOSInt width;
				UOSInt height;
				UOSInt width0;
				UOSInt tap;
				OSInt *index;
				Int64 *weight;
				UOSInt tap2;
				OSInt *index2;
				Int64 *weight2;
				UInt8 *buffPt;
				OSInt sstep;
				OSInt dstep;
			} TaskParam;

			typedef struct
			{
				UOSInt length;
				Int64 *weight;
				OSInt *index;
				OSInt tap;
			} LRHPARAMETER;

		private:
			TaskParam *params;
			Sync::ParallelTask *ptask;
			UOSInt nThread;

			UInt8 *rgbTable;
			UOSInt buffSize;
			UInt8 *buffPtr;

			Double hTotTime;
			UOSInt hTotCount;
			Double vTotTime;
			UOSInt vTotCount;

			void mt_horizontal_filter(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep);
			void mt_vertical_filter(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType);
			void mt_hv_filter(const UInt8 *inPt, UInt8 *outPt, UOSInt dwidth, UOSInt dheight, UOSInt swidth, UOSInt htap, OSInt *hindex, Int64 *hweight, UOSInt vtap, OSInt *vindex, Int64 *vweight, OSInt sstep, OSInt dstep, UInt8 *buffPt, Media::AlphaType srcAlphaType);
			void mt_collapse(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType);

			static void __stdcall DoTask(AnyType obj);
		public:
			LanczosResizerLR_C32_CPU();
			virtual ~LanczosResizerLR_C32_CPU();

			virtual void DoHorizontalVerticalFilter(const UInt8 *inPt, UInt8 *outPt, UOSInt dwidth, UOSInt sheight, UOSInt dheight, HoriFilter *hfilter, VertFilter *vfilter, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType);
			virtual void DoHorizontalFilterCollapse(const UInt8 *inPt, UInt8 *outPt, UOSInt dwidth, UOSInt sheight, HoriFilter *hfilter, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType);
			virtual void DoVerticalFilter(const UInt8 *inPt, UInt8 *outPt, UOSInt swidth, UOSInt sheight, UOSInt dheight, VertFilter *vfilter, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType);
			virtual void DoCollapse(const UInt8 *inPt, UInt8 *outPt, UOSInt swidth, UOSInt sheight, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType);

			virtual void UpdateRGBTable(UInt8 *rgbTable);
			virtual HoriFilter *CreateHoriFilter(UOSInt htap, OSInt *hIndex, Int64 *hWeight, UOSInt length);
			virtual void DestroyHoriFilter(HoriFilter *hfilter);
			virtual VertFilter *CreateVertFilter(UOSInt vtap, OSInt *vIndex, Int64 *vWeight, UOSInt length);
			virtual void DestroyVertFilter(VertFilter *vfilter);

			virtual Double GetHAvgTime();
			virtual Double GetVAvgTime();
		};
	}
}
#endif
