#ifndef _SM_MEDIA_DRAWIMAGETOOL
#define _SM_MEDIA_DRAWIMAGETOOL
#include "Media/DrawEngine.h"

namespace Media
{
	class DrawImageTool
	{
	public:
		static void SplitString(Media::DrawImage *dimg, Text::CString txt, Data::ArrayList<Text::String *> *outStr, Media::DrawFont *f, Double width);
	};
}
#endif
