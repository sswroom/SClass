#ifndef _SM_MEDIA_MSFONTRENDERER
#define _SM_MEDIA_MSFONTRENDERER
#include "Media/FontRenderer.h"

namespace Media
{
	class MSFontRenderer : public FontRenderer
	{
	private:
		UInt8 *fontBuff;

	public:
		MSFontRenderer(Text::String *sourceName, const UInt8 *fontBuff, UOSInt buffSize);
		MSFontRenderer(const UTF8Char *sourceName, const UInt8 *fontBuff, UOSInt buffSize);
		virtual ~MSFontRenderer();

		Bool IsError();
		virtual UTF32Char GetMinChar();
		virtual UTF32Char GetMaxChar();
		virtual Media::StaticImage *CreateImage(UTF32Char charCode, UOSInt targetWidth, UOSInt targetHeight);
	};
}
#endif
