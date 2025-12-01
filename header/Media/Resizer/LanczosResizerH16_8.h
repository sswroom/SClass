#ifndef _SM_MEDIA_RESIZER_LANCZOSRESIZERH16_8
#define _SM_MEDIA_RESIZER_LANCZOSRESIZERH16_8
#include "AnyType.h"
//MMX Accelerated Lanczos Resizer
namespace Media
{
	namespace Resizer
	{
		class LanczosResizerH16_8 : public Media::ImageResizer
		{
		private:
			typedef struct
			{
				Int32 status; // 0 = not running, 1 = waiting, 2 = to exit, 3 = h filter, 4 = end h filter, 5 = v filter, 6 = end v filter, 7 = expand, 8 = end expand, 9 = collapse, 10 = end collapse
				NN<Sync::Event> evt;
				UnsafeArray<const UInt8> inPt;
				UnsafeArray<UInt8> outPt;
				UOSInt width;
				UOSInt height;
				Int32 tap;
				UnsafeArray<Int32> index;
				UnsafeArray<Int64> weight;
				UInt32 sstep;
				UInt32 dstep;
			} LRH16THREADSTAT;

			typedef struct
			{
				UOSInt length;
				UnsafeArray<Int64> weight;
				UnsafeArray<Int32> index;
				Int32 tap;
			} LRH16PARAMETER;

		private:
			Int32 currId;
			UOSInt hnTap;
			UOSInt vnTap;
			NN<Sync::Event> evtMain;
			UnsafeArray<LRH16THREADSTAT> stats;
			UOSInt nThread;

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

			Double Lanczos3Weight(Double phase, UOSInt nTap);
			void SetupInterpolationParameter(Int32 source_length, UOSInt result_length, NN<LRH16PARAMETER> out, Int32 indexSep, Double offsetCorr);
			void SetupDecimationParameter(Int32 source_length, UOSInt result_length, NN<LRH16PARAMETER> out, Int32 indexSep, Int32 offsetCorr);
			static void HorizontalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, Int32 tap, UnsafeArray<Int32> index, UnsafeArray<Int64> weight, UInt32 sstep, UInt32 dstep);
			static void VerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, Int32 tap, UnsafeArray<Int32> index, UnsafeArray<Int64> weight, UInt32 sstep, UInt32 dstep);
			static void Expand(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, Int32 sstep, Int32 dstep);
			static void Collapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, Int32 sstep, Int32 dstep);

			void MTHorizontalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, Int32 tap, UnsafeArray<Int32> index, UnsafeArray<Int64> weight, UInt32 sstep, UInt32 dstep);
			void MTVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, Int32 tap, UnsafeArray<Int32> index, UnsafeArray<Int64> weight, UInt32 sstep, UInt32 dstep);
			void MTExpand(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, Int32 sstep, Int32 dstep);
			void MTCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, Int32 sstep, Int32 dstep);

			static UInt32 WorkerThread(AnyType obj);
			void DestoryHori();
			void DestoryVert();
		public:
			LanczosResizerH16_8(UOSInt hnTap, UOSInt vnTap);
			virtual ~LanczosResizerH16_8();

			virtual void Resize(UnsafeArray<const UInt8> src, OSInt sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UnsafeArray<UInt8> dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight);
			virtual Bool Resize(NN<const Media::StaticImage> srcImage, NN<Media::StaticImage> destImage);

			virtual Bool IsSupported(NN<const Media::FrameInfo> srcInfo);
			virtual Optional<Media::StaticImage> ProcessToNewPartial(NN<const Media::RasterImage> srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR);

		};
	}
}
#endif
