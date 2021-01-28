#ifndef _SM_UI_GUIICON
#define _SM_UI_GUIICON
#include "Media/DrawEngine.h"

namespace UI
{
	class GUIIcon
	{
	private:
		void *hand;

	protected:
		GUIIcon();
	public:
		GUIIcon(Media::DrawImage *img);
		~GUIIcon();

		void *GetHandle();
	};
};
#endif
