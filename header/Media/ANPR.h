#ifndef _SM_MEDIA_ANPR
#define _SM_MEDIA_ANPR
#include "Media/OCREngine.h"
#include "Media/OpenCV/OCVNumPlateFinder.h"
#include "Math/RectArea.h"
#include "Text/String.h"

namespace Media
{
	class ANPR
	{
	public:
		typedef void (__stdcall *NumPlateResult)(void *userObj, Media::StaticImage *simg, Math::RectArea<UOSInt> *area, Text::String *result, Double maxTileAngle, Double pxArea, UOSInt confidence, Media::StaticImage *plateImg);
	private:
		Media::OCREngine ocr;
		Media::OpenCV::OCVNumPlateFinder finder;
		UOSInt parsedCnt;
		NumPlateResult hdlr;
		void *hdlrObj;

		static void NumPlateArea(void *userObj, Media::OpenCV::OCVFrame *filteredFrame, Math::Coord2D<UOSInt> *rect, Double maxTileAngle, Double pxArea, Media::OpenCV::OCVNumPlateFinder::PlateSize psize);
		static Media::StaticImage *CreatePlainImage(UInt8 *sptr, UOSInt swidth, UOSInt sheight, UOSInt sbpl, Math::Coord2D<UOSInt> *rect, Media::OpenCV::OCVNumPlateFinder::PlateSize psize);
		static Media::StaticImage *CreatePlainImage(UInt8 *sptr, UOSInt swidth, UOSInt sheight, UOSInt sbpl, Math::Quadrilateral quad, Media::OpenCV::OCVNumPlateFinder::PlateSize psize);
	public:
		ANPR();
		~ANPR();

		void SetResultHandler(NumPlateResult hdlr, void *userObj);
		Bool ParseImage(Media::StaticImage *simg);
		Bool ParseImageQuad(Media::StaticImage *simg, Math::Quadrilateral quad);
	};
}
#endif
