#ifndef _SM_UI_CLIPBOARDUTIL
#define _SM_UI_CLIPBOARDUTIL
#include "Media/ImageList.h"
#include "Parser/ParserList.h"
#include "UI/Clipboard.h"

namespace UI
{
	class ClipboardUtil
	{
	public:
		static Optional<Media::ImageList> LoadImage(NN<UI::Clipboard> clipboard, NN<Parser::ParserList> parsers);
	};
}
#endif
