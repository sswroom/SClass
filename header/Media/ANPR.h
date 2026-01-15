#ifndef _SM_MEDIA_ANPR
#define _SM_MEDIA_ANPR
#include "AnyType.h"
#include "Media/OCREngine.h"
#include "Media/OpenCV/OCVNumPlateFinder.h"
#include "Math/RectArea.hpp"
#include "Text/String.h"

namespace Media
{
	class ANPR
	{
	public:
		typedef void (CALLBACKFUNC NumPlateResult)(AnyType userObj, NN<Media::StaticImage> simg, Math::RectArea<UIntOS> area, NN<Text::String> result, Double maxTileAngle, Double pxArea, UIntOS confidence, NN<Media::StaticImage> plateImg);
	private:
		Media::OCREngine ocr;
		Media::OpenCV::OCVNumPlateFinder finder;
		UIntOS parsedCnt;
		NumPlateResult hdlr;
		AnyType hdlrObj;

		static void __stdcall NumPlateArea(AnyType userObj, NN<Media::OpenCV::OCVFrame> filteredFrame, UnsafeArray<Math::Coord2D<UIntOS>> rect, Double maxTileAngle, Double pxArea, Media::OpenCV::OCVNumPlateFinder::PlateSize psize);
		static NN<Media::StaticImage> CreatePlainImage(UnsafeArray<UInt8> sptr, Math::Size2D<UIntOS> sSize, UIntOS sbpl, UnsafeArray<Math::Coord2D<UIntOS>> rect, Media::OpenCV::OCVNumPlateFinder::PlateSize psize);
		static NN<Media::StaticImage> CreatePlainImage(UnsafeArray<UInt8> sptr, Math::Size2D<UIntOS> sSize, UIntOS sbpl, Math::Quadrilateral quad, Media::OpenCV::OCVNumPlateFinder::PlateSize psize);
	public:
		ANPR();
		~ANPR();

		void SetResultHandler(NumPlateResult hdlr, AnyType userObj);
		Bool ParseImage(NN<Media::StaticImage> simg);
		Bool ParseImageQuad(NN<Media::StaticImage> simg, Math::Quadrilateral quad);
		Bool ParseImagePlatePoint(NN<Media::StaticImage> simg, Math::Coord2D<UIntOS> coord);
	};
}
#endif
