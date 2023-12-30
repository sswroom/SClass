#ifndef _SM_UI_JAVA_JAVAPICTUREBOX
#define _SM_UI_JAVA_JAVAPICTUREBOX
#include "UI/GUIPictureBox.h"

namespace UI
{
	namespace Java
	{
		class JavaPictureBox : public GUIPictureBox
		{
		private:
			virtual void UpdatePreview();
		public:
			JavaPictureBox(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, NotNullPtr<Media::DrawEngine> eng, Bool hasBorder, Bool allowResize);
			virtual ~JavaPictureBox();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
		};
	}
}
#endif
