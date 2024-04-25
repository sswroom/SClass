#ifndef _SM_MEDIA_DRAWIMAGETOOL
#define _SM_MEDIA_DRAWIMAGETOOL
#include "Data/ArrayListStringNN.h"
#include "Media/DrawEngine.h"

namespace Media
{
	class DrawImageTool
	{
	public:
		static void SplitString(NN<Media::DrawImage> dimg, Text::CStringNN txt, NN<Data::ArrayListStringNN> outStr, NN<Media::DrawFont> f, Double width);
	};
}
#endif
