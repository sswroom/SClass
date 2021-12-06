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
		FontRenderer(const UTF8Char *sourceName);
		virtual ~FontRenderer();

		virtual IO::ParserType GetParserType();
		virtual UTF32Char GetMinChar() = 0;
		virtual UTF32Char GetMaxChar() = 0;
		virtual Media::StaticImage *CreateImage(UTF32Char charCode, UOSInt targetWidth, UOSInt targetHeight) = 0;
	};
}
#endif
