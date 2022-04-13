#ifndef _SM_MEDIA_RESIZER_LANCZOSRESIZERLR_C32
#define _SM_MEDIA_RESIZER_LANCZOSRESIZERLR_C32
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Media/IImgResizer.h"
#include "Media/ColorManager.h"
#include "Media/Resizer/LanczosResizerLR_C32Action.h"

namespace Media
{
	namespace Resizer
	{
		class LanczosResizerLR_C32 : public Media::IImgResizer, public Media::IColorHandler
		{
		protected:
			typedef struct
			{
				UOSInt length;
				Int64 *weight;
				OSInt *index;
				UOSInt tap;
			} LRHPARAMETER;

		protected:
			Int32 currId;
			UOSInt hnTap;
			UOSInt vnTap;
			Sync::Mutex mut;
			Media::Resizer::LanczosResizerLR_C32Action *action;

			Double hsSize;
			Double hsOfst;
			UOSInt hdSize;
			OSInt *hIndex;
			Int64 *hWeight;
			UOSInt hTap;
			Media::Resizer::LanczosResizerLR_C32Action::HoriFilter *hFilter;

			Double vsSize;
			Double vsOfst;
			UOSInt vdSize;
			OSInt vsStep;
			OSInt *vIndex;
			Int64 *vWeight;
			UOSInt vTap;
			Media::Resizer::LanczosResizerLR_C32Action::VertFilter *vFilter;

			Media::ColorProfile destColor;
			Media::ColorManagerSess *colorSess;
			Double srcRefLuminance;
			Bool rgbChanged;
			UInt8 *rgbTable;
			Media::PixelFormat pf;

			static void setup_interpolation_parameter(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, LRHPARAMETER *out, OSInt indexSep, Double offsetCorr);
			static void setup_decimation_parameter(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, LRHPARAMETER *out, OSInt indexSep, Double offsetCorr);
			static void setup_interpolation_parameter_h(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, LRHPARAMETER *out, OSInt indexSep, Double offsetCorr);
			static void setup_decimation_parameter_h(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, LRHPARAMETER *out, OSInt indexSep, Double offsetCorr);

			virtual void UpdateRGBTable();

			void DestoryHori();
			void DestoryVert();
		public:
			LanczosResizerLR_C32(UOSInt hnTap, UOSInt vnTap, const Media::ColorProfile *destColor, Media::ColorManagerSess *colorSess, Media::AlphaType srcAlphaType, Double srcRefLuminance, Media::PixelFormat pf);
			virtual ~LanczosResizerLR_C32();

			virtual void Resize(UInt8 *src, OSInt sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UInt8 *dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight);
			virtual void YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam);
			virtual void RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam);
			virtual void SetSrcRefLuminance(Double srcRefLuminance);

			virtual Bool IsSupported(Media::FrameInfo *srcInfo);
			virtual Media::StaticImage *ProcessToNewPartial(Media::StaticImage *srcImage, Double srcX1, Double srcY1, Double srcX2, Double srcY2);

			Double GetHAvgTime();
			Double GetVAvgTime();

			Bool IsRGBChanged();
		};
	}
}
#endif
