#ifndef _SM_MEDIA_RESIZER_LANCZOSRESIZER8_8
#define _SM_MEDIA_RESIZER_LANCZOSRESIZER8_8
#include "AnyType.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Media/ImageResizer.h"

namespace Media
{
	namespace Resizer
	{
		class LanczosResizer8_8 : public Media::ImageResizer
		{
		private:
			typedef struct
			{
				Int32 status; // 0 = not running, 1 = waiting, 2 = to exit, 3 = h filter, 4 = end h filter, 5 = v filter, 6 = end v filter
				NN<Sync::Event> evt;
				UnsafeArray<const UInt8> inPt;
				UnsafeArray<UInt8> outPt;
				Int32 width;
				Int32 height;
				Int32 tap;
				UnsafeArray<Int32> index;
				UnsafeArray<Int32> weight;
				UInt32 sstep;
				UInt32 dstep;
			} LRTHREADSTAT;

			typedef struct
			{
				Int32 length;
				UnsafeArray<Int32> weight;
				UnsafeArray<Int32> index;
				Int32 tap;
			} PARAMETER;

		private:
			Int32 currId;
			Int32 nTap;
			Sync::Event evtMain;
			Sync::Mutex mut;
			UnsafeArray<LRTHREADSTAT> stats;
			UIntOS nThread;

			Double hsSize;
			Int32 hdSize;
			Double hsOfst;
			UnsafeArrayOpt<Int32> hIndex;
			UnsafeArrayOpt<Int32> hWeight;
			Int32 hTap;

			Double vsSize;
			Int32 vdSize;
			Double vsOfst;
			UnsafeArrayOpt<Int32> vIndex;
			UnsafeArrayOpt<Int32> vWeight;
			Int32 vTap;

			Int32 buffW;
			Int32 buffH;
			UnsafeArrayOpt<UInt8> buffPtr;

			Double Lanczos3Weight(Double phase);
			void SetupInterpolationParameter(Double source_length, Int32 source_max_pos, Int32 result_length, NN<PARAMETER> out, Int32 indexSep, Double offsetCorr);
			void SetupDecimationParameter(Double source_length, Int32 source_max_pos, Int32 result_length, NN<PARAMETER> out, Int32 indexSep, Double offsetCorr);
			static void HorizontalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt,Int32 width, Int32 height, Int32 tap, UnsafeArray<Int32> index, UnsafeArray<Int32> weight, UInt32 sstep, UInt32 dstep);
			static void VerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, Int32 width, Int32 height, Int32 tap, UnsafeArray<Int32> index, UnsafeArray<Int32> weight, UInt32 sstep, UInt32 dstep);

			void MTHorizontalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt,Int32 width, Int32 height, Int32 tap, UnsafeArray<Int32> index, UnsafeArray<Int32> weight, UInt32 sstep, UInt32 dstep);
			void MTVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, Int32 width, Int32 height, Int32 tap, UnsafeArray<Int32> index, UnsafeArray<Int32> weight, UInt32 sstep, UInt32 dstep);

			static UInt32 WorkerThread(AnyType obj);
			void DestoryHori();
			void DestoryVert();
		public:
			LanczosResizer8_8(Int32 nTap);
			virtual ~LanczosResizer8_8();

			virtual void Resize(UnsafeArray<const UInt8> src, IntOS sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UnsafeArray<UInt8> dest, IntOS dbpl, UIntOS dwidth, UIntOS dheight);

			virtual Bool IsSupported(NN<const Media::FrameInfo> srcInfo);
			virtual Optional<Media::StaticImage> ProcessToNewPartial(NN<const Media::RasterImage> srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR);
		};
	}
}
#endif
