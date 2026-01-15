#ifndef _SM_MEDIA_RESIZER_LANCZOSRESIZERW8_8
#define _SM_MEDIA_RESIZER_LANCZOSRESIZERW8_8
#include "AnyType.h"
#include "Media/ColorManager.h"
#include "Media/ImageResizer.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/ParallelTask.h"

namespace Media
{
	namespace Resizer
	{
		class LanczosResizerW8_8 : public Media::ImageResizer, public Media::ColorHandler
		{
		private:
			enum class FuncType
			{
				NoFunction,
				HFilter,
				VFilter,
				Expand,
				Collapse,
				ImgCopy
			};

			typedef struct
			{
				NN<LanczosResizerW8_8> me;
				FuncType funcType;
				UnsafeArray<const UInt8> inPt;
				UnsafeArray<UInt8> outPt;
				UIntOS swidth;
				UIntOS dwidth;
				UIntOS height;
				UIntOS tap;
				UnsafeArray<IntOS> index;
				UnsafeArray<Int16> weight;
				IntOS sstep;
				IntOS dstep;
				UnsafeArrayOpt<UInt8> tmpbuff;
				UIntOS tmpbuffSize;
			} TaskParam;

			typedef struct
			{
				UIntOS length;
				UnsafeArray<Int16> weight;
				UnsafeArray<IntOS> index;
				UIntOS tap;
			} LRHPARAMETER;

		private:
			UIntOS hnTap;
			UIntOS vnTap;
			Sync::Mutex mut;
			UnsafeArray<TaskParam> params;
			UIntOS nThread;
			NN<Sync::ParallelTask> ptask;

			Double hsSize;
			Double hsOfst;
			UIntOS hdSize;
			UnsafeArrayOpt<IntOS> hIndex;
			UnsafeArrayOpt<Int16> hWeight;
			UIntOS hTap;

			Double vsSize;
			Double vsOfst;
			UIntOS vdSize;
			IntOS vsStep;
			UnsafeArrayOpt<IntOS> vIndex;
			UnsafeArrayOpt<Int16> vWeight;
			UIntOS vTap;

			UIntOS buffW;
			IntOS buffH;
			UnsafeArrayOpt<UInt8> buffPtr;

			Media::PixelFormat srcPF;
			UnsafeArrayOpt<const UInt8> srcPal;
			Media::CS::TransferParam srcTran;
			Media::CS::TransferParam destTran;
			Optional<Media::ColorManagerSess> colorSess;
			Bool rgbChanged;
			UnsafeArrayOpt<UInt8> rgbTable;

			void SetupInterpolationParameterV(UIntOS nTap, Double source_length, IntOS source_max_pos, UIntOS result_length, NN<LRHPARAMETER> out, IntOS indexSep, Double offsetCorr);
			void SetupDecimationParameterV(UIntOS nTap, Double source_length, IntOS source_max_pos, UIntOS result_length, NN<LRHPARAMETER> out, IntOS indexSep, Double offsetCorr);
			void SetupInterpolationParameterH(UIntOS nTap, Double source_length, IntOS source_max_pos, UIntOS result_length, NN<LRHPARAMETER> out, IntOS indexSep, Double offsetCorr);
			void SetupDecimationParameterH(UIntOS nTap, Double source_length, IntOS source_max_pos, UIntOS result_length, NN<LRHPARAMETER> out, IntOS indexSep, Double offsetCorr);

			void MTHorizontalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, UIntOS tap, UnsafeArray<IntOS> index, UnsafeArray<Int16> weight, IntOS sstep, IntOS dstep, UIntOS swidth);
			void MTVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, UIntOS tap, UnsafeArray<IntOS> index, UnsafeArray<Int16> weight, IntOS sstep, IntOS dstep);
			void MTExpand(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep);
			void MTCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep);
			void MTCopy(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep);

			void UpdateRGBTable();

			static void __stdcall DoTask(AnyType obj);
			void DestoryHori();
			void DestoryVert();
		public:
			LanczosResizerW8_8(UIntOS hnTap, UIntOS vnTap, NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Optional<Media::ColorManagerSess> colorSess);
			virtual ~LanczosResizerW8_8();

			virtual void Resize(UnsafeArray<const UInt8> src, IntOS sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UnsafeArray<UInt8> dest, IntOS dbpl, UIntOS dwidth, UIntOS dheight);
			virtual Bool Resize(NN<const Media::StaticImage> srcImg, NN<Media::StaticImage> destImg);
			virtual void YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam);
			virtual void RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam);
			void SetSrcTransfer(NN<const Media::CS::TransferParam> srcTran);
			void SetDestTransfer(NN<const Media::CS::TransferParam> destTran);
			virtual Bool IsSupported(NN<const Media::FrameInfo> srcInfo);
			virtual Optional<Media::StaticImage> ProcessToNewPartial(NN<const Media::RasterImage> srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR);
		};
	}
}
#endif
