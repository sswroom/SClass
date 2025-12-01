#ifndef _SM_MEDIA_RESIZER_LANCZOSRESIZER16_C8
#define _SM_MEDIA_RESIZER_LANCZOSRESIZER16_C8
#include "AnyType.h"
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
				UOSInt swidth;
				UOSInt dwidth;
				UOSInt height;
				UOSInt tap;
				UnsafeArray<OSInt> index;
				UnsafeArray<Int64> weight;
				OSInt sstep;
				OSInt dstep;
				UnsafeArrayOpt<UInt8> tmpbuff;
				UOSInt tmpbuffSize;
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

			Media::ColorProfile srcProfile;
			Media::ColorProfile destProfile;
			Optional<Media::ColorManagerSess> colorSess;
			Bool rgbChanged;
			UnsafeArrayOpt<UInt8> rgbTable;

			void SetupInterpolationParameter(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, NN<LRHPARAMETER> out, OSInt indexSep, Double offsetCorr);
			void SetupDecimationParameter(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, NN<LRHPARAMETER> out, OSInt indexSep, Double offsetCorr);

			void MTHorizontalFilterPA(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, UOSInt tap, UnsafeArray<OSInt> index, UnsafeArray<Int64> weight, OSInt sstep, OSInt dstep, UOSInt swidth);
			void MTHorizontalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, UOSInt tap, UnsafeArray<OSInt> index, UnsafeArray<Int64> weight, OSInt sstep, OSInt dstep, UOSInt swidth);
			void MTVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, UOSInt tap, UnsafeArray<OSInt> index, UnsafeArray<Int64> weight, OSInt sstep, OSInt dstep);
			void MTExpandPA(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep);
			void MTExpand(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep);
			void MTCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep);
			void MTCopyPA(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep);
			void MTCopy(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep);

			void UpdateRGBTable();

			static void __stdcall DoTask(AnyType obj);
			void DestoryHori();
			void DestoryVert();
		public:
			LanczosResizer16_C8(UOSInt hnTap, UOSInt vnTap, NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Optional<Media::ColorManagerSess> colorSess, Media::AlphaType srcAlphaType);
			virtual ~LanczosResizer16_C8();

			virtual void Resize(UnsafeArray<const UInt8> src, OSInt sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UnsafeArray<UInt8> dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight);
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
