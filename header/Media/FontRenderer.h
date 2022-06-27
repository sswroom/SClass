#ifndef _SM_MEDIA_FONTRENDERER
#define _SM_MEDIA_FONTRENDERER
#include "IO/ParsedObject.h"

namespace Media
{
	class StaticImage;
	
	class FontRenderer : public IO::ParsedObject
	{
	public:
		FontRenderer(Text::String *sourceName);
		virtual ~FontRenderer();

		virtual IO::ParserType GetParserType() const;
		virtual UTF32Char GetMinChar() const = 0;
		virtual UTF32Char GetMaxChar() const = 0;
		virtual Media::StaticImage *CreateImage(UTF32Char charCode, UOSInt targetWidth, UOSInt targetHeight) const = 0;
	};
}
#endif
