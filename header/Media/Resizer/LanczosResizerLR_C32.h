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
				UIntOS length;
				UnsafeArray<Int64> weight;
				UnsafeArray<IntOS> index;
				UIntOS tap;
			} LRHPARAMETER;

		protected:
			Int32 currId;
			UIntOS hnTap;
			UIntOS vnTap;
			Sync::Mutex mut;
			NN<Media::Resizer::LanczosResizerLR_C32Action> action;

			Double hsSize;
			Double hsOfst;
			UIntOS hdSize;
			UnsafeArrayOpt<IntOS> hIndex;
			UnsafeArrayOpt<Int64> hWeight;
			UIntOS hTap;
			Optional<Media::Resizer::LanczosResizerLR_C32Action::HoriFilter> hFilter;

			Double vsSize;
			Double vsOfst;
			UIntOS vdSize;
			IntOS vsStep;
			UnsafeArrayOpt<IntOS> vIndex;
			UnsafeArrayOpt<Int64> vWeight;
			UIntOS vTap;
			Optional<Media::Resizer::LanczosResizerLR_C32Action::VertFilter> vFilter;

			Media::ColorProfile destColor;
			Optional<Media::ColorManagerSess> colorSess;
			Double srcRefLuminance;
			Bool rgbChanged;
			UnsafeArrayOpt<UInt8> rgbTable;
			Media::PixelFormat pf;

			static void SetupInterpolationParameterV(UIntOS nTap, Double source_length, UIntOS source_max_pos, UIntOS result_length, NN<LRHPARAMETER> out, IntOS indexSep, Double offsetCorr);
			static void SetupDecimationParameterV(UIntOS nTap, Double source_length, UIntOS source_max_pos, UIntOS result_length, NN<LRHPARAMETER> out, IntOS indexSep, Double offsetCorr);
			static void SetupInterpolationParameterH(UIntOS nTap, Double source_length, UIntOS source_max_pos, UIntOS result_length, NN<LRHPARAMETER> out, IntOS indexSep, Double offsetCorr);
			static void SetupDecimationParameterH(UIntOS nTap, Double source_length, UIntOS source_max_pos, UIntOS result_length, NN<LRHPARAMETER> out, IntOS indexSep, Double offsetCorr);

			virtual UnsafeArray<UInt8> UpdateRGBTable();

			void DestoryHori();
			void DestoryVert();
		public:
			LanczosResizerLR_C32(UIntOS hnTap, UIntOS vnTap, NN<const Media::ColorProfile> destColor, Optional<Media::ColorManagerSess> colorSess, Media::AlphaType srcAlphaType, Double srcRefLuminance, Media::PixelFormat pf);
			virtual ~LanczosResizerLR_C32();

			virtual void Resize(UnsafeArray<const UInt8> src, IntOS sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UnsafeArray<UInt8> dest, IntOS dbpl, UIntOS dwidth, UIntOS dheight);
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
