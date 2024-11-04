#ifndef _SM_MEDIA_RESIZER_LANCZOSRESIZER8_C8
#define _SM_MEDIA_RESIZER_LANCZOSRESIZER8_C8
#include "AnyType.h"
#include "Media/ColorManager.h"
#include "Media/IImgResizer.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/ParallelTask.h"

namespace Media
{
	namespace Resizer
	{
		class LanczosResizer8_C8 : public Media::IImgResizer, public Media::IColorHandler
		{
		private:
			enum class FuncType
			{
				NoFunction,
				HFilter,
				VFilter,
				Expand,
				Collapse,
				ImgCopy,
				HFilterPA,
				ExpandPA,
				ImgCopyPA
			};

			typedef struct
			{
				NN<LanczosResizer8_C8> me;
				FuncType funcType; // 3 = h filter, 5 = v filter, 7 = expand, 9 = collapse, 11 = copying, 12 = h filter pa, 13 = expand pa
				UnsafeArray<const UInt8> inPt;
				UnsafeArray<UInt8> outPt;
				UOSInt swidth;
				UOSInt dwidth;
				UOSInt height;
				UOSInt tap;
				OSInt *index;
				Int64 *weight;
				OSInt sstep;
				OSInt dstep;
				UInt8 *tmpbuff;
				UOSInt tmpbuffSize;
			} TaskParam;

			typedef struct
			{
				UOSInt length;
				Int64 *weight;
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
			Int64 *hWeight;
			UOSInt hTap;

			Double vsSize;
			Double vsOfst;
			UOSInt vdSize;
			OSInt vsStep;
			OSInt *vIndex;
			Int64 *vWeight;
			UOSInt vTap;

			UOSInt buffW;
			OSInt buffH;
			UInt8 *buffPtr;

			Media::ColorProfile srcProfile;
			Media::ColorProfile destProfile;
			Optional<Media::ColorManagerSess> colorSess;
			Bool rgbChanged;
			UnsafeArrayOpt<UInt8> rgbTable;

			void setup_interpolation_parameter(UOSInt nTap, Double source_length, OSInt source_max_pos, UOSInt result_length, LRHPARAMETER *out, OSInt indexSep, Double offsetCorr);
			void setup_decimation_parameter(UOSInt nTap, Double source_length, OSInt source_max_pos, UOSInt result_length, LRHPARAMETER *out, OSInt indexSep, Double offsetCorr);
			void setup_interpolation_parameter_h(UOSInt nTap, Double source_length, OSInt source_max_pos, UOSInt result_length, LRHPARAMETER *out, OSInt indexSep, Double offsetCorr);
			void setup_decimation_parameter_h(UOSInt nTap, Double source_length, OSInt source_max_pos, UOSInt result_length, LRHPARAMETER *out, OSInt indexSep, Double offsetCorr);

			void mt_horizontal_filter_pa(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt,UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UOSInt swidth);
			void mt_horizontal_filter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UOSInt swidth);
			void mt_vertical_filter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep);
			void mt_expand_pa(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep);
			void mt_expand(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep);
			void mt_collapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep);
			void mt_copy_pa(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep);
			void mt_copy(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep);

			void UpdateRGBTable();

			static void __stdcall DoTask(AnyType obj);
			void DestoryHori();
			void DestoryVert();
		public:
			LanczosResizer8_C8(UOSInt hnTap, UOSInt vnTap, NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Optional<Media::ColorManagerSess> colorSess, Media::AlphaType srcAlphaType);
			virtual ~LanczosResizer8_C8();

			virtual void Resize(UnsafeArray<const UInt8> src, OSInt sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UnsafeArray<UInt8> dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight);
			virtual Bool Resize(NN<const Media::StaticImage> srcImg, NN<Media::StaticImage> destImg);
			virtual void YUVParamChanged(NN<const Media::IColorHandler::YUVPARAM> yuvParam);
			virtual void RGBParamChanged(NN<const Media::IColorHandler::RGBPARAM2> rgbParam);
			void SetSrcProfile(NN<const Media::ColorProfile> srcProfile);
			void SetDestProfile(NN<const Media::ColorProfile> destProfile);
			Media::AlphaType GetDestAlphaType();
			virtual Bool IsSupported(NN<const Media::FrameInfo> srcInfo);
			virtual Media::StaticImage *ProcessToNewPartial(NN<const Media::RasterImage> srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR);
		};
	}
}
#endif
