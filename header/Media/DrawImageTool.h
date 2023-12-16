#ifndef _SM_MEDIA_DRAWIMAGETOOL
#define _SM_MEDIA_DRAWIMAGETOOL
#include "Data/ArrayListStringNN.h"
#include "Media/DrawEngine.h"

namespace Media
{
	class DrawImageTool
	{
	public:
		static void SplitString(NotNullPtr<Media::DrawImage> dimg, Text::CStringNN txt, NotNullPtr<Data::ArrayListStringNN> outStr, NotNullPtr<Media::DrawFont> f, Double width);
	};
}
#endif
