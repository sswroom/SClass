#ifndef _SM_UI_JAVAUI_JUIPICTUREBOX
#define _SM_UI_JAVAUI_JUIPICTUREBOX
#include "UI/GUIPictureBox.h"

namespace UI
{
	namespace JavaUI
	{
		class JUIPictureBox : public GUIPictureBox
		{
		private:
			virtual void UpdatePreview();
		public:
			JUIPictureBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder, Bool allowResize);
			virtual ~JUIPictureBox();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
		};
	}
}
#endif
