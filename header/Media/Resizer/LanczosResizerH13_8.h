#ifndef _SM_MEDIA_RESIZER_LANCZOSRESIZERH13_8
#define _SM_MEDIA_RESIZER_LANCZOSRESIZERH13_8
#include "AnyType.h"
//MMX Accelerated Lanczos Resizer
namespace Media
{
	namespace Resizer
	{
		class LanczosResizerH13_8 : public Media::ImageResizer
		{
		private:
			typedef struct
			{
				Int32 status; // 0 = not running, 1 = waiting, 2 = to exit, 3 = h filter, 4 = end h filter, 5 = v filter, 6 = end v filter, 7 = expand, 8 = end expand, 9 = collapse, 10 = end collapse
				NN<Sync::Event> evt;
				UnsafeArray<const UInt8> inPt;
				UnsafeArray<UInt8> outPt;
				UIntOS width;
				UIntOS height;
				Int32 tap;
				UnsafeArray<Int32> index;
				UnsafeArray<Int64> weight;
				UInt32 sstep;
				UInt32 dstep;
			} LRH13THREADSTAT;

			typedef struct
			{
				UIntOS length;
				UnsafeArray<Int64> weight;
				UnsafeArray<Int32> index;
				Int32 tap;
			} LRH13PARAMETER;


		private:
			Int32 currId;
			Int32 nTap;
			NN<Sync::Event> evtMain;
			UnsafeArray<LRH13THREADSTAT> stats;
			UIntOS nThread;

			Int32 hsSize;
			Int32 hdSize;
			UnsafeArrayOpt<Int32> hIndex;
			UnsafeArrayOpt<Int64> hWeight;
			Int32 hTap;

			Int32 vsSize;
			Int32 vdSize;
			UnsafeArrayOpt<Int32> vIndex;
			UnsafeArrayOpt<Int64> vWeight;
			Int32 vTap;

			Int32 buffW;
			Int32 buffH;
			UnsafeArrayOpt<UInt8> buffPtr;

			Double Lanczos3Weight(Double phase);
			void SetupInterpolationParameter(Int32 source_length, UIntOS result_length, NN<LRH13PARAMETER> out, Int32 indexSep, Double offsetCorr);
			void SetupDecimationParameter(Int32 source_length, UIntOS result_length, NN<LRH13PARAMETER> out, Int32 indexSep, Int32 offsetCorr);
			static void HorizontalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt,UIntOS width, UIntOS height, Int32 tap, UnsafeArray<Int32> index, UnsafeArray<Int64> weight, UInt32 sstep, UInt32 dstep);
			static void VerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, Int32 tap, UnsafeArray<Int32> index, UnsafeArray<Int64> weight, UInt32 sstep, UInt32 dstep);
			static void Expand(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, Int32 sstep, Int32 dstep);
			static void Collapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, Int32 sstep, Int32 dstep);

			void MTHorizontalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, Int32 tap, UnsafeArray<Int32> index, UnsafeArray<Int64> weight, UInt32 sstep, UInt32 dstep);
			void MTVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, Int32 tap, UnsafeArray<Int32> index, UnsafeArray<Int64> weight, UInt32 sstep, UInt32 dstep);
			void MTExpand(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, Int32 sstep, Int32 dstep);
			void MTCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, Int32 sstep, Int32 dstep);

			static UInt32 WorkerThread(AnyType obj);
			void DestoryHori();
			void DestoryVert();
		public:
			LanczosResizerH13_8(Int32 nTap);
			virtual ~LanczosResizerH13_8();

			virtual void Resize(UnsafeArray<const UInt8> src, Int32 sbpl, Int32 swidth, Int32 sheight, UnsafeArray<UInt8> dest, Int32 dbpl, UIntOS dwidth, UIntOS dheight);
		};
	}
}
#endif
