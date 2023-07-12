#ifndef _SM_MEDIA_FONTRENDERER
#define _SM_MEDIA_FONTRENDERER
#include "Math/Size2D.h"
#include "IO/ParsedObject.h"

namespace Media
{
	class StaticImage;
	
	class FontRenderer : public IO::ParsedObject
	{
	public:
		FontRenderer(NotNullPtr<Text::String> sourceName);
		virtual ~FontRenderer();

		virtual IO::ParserType GetParserType() const;
		virtual UTF32Char GetMinChar() const = 0;
		virtual UTF32Char GetMaxChar() const = 0;
		virtual Media::StaticImage *CreateImage(UTF32Char charCode, Math::Size2D<UOSInt> targetSize) const = 0;
	};
}
#endif
