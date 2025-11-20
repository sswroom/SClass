#ifndef _SM_MEDIA_OCRENGINE
#define _SM_MEDIA_OCRENGINE
#include "AnyType.h"
#include "Math/RectArea.hpp"
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
		
		typedef void (CALLBACKFUNC OCRResultFunc)(AnyType userObj, NN<Text::String> text, Double confidence, Math::RectArea<OSInt> boundary);
	private:
		class ClassData;
		ClassData *clsData;

		OCRResultFunc hdlr;
		AnyType hdlrObj;
	public:
		OCREngine(Language lang);
		~OCREngine();

		void SetCharWhiteList(const Char *whiteList);

		Bool SetParsingImage(NN<Media::StaticImage> img);
		Bool SetOCVFrame(NN<Media::OpenCV::OCVFrame> frame);
		Optional<Text::String> ParseInsideImage(Math::RectArea<UOSInt> area, OptOut<UOSInt> confidence);
		
		void HandleOCRResult(OCRResultFunc hdlr, AnyType userObj);
		Bool ParseAllInImage();
	};
}
#endif
