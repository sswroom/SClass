#ifndef _SM_UI_JAVAUI_JUIPICTUREBOXSIMPLE
#define _SM_UI_JAVAUI_JUIPICTUREBOXSIMPLE
#include "UI/GUIPictureBoxSimple.h"

namespace UI
{
	namespace JavaUI
	{
		class JUIPictureBoxSimple : public GUIPictureBoxSimple
		{
		public:
			JUIPictureBoxSimple(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder);
			virtual ~JUIPictureBoxSimple();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual void SetImage(Optional<Media::StaticImage> currImage);
			virtual void SetImageDImg(Optional<Media::DrawImage> img);
		};
	}
}
#endif
