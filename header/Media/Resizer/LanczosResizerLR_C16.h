#ifndef _SM_MEDIA_RESIZER_LANCZOSRESIZERLR_C16
#define _SM_MEDIA_RESIZER_LANCZOSRESIZERLR_C16
#include "Media/IImgResizer.h"
#include "Media/ColorManager.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/ParallelTask.h"

namespace Media
{
	namespace Resizer
	{
		class LanczosResizerLR_C16 : public Media::IImgResizer, public Media::IColorHandler
		{
		private:
			typedef struct
			{
				LanczosResizerLR_C16 *me;
				Int32 funcType; // 3 = h filter, 5 = v filter, 7 = expand, 9 = collapse, 11 = copying
				Sync::Event *evt;
				const UInt8 *inPt;
				UInt8 *outPt;
				UOSInt width;
				UOSInt height;
				UOSInt tap;
				OSInt *index;
				Int64 *weight;
				OSInt sstep;
				OSInt dstep;
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
			UOSInt nThread;
			Sync::Mutex mut;
			TaskParam *params;
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
			UOSInt buffH;
			UInt8 *buffPtr;

			Media::ColorProfile destColor;
			Media::ColorManagerSess *colorSess;
			Double srcRefLuminance;
			Bool rgbChanged;
			UInt8 *rgbTable;

			void setup_interpolation_parameter(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, LRHPARAMETER *out, OSInt indexSep, Double offsetCorr);
			void setup_decimation_parameter(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, LRHPARAMETER *out, OSInt indexSep, Double offsetCorr);

			void mt_horizontal_filter(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep);
			void mt_vertical_filter(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep);
			void mt_collapse(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep);

			void UpdateRGBTable();

			static void __stdcall DoTask(void *obj);
			void DestoryHori();
			void DestoryVert();
		public:
			LanczosResizerLR_C16(UOSInt hnTap, UOSInt vnTap, NotNullPtr<const Media::ColorProfile> destColor, Media::ColorManagerSess *colorSess, Media::AlphaType srcAlphaType, Double srcRefLuminance);
			virtual ~LanczosResizerLR_C16();

			virtual void Resize(const UInt8 *src, OSInt sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UInt8 *dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight);
			virtual void YUVParamChanged(NotNullPtr<const Media::IColorHandler::YUVPARAM> yuvParam);
			virtual void RGBParamChanged(NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam);
			virtual void SetSrcRefLuminance(Double srcRefLuminance);

			virtual Bool IsSupported(NotNullPtr<const Media::FrameInfo> srcInfo);
			virtual Media::StaticImage *ProcessToNewPartial(NotNullPtr<const Media::RasterImage> srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR);
		};
	}
}
#endif
