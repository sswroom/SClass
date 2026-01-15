#ifndef _SM_MEDIA_OCRENGINE
#define _SM_MEDIA_OCRENGINE
#include "AnyType.h"
#include "Data/ArrayListStringNN.h"
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
		
		typedef void (CALLBACKFUNC OCRResultFunc)(AnyType userObj, NN<Text::String> text, Double confidence, Math::RectArea<IntOS> boundary);
	private:
		class ClassData;
		NN<ClassData> clsData;
		NN<Text::String> lang;

		OCRResultFunc hdlr;
		AnyType hdlrObj;
	public:
		OCREngine(Language lang);
		OCREngine(Text::CStringNN lang);
		~OCREngine();

		void ChangeLanguage(Text::CStringNN lang);
		void SetCharWhiteList(UnsafeArrayOpt<const Char> whiteList);

		Bool SetParsingImage(NN<Media::StaticImage> img);
		Bool SetOCVFrame(NN<Media::OpenCV::OCVFrame> frame);
		Optional<Text::String> ParseInsideImage(Math::RectArea<UIntOS> area, OptOut<UIntOS> confidence);
		
		void HandleOCRResult(OCRResultFunc hdlr, AnyType userObj);
		Bool ParseAllInImage();

		void GetAvailableLanguages(NN<Data::ArrayListStringNN> langs) const; //Need to release
	};
}
#endif
