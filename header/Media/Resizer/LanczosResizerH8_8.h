#ifndef _SM_MEDIA_RESIZER_LANCZOSRESIZERH8_8
#define _SM_MEDIA_RESIZER_LANCZOSRESIZERH8_8
#include "AnyType.h"
#include "Media/ImageResizer.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/ParallelTask.h"

namespace Media
{
	namespace Resizer
	{
		class LanczosResizerH8_8 : public Media::ImageResizer
		{
		private:
			typedef struct
			{
				NN<LanczosResizerH8_8> me;
				Int32 funcType; // 3 = h filter, 5 = v filter, 7 = expand, 9 = collapse, 11 = copying, 13 = h filter
				UnsafeArray<const UInt8> inPt;
				UnsafeArray<UInt8> outPt;
				UOSInt width;
				UOSInt height;
				UOSInt tap;
				UnsafeArray<OSInt> index;
				UnsafeArray<Int64> weight;
				OSInt sstep;
				OSInt dstep;
			} TaskParam;

			typedef struct
			{
				UOSInt length;
				UnsafeArray<Int64> weight;
				UnsafeArray<OSInt> index;
				UOSInt tap;
			} LRHPARAMETER;

		private:
			UOSInt hnTap;
			UOSInt vnTap;
			UOSInt nThread;
			Sync::Mutex mut;
			UnsafeArray<TaskParam> params;
			NN<Sync::ParallelTask> ptask;

			Double hsSize;
			Double hsOfst;
			UOSInt hdSize;
			UnsafeArrayOpt<OSInt> hIndex;
			UnsafeArrayOpt<Int64> hWeight;
			UOSInt hTap;

			Double vsSize;
			Double vsOfst;
			UOSInt vdSize;
			OSInt vsStep;
			UnsafeArrayOpt<OSInt> vIndex;
			UnsafeArrayOpt<Int64> vWeight;
			UOSInt vTap;

			UOSInt buffW;
			UOSInt buffH;
			UnsafeArrayOpt<UInt8> buffPtr;

			Double hTime;
			Double vTime;

			void SetupInterpolationParameterV(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, NN<LRHPARAMETER> out, OSInt indexSep, Double offsetCorr);
			void SetupDecimationParameterV(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, NN<LRHPARAMETER> out, OSInt indexSep, Double offsetCorr);
			void SetupInterpolationParameterH(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, NN<LRHPARAMETER> out, OSInt indexSep, Double offsetCorr);
			void SetupDecimationParameterH(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, NN<LRHPARAMETER> out, OSInt indexSep, Double offsetCorr);

			void MTHorizontalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, UOSInt tap, UnsafeArray<OSInt> index, UnsafeArray<Int64> weight, OSInt sstep, OSInt dstep);
			void MTHorizontalFilter8(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, UOSInt tap, UnsafeArray<OSInt> index, UnsafeArray<Int64> weight, OSInt sstep, OSInt dstep);
			void MTVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, UOSInt tap, UnsafeArray<OSInt> index, UnsafeArray<Int64> weight, OSInt sstep, OSInt dstep);
			void MTExpand(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep);
			void MTCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep);
			void MTCopy(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep);

			static void __stdcall DoTask(AnyType obj);
			void DestoryHori();
			void DestoryVert();
		public:
			LanczosResizerH8_8(UOSInt hnTap, UOSInt vnTap, Media::AlphaType srcAlphaType);
			virtual ~LanczosResizerH8_8();

			virtual void Resize(UnsafeArray<const UInt8> src, OSInt sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UnsafeArray<UInt8> dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight);
			virtual Bool Resize(NN<const Media::StaticImage> srcImg, NN<Media::StaticImage> destImg);

			virtual Bool IsSupported(NN<const Media::FrameInfo> srcInfo);
			virtual Optional<Media::StaticImage> ProcessToNewPartial(NN<const Media::RasterImage> srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR);

			Double GetHAvgTime();
			Double GetVAvgTime();
		};
	}
}
#endif
