#ifndef _SM_UI_GTK_GTKPICTUREBOX
#define _SM_UI_GTK_GTKPICTUREBOX
#include "UI/GUIPictureBox.h"
#include <gtk/gtk.h>

namespace UI
{
	namespace GTK
	{
		class GTKPictureBox : public GUIPictureBox
		{
		private:
			GdkPixbuf *pixbuf;
			GtkWidget *gtkImage;
			void *eventBox;
			void *eventBox2;
			Optional<Media::StaticImage> tmpImage;

			virtual void UpdatePreview();
		public:
			GTKPictureBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder, Bool allowResize);
			virtual ~GTKPictureBox();

			virtual IntOS OnNotify(UInt32 code, IntOS lParam);
		};
	}
}
#endif
