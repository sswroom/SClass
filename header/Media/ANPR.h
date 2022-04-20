#ifndef _SM_MEDIA_ANPR
#define _SM_MEDIA_ANPR
#include "Media/OCREngine.h"
#include "Text/String.h"

namespace Media
{
	class ANPR
	{
	private:
		Media::OCREngine ocr;

	public:
		ANPR();
		~ANPR();

		Text::String *ParseImage(Media::StaticImage *simg);
	};
}
#endif
