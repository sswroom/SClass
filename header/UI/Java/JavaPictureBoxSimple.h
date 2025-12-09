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
			JavaPictureBoxSimple(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder);
			virtual ~JavaPictureBoxSimple();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual void SetImage(Optional<Media::StaticImage> currImage);
			virtual void SetImageDImg(Optional<Media::DrawImage> img);
		};
	}
}
#endif
