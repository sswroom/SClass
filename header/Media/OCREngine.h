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
		
		typedef void (__stdcall *OCRResultFunc)(void *userObj, NotNullPtr<Text::String> text, Double confidence, Math::RectArea<OSInt> boundary);
	private:
		class ClassData;
		ClassData *clsData;

		OCRResultFunc hdlr;
		void *hdlrObj;
	public:
		OCREngine(Language lang);
		~OCREngine();

		void SetCharWhiteList(const Char *whiteList);

		Bool SetParsingImage(Media::StaticImage *img);
		Bool SetOCVFrame(Media::OpenCV::OCVFrame *frame);
		Text::String *ParseInsideImage(Math::RectArea<UOSInt> area, UOSInt *confidence);
		
		void HandleOCRResult(OCRResultFunc hdlr, void *userObj);
		Bool ParseAllInImage();
	};
}
#endif
