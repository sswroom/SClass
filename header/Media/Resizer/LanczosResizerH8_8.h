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
				UIntOS width;
				UIntOS height;
				UIntOS tap;
				UnsafeArray<IntOS> index;
				UnsafeArray<Int64> weight;
				IntOS sstep;
				IntOS dstep;
			} TaskParam;

			typedef struct
			{
				UIntOS length;
				UnsafeArray<Int64> weight;
				UnsafeArray<IntOS> index;
				UIntOS tap;
			} LRHPARAMETER;

		private:
			UIntOS hnTap;
			UIntOS vnTap;
			UIntOS nThread;
			Sync::Mutex mut;
			UnsafeArray<TaskParam> params;
			NN<Sync::ParallelTask> ptask;

			Double hsSize;
			Double hsOfst;
			UIntOS hdSize;
			UnsafeArrayOpt<IntOS> hIndex;
			UnsafeArrayOpt<Int64> hWeight;
			UIntOS hTap;

			Double vsSize;
			Double vsOfst;
			UIntOS vdSize;
			IntOS vsStep;
			UnsafeArrayOpt<IntOS> vIndex;
			UnsafeArrayOpt<Int64> vWeight;
			UIntOS vTap;

			UIntOS buffW;
			UIntOS buffH;
			UnsafeArrayOpt<UInt8> buffPtr;

			Double hTime;
			Double vTime;

			void SetupInterpolationParameterV(UIntOS nTap, Double source_length, UIntOS source_max_pos, UIntOS result_length, NN<LRHPARAMETER> out, IntOS indexSep, Double offsetCorr);
			void SetupDecimationParameterV(UIntOS nTap, Double source_length, UIntOS source_max_pos, UIntOS result_length, NN<LRHPARAMETER> out, IntOS indexSep, Double offsetCorr);
			void SetupInterpolationParameterH(UIntOS nTap, Double source_length, UIntOS source_max_pos, UIntOS result_length, NN<LRHPARAMETER> out, IntOS indexSep, Double offsetCorr);
			void SetupDecimationParameterH(UIntOS nTap, Double source_length, UIntOS source_max_pos, UIntOS result_length, NN<LRHPARAMETER> out, IntOS indexSep, Double offsetCorr);

			void MTHorizontalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, UIntOS tap, UnsafeArray<IntOS> index, UnsafeArray<Int64> weight, IntOS sstep, IntOS dstep);
			void MTHorizontalFilter8(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, UIntOS tap, UnsafeArray<IntOS> index, UnsafeArray<Int64> weight, IntOS sstep, IntOS dstep);
			void MTVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, UIntOS tap, UnsafeArray<IntOS> index, UnsafeArray<Int64> weight, IntOS sstep, IntOS dstep);
			void MTExpand(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep);
			void MTCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep);
			void MTCopy(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep);

			static void __stdcall DoTask(AnyType obj);
			void DestoryHori();
			void DestoryVert();
		public:
			LanczosResizerH8_8(UIntOS hnTap, UIntOS vnTap, Media::AlphaType srcAlphaType);
			virtual ~LanczosResizerH8_8();

			virtual void Resize(UnsafeArray<const UInt8> src, IntOS sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UnsafeArray<UInt8> dest, IntOS dbpl, UIntOS dwidth, UIntOS dheight);
			virtual Bool Resize(NN<const Media::StaticImage> srcImg, NN<Media::StaticImage> destImg);

			virtual Bool IsSupported(NN<const Media::FrameInfo> srcInfo);
			virtual Optional<Media::StaticImage> ProcessToNewPartial(NN<const Media::RasterImage> srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR);

			Double GetHAvgTime();
			Double GetVAvgTime();
		};
	}
}
#endif
