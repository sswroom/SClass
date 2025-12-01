#ifndef _SM_MEDIA_RESIZER_LANCZOSRESIZERLR_C32
#define _SM_MEDIA_RESIZER_LANCZOSRESIZERLR_C32
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Media/ImageResizer.h"
#include "Media/ColorManager.h"
#include "Media/Resizer/LanczosResizerLR_C32Action.h"

namespace Media
{
	namespace Resizer
	{
		class LanczosResizerLR_C32 : public Media::ImageResizer, public Media::ColorHandler
		{
		protected:
			typedef struct
			{
				UOSInt length;
				UnsafeArray<Int64> weight;
				UnsafeArray<OSInt> index;
				UOSInt tap;
			} LRHPARAMETER;

		protected:
			Int32 currId;
			UOSInt hnTap;
			UOSInt vnTap;
			Sync::Mutex mut;
			NN<Media::Resizer::LanczosResizerLR_C32Action> action;

			Double hsSize;
			Double hsOfst;
			UOSInt hdSize;
			UnsafeArrayOpt<OSInt> hIndex;
			UnsafeArrayOpt<Int64> hWeight;
			UOSInt hTap;
			Optional<Media::Resizer::LanczosResizerLR_C32Action::HoriFilter> hFilter;

			Double vsSize;
			Double vsOfst;
			UOSInt vdSize;
			OSInt vsStep;
			UnsafeArrayOpt<OSInt> vIndex;
			UnsafeArrayOpt<Int64> vWeight;
			UOSInt vTap;
			Optional<Media::Resizer::LanczosResizerLR_C32Action::VertFilter> vFilter;

			Media::ColorProfile destColor;
			Optional<Media::ColorManagerSess> colorSess;
			Double srcRefLuminance;
			Bool rgbChanged;
			UnsafeArrayOpt<UInt8> rgbTable;
			Media::PixelFormat pf;

			static void SetupInterpolationParameterV(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, NN<LRHPARAMETER> out, OSInt indexSep, Double offsetCorr);
			static void SetupDecimationParameterV(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, NN<LRHPARAMETER> out, OSInt indexSep, Double offsetCorr);
			static void SetupInterpolationParameterH(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, NN<LRHPARAMETER> out, OSInt indexSep, Double offsetCorr);
			static void SetupDecimationParameterH(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, NN<LRHPARAMETER> out, OSInt indexSep, Double offsetCorr);

			virtual UnsafeArray<UInt8> UpdateRGBTable();

			void DestoryHori();
			void DestoryVert();
		public:
			LanczosResizerLR_C32(UOSInt hnTap, UOSInt vnTap, NN<const Media::ColorProfile> destColor, Optional<Media::ColorManagerSess> colorSess, Media::AlphaType srcAlphaType, Double srcRefLuminance, Media::PixelFormat pf);
			virtual ~LanczosResizerLR_C32();

			virtual void Resize(UnsafeArray<const UInt8> src, OSInt sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UnsafeArray<UInt8> dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight);
			virtual void YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam);
			virtual void RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam);
			virtual void SetSrcRefLuminance(Double srcRefLuminance);

			virtual Bool IsSupported(NN<const Media::FrameInfo> srcInfo);
			virtual Optional<Media::StaticImage> ProcessToNewPartial(NN<const Media::RasterImage> srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR);

			Double GetHAvgTime();
			Double GetVAvgTime();

			Bool IsRGBChanged();
		};
	}
}
#endif
