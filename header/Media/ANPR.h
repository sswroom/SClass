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
		typedef void (__stdcall *NumPlateResult)(void *userObj, Media::StaticImage *simg, Math::RectArea<UOSInt> *area, Text::String *result);
	private:
		Media::OCREngine ocr;
		Media::OpenCV::OCVNumPlateFinder finder;
		UOSInt parsedCnt;
		NumPlateResult hdlr;
		void *hdlrObj;

		static void NumPlateArea(void *userObj, Media::OpenCV::OCVFrame *filteredFrame, UOSInt *rect);
	public:
		ANPR();
		~ANPR();

		void SetResultHandler(NumPlateResult hdlr, void *userObj);
		Bool ParseImage(Media::StaticImage *simg);
	};
}
#endif
