#ifndef _SM_MEDIA_ANPR
#define _SM_MEDIA_ANPR
#include "AnyType.h"
#include "Media/OCREngine.h"
#include "Media/OpenCV/OCVNumPlateFinder.h"
#include "Math/RectArea.h"
#include "Text/String.h"

namespace Media
{
	class ANPR
	{
	public:
		typedef void (__stdcall *NumPlateResult)(AnyType userObj, NN<Media::StaticImage> simg, Math::RectArea<UOSInt> area, NN<Text::String> result, Double maxTileAngle, Double pxArea, UOSInt confidence, NN<Media::StaticImage> plateImg);
	private:
		Media::OCREngine ocr;
		Media::OpenCV::OCVNumPlateFinder finder;
		UOSInt parsedCnt;
		NumPlateResult hdlr;
		AnyType hdlrObj;

		static void NumPlateArea(AnyType userObj, Media::OpenCV::OCVFrame *filteredFrame, Math::Coord2D<UOSInt> *rect, Double maxTileAngle, Double pxArea, Media::OpenCV::OCVNumPlateFinder::PlateSize psize);
		static NN<Media::StaticImage> CreatePlainImage(UnsafeArray<UInt8> sptr, Math::Size2D<UOSInt> sSize, UOSInt sbpl, Math::Coord2D<UOSInt> *rect, Media::OpenCV::OCVNumPlateFinder::PlateSize psize);
		static NN<Media::StaticImage> CreatePlainImage(UnsafeArray<UInt8> sptr, Math::Size2D<UOSInt> sSize, UOSInt sbpl, Math::Quadrilateral quad, Media::OpenCV::OCVNumPlateFinder::PlateSize psize);
	public:
		ANPR();
		~ANPR();

		void SetResultHandler(NumPlateResult hdlr, AnyType userObj);
		Bool ParseImage(NN<Media::StaticImage> simg);
		Bool ParseImageQuad(NN<Media::StaticImage> simg, Math::Quadrilateral quad);
		Bool ParseImagePlatePoint(NN<Media::StaticImage> simg, Math::Coord2D<UOSInt> coord);
	};
}
#endif
