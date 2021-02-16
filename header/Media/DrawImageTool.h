#ifndef _SM_MEDIA_DRAWIMAGETOOL
#define _SM_MEDIA_DRAWIMAGETOOL
#include "Media/DrawEngine.h"

namespace Media
{
	class DrawImageTool
	{
	public:
		static void SplitString(Media::DrawImage *dimg, const UTF8Char *txt, Data::ArrayList<const UTF8Char *> *outStr, Media::DrawFont *f, Double width);
	};
}
#endif
