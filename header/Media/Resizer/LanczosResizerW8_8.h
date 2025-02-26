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
				UOSInt swidth;
				UOSInt dwidth;
				UOSInt height;
				UOSInt tap;
				UnsafeArray<OSInt> index;
				UnsafeArray<Int16> weight;
				OSInt sstep;
				OSInt dstep;
				UInt8 *tmpbuff;
				UOSInt tmpbuffSize;
			} TaskParam;

			typedef struct
			{
				UOSInt length;
				Int16 *weight;
				OSInt *index;
				UOSInt tap;
			} LRHPARAMETER;

		private:
			UOSInt hnTap;
			UOSInt vnTap;
			Sync::Mutex mut;
			TaskParam *params;
			UOSInt nThread;
			Sync::ParallelTask *ptask;

			Double hsSize;
			Double hsOfst;
			UOSInt hdSize;
			OSInt *hIndex;
			Int16 *hWeight;
			UOSInt hTap;

			Double vsSize;
			Double vsOfst;
			UOSInt vdSize;
			OSInt vsStep;
			OSInt *vIndex;
			Int16 *vWeight;
			UOSInt vTap;

			UOSInt buffW;
			OSInt buffH;
			UInt8 *buffPtr;

			Media::PixelFormat srcPF;
			UnsafeArrayOpt<const UInt8> srcPal;
			Media::CS::TransferParam srcTran;
			Media::CS::TransferParam destTran;
			Optional<Media::ColorManagerSess> colorSess;
			Bool rgbChanged;
			UnsafeArrayOpt<UInt8> rgbTable;

			void setup_interpolation_parameter(UOSInt nTap, Double source_length, OSInt source_max_pos, UOSInt result_length, NN<LRHPARAMETER> out, OSInt indexSep, Double offsetCorr);
			void setup_decimation_parameter(UOSInt nTap, Double source_length, OSInt source_max_pos, UOSInt result_length, NN<LRHPARAMETER> out, OSInt indexSep, Double offsetCorr);
			void setup_interpolation_parameter_h(UOSInt nTap, Double source_length, OSInt source_max_pos, UOSInt result_length, NN<LRHPARAMETER> out, OSInt indexSep, Double offsetCorr);
			void setup_decimation_parameter_h(UOSInt nTap, Double source_length, OSInt source_max_pos, UOSInt result_length, NN<LRHPARAMETER> out, OSInt indexSep, Double offsetCorr);

			void mt_horizontal_filter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int16 *weight, OSInt sstep, OSInt dstep, UOSInt swidth);
			void mt_vertical_filter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int16 *weight, OSInt sstep, OSInt dstep);
			void mt_expand(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep);
			void mt_collapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep);
			void mt_copy(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep);

			void UpdateRGBTable();

			static void __stdcall DoTask(AnyType obj);
			void DestoryHori();
			void DestoryVert();
		public:
			LanczosResizerW8_8(UOSInt hnTap, UOSInt vnTap, NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Optional<Media::ColorManagerSess> colorSess);
			virtual ~LanczosResizerW8_8();

			virtual void Resize(UnsafeArray<const UInt8> src, OSInt sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UnsafeArray<UInt8> dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight);
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
