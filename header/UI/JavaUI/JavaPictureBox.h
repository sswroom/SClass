#ifndef _SM_UI_JAVAUI_JAVAPICTUREBOX
#define _SM_UI_JAVAUI_JAVAPICTUREBOX
#include "UI/GUIPictureBox.h"

namespace UI
{
	namespace JavaUI
	{
		class JavaPictureBox : public GUIPictureBox
		{
		private:
			virtual void UpdatePreview();
		public:
			JavaPictureBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder, Bool allowResize);
			virtual ~JavaPictureBox();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
		};
	}
}
#endif
