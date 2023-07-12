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
		MSFontRenderer(NotNullPtr<Text::String> sourceName, const UInt8 *fontBuff, UOSInt buffSize);
		virtual ~MSFontRenderer();

		Bool IsError();
		virtual UTF32Char GetMinChar() const;
		virtual UTF32Char GetMaxChar() const;
		virtual Media::StaticImage *CreateImage(UTF32Char charCode, Math::Size2D<UOSInt> targetSize) const;
	};
}
#endif
