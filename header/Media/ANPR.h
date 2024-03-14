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
		typedef void (__stdcall *NumPlateResult)(AnyType userObj, NotNullPtr<Media::StaticImage> simg, Math::RectArea<UOSInt> area, NotNullPtr<Text::String> result, Double maxTileAngle, Double pxArea, UOSInt confidence, NotNullPtr<Media::StaticImage> plateImg);
	private:
		Media::OCREngine ocr;
		Media::OpenCV::OCVNumPlateFinder finder;
		UOSInt parsedCnt;
		NumPlateResult hdlr;
		AnyType hdlrObj;

		static void NumPlateArea(AnyType userObj, Media::OpenCV::OCVFrame *filteredFrame, Math::Coord2D<UOSInt> *rect, Double maxTileAngle, Double pxArea, Media::OpenCV::OCVNumPlateFinder::PlateSize psize);
		static NotNullPtr<Media::StaticImage> CreatePlainImage(UInt8 *sptr, Math::Size2D<UOSInt> sSize, UOSInt sbpl, Math::Coord2D<UOSInt> *rect, Media::OpenCV::OCVNumPlateFinder::PlateSize psize);
		static NotNullPtr<Media::StaticImage> CreatePlainImage(UInt8 *sptr, Math::Size2D<UOSInt> sSize, UOSInt sbpl, Math::Quadrilateral quad, Media::OpenCV::OCVNumPlateFinder::PlateSize psize);
	public:
		ANPR();
		~ANPR();

		void SetResultHandler(NumPlateResult hdlr, AnyType userObj);
		Bool ParseImage(NotNullPtr<Media::StaticImage> simg);
		Bool ParseImageQuad(NotNullPtr<Media::StaticImage> simg, Math::Quadrilateral quad);
		Bool ParseImagePlatePoint(NotNullPtr<Media::StaticImage> simg, Math::Coord2D<UOSInt> coord);
	};
}
#endif
