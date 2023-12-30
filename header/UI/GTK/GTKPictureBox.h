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
			GtkWidget *eventBox;
			Media::StaticImage *tmpImage;

			static gboolean SignalButtonPress(GtkWidget *widget, GdkEvent  *event, gpointer user_data);
			static gboolean SignalButtonRelease(GtkWidget *widget, GdkEvent  *event, gpointer user_data);
			static gboolean SignalMotionNotify(GtkWidget *widget, GdkEvent *event, gpointer data);
			virtual void UpdatePreview();
		public:
			GTKPictureBox(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, NotNullPtr<Media::DrawEngine> eng, Bool hasBorder, Bool allowResize);
			virtual ~GTKPictureBox();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
		};
	}
}
#endif
