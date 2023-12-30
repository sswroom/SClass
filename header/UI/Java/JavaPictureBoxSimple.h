#ifndef _SM_UI_JAVA_JAVAPICTUREBOXSIMPLE
#define _SM_UI_JAVA_JAVAPICTUREBOXSIMPLE
#include "UI/GUIPictureBoxSimple.h"

namespace UI
{
	namespace Java
	{
		class JavaPictureBoxSimple : public GUIPictureBoxSimple
		{
		public:
			JavaPictureBoxSimple(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, NotNullPtr<Media::DrawEngine> eng, Bool hasBorder);
			virtual ~JavaPictureBoxSimple();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual void SetImage(Media::StaticImage *currImage);
			virtual void SetImageDImg(Media::DrawImage *img);
		};
	}
}
#endif
