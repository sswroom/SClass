#ifndef _SM_MEDIA_RESIZER_LANCZOSRESIZER16_C8
#define _SM_MEDIA_RESIZER_LANCZOSRESIZER16_C8
#include "AnyType.h"
#include "UnsafeArray.h"
#include "Media/ImageResizer.h"
#include "Media/ColorManager.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/ParallelTask.h"

namespace Media
{
	namespace Resizer
	{
		class LanczosResizer16_C8 : public Media::ImageResizer, public Media::ColorHandler
		{
		private:
			typedef struct
			{
				Int32 funcType; // 3 = h filter, 5 = v filter, 7 = expand, 9 = collapse, 11 = copying, 12 = h filter pa, 13 = expand pa
				NN<LanczosResizer16_C8> me;
				UnsafeArray<const UInt8> inPt;
				UnsafeArray<UInt8> outPt;
				UIntOS swidth;
				UIntOS dwidth;
				UIntOS height;
				UIntOS tap;
				UnsafeArray<IntOS> index;
				UnsafeArray<Int64> weight;
				IntOS sstep;
				IntOS dstep;
				UnsafeArrayOpt<UInt8> tmpbuff;
				UIntOS tmpbuffSize;
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

			Media::ColorProfile srcProfile;
			Media::ColorProfile destProfile;
			Optional<Media::ColorManagerSess> colorSess;
			Bool rgbChanged;
			UnsafeArrayOpt<UInt8> rgbTable;

			void SetupInterpolationParameter(UIntOS nTap, Double source_length, UIntOS source_max_pos, UIntOS result_length, NN<LRHPARAMETER> out, IntOS indexSep, Double offsetCorr);
			void SetupDecimationParameter(UIntOS nTap, Double source_length, UIntOS source_max_pos, UIntOS result_length, NN<LRHPARAMETER> out, IntOS indexSep, Double offsetCorr);

			void MTHorizontalFilterPA(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, UIntOS tap, UnsafeArray<IntOS> index, UnsafeArray<Int64> weight, IntOS sstep, IntOS dstep, UIntOS swidth);
			void MTHorizontalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, UIntOS tap, UnsafeArray<IntOS> index, UnsafeArray<Int64> weight, IntOS sstep, IntOS dstep, UIntOS swidth);
			void MTVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, UIntOS tap, UnsafeArray<IntOS> index, UnsafeArray<Int64> weight, IntOS sstep, IntOS dstep);
			void MTExpandPA(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep);
			void MTExpand(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep);
			void MTCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep);
			void MTCopyPA(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep);
			void MTCopy(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep);

			void UpdateRGBTable();

			static void __stdcall DoTask(AnyType obj);
			void DestoryHori();
			void DestoryVert();
		public:
			LanczosResizer16_C8(UIntOS hnTap, UIntOS vnTap, NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Optional<Media::ColorManagerSess> colorSess, Media::AlphaType srcAlphaType);
			virtual ~LanczosResizer16_C8();

			virtual void Resize(UnsafeArray<const UInt8> src, IntOS sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UnsafeArray<UInt8> dest, IntOS dbpl, UIntOS dwidth, UIntOS dheight);
			virtual Bool Resize(NN<const Media::StaticImage> srcImg, NN<Media::StaticImage> destImg);
			virtual void YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam);
			virtual void RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam);
			void SetSrcProfile(NN<const Media::ColorProfile> srcProfile);
			void SetDestProfile(NN<const Media::ColorProfile> destProfile);
			Media::AlphaType GetDestAlphaType();
			virtual Bool IsSupported(NN<const Media::FrameInfo> srcInfo);
			virtual Optional<Media::StaticImage> ProcessToNewPartial(NN<const Media::RasterImage> srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR);
		};
	}
}
#endif
