#ifndef _SM_MEDIA_RESIZER_LANCZOSRESIZERH13_8
#define _SM_MEDIA_RESIZER_LANCZOSRESIZERH13_8
#include "AnyType.h"
//MMX Accelerated Lanczos Resizer
namespace Media
{
	namespace Resizer
	{
		class LanczosResizerH13_8 : public Media::IImgResizer
		{
		private:
			typedef struct
			{
				Int32 status; // 0 = not running, 1 = waiting, 2 = to exit, 3 = h filter, 4 = end h filter, 5 = v filter, 6 = end v filter, 7 = expand, 8 = end expand, 9 = collapse, 10 = end collapse
				Sync::Event *evt;
				UInt8 *inPt;
				UInt8 *outPt;
				UOSInt width;
				UOSInt height;
				Int32 tap;
				Int32 *index;
				Int64 *weight;
				UInt32 sstep;
				UInt32 dstep;
			} LRH13THREADSTAT;

			typedef struct
			{
				UOSInt length;
				Int64 *weight;
				Int32 *index;
				Int32 tap;
			} LRH13PARAMETER;


		private:
			Int32 currId;
			Int32 nTap;
			Sync::Event *evtMain;
			LRH13THREADSTAT *stats;
			UOSInt nThread;

			Int32 hsSize;
			Int32 hdSize;
			Int32 *hIndex;
			Int64 *hWeight;
			Int32 hTap;

			Int32 vsSize;
			Int32 vdSize;
			Int32 *vIndex;
			Int64 *vWeight;
			Int32 vTap;

			Int32 buffW;
			Int32 buffH;
			UInt8 *buffPtr;

			Double lanczos3_weight(Double phase);
			void setup_interpolation_parameter(Int32 source_length, UOSInt result_length, LRH13PARAMETER *out, Int32 indexSep, Double offsetCorr);
			void setup_decimation_parameter(Int32 source_length, UOSInt result_length, LRH13PARAMETER *out, Int32 indexSep, Int32 offsetCorr);
			static void horizontal_filter(UInt8 *inPt, UInt8 *outPt,UOSInt width, UOSInt height, Int32 tap, Int32 *index, Int64 *weight, UInt32 sstep, UInt32 dstep);
			static void vertical_filter(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, Int32 tap, Int32 *index, Int64 *weight, UInt32 sstep, UInt32 dstep);
			static void expand(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, Int32 sstep, Int32 dstep);
			static void collapse(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, Int32 sstep, Int32 dstep);

			void mt_horizontal_filter(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, Int32 tap, Int32 *index, Int64 *weight, UInt32 sstep, UInt32 dstep);
			void mt_vertical_filter(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, Int32 tap, Int32 *index, Int64 *weight, UInt32 sstep, UInt32 dstep);
			void mt_expand(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, Int32 sstep, Int32 dstep);
			void mt_collapse(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, Int32 sstep, Int32 dstep);

			static UInt32 WorkerThread(AnyType obj);
			void DestoryHori();
			void DestoryVert();
		public:
			LanczosResizerH13_8(Int32 nTap);
			virtual ~LanczosResizerH13_8();

			virtual void Resize(UInt8 *src, Int32 sbpl, Int32 swidth, Int32 sheight, UInt8 *dest, Int32 dbpl, UOSInt dwidth, UOSInt dheight);
		};
	}
}
#endif
