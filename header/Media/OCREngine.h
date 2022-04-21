#ifndef _SM_MEDIA_OCRENGINE
#define _SM_MEDIA_OCRENGINE
#include "Math/RectArea.h"
#include "Media/StaticImage.h"
#include "Media/OpenCV/OCVFrame.h"
#include "Text/String.h"

namespace Media
{
	class OCREngine
	{
	public:
		enum class Language
		{
			English
		};
	private:
		class ClassData;
		ClassData *clsData;

	public:
		OCREngine(Language lang);
		~OCREngine();

		void SetCharWhiteList(const Char *whiteList);

		Bool SetParsingImage(Media::StaticImage *img);
		Bool SetOCVFrame(Media::OpenCV::OCVFrame *frame);
		Text::String *ParseInsideImage(Math::RectArea<UOSInt> area, UOSInt *confidence);
	};
}
#endif
