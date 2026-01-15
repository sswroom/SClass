#ifndef _SM_UI_GTK_GTKPICTUREBOXSIMPLE
#define _SM_UI_GTK_GTKPICTUREBOXSIMPLE
#include "UI/GUIPictureBoxSimple.h"
#include <gtk/gtk.h>

namespace UI
{
	namespace GTK
	{
		class GTKPictureBoxSimple : public GUIPictureBoxSimple
		{
		private:
			GdkPixbuf *pixbuf;
			Media::StaticImage *tmpImage;
			Optional<Media::StaticImage> currImage;
			Optional<Media::DrawImage> prevImageD;

			void UpdatePreview();
		public:
			GTKPictureBoxSimple(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder);
			virtual ~GTKPictureBoxSimple();

			virtual IntOS OnNotify(UInt32 code, void *lParam);
			virtual void SetImage(Optional<Media::StaticImage> currImage);
			virtual void SetImageDImg(Optional<Media::DrawImage> img);
		};
	}
}
#endif
