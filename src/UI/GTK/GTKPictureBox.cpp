#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/ImageUtil_C.h"
#include "Media/Resizer/LanczosResizerRGB_C8.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/GTK/GTKPictureBox.h"


gboolean UI::GTK::GTKPictureBox::SignalButtonPress(GtkWidget *widget, GdkEvent  *event, gpointer user_data)
{
	UI::GTK::GTKPictureBox *me = (UI::GTK::GTKPictureBox*)user_data;
	GdkEventButton *evt = (GdkEventButton*)event;
	if (evt->type == GDK_BUTTON_PRESS)
	{
		UI::GUIControl::MouseButton btn;
		switch (evt->button)
		{
			default:
			case 1:
				btn = UI::GUIControl::MBTN_LEFT;
				break;
			case 2:
				btn = UI::GUIControl::MBTN_MIDDLE;
				break;
			case 3:
				btn = UI::GUIControl::MBTN_RIGHT;
				break;
			case 4:
				btn = UI::GUIControl::MBTN_X1;
				break;
			case 5:
				btn = UI::GUIControl::MBTN_X2;
				break;
		}
		me->EventButtonDown(Math::Coord2D<OSInt>(Double2OSInt(evt->x), Double2OSInt(evt->y)), btn);
	}
	return false;
}

gboolean UI::GTK::GTKPictureBox::SignalButtonRelease(GtkWidget *widget, GdkEvent  *event, gpointer user_data)
{
	UI::GTK::GTKPictureBox *me = (UI::GTK::GTKPictureBox*)user_data;
	GdkEventButton *evt = (GdkEventButton*)event;
	if (evt->type == GDK_BUTTON_RELEASE)
	{
		UI::GUIControl::MouseButton btn;
		switch (evt->button)
		{
			default:
			case 1:
				btn = UI::GUIControl::MBTN_LEFT;
				break;
			case 2:
				btn = UI::GUIControl::MBTN_MIDDLE;
				break;
			case 3:
				btn = UI::GUIControl::MBTN_RIGHT;
				break;
			case 4:
				btn = UI::GUIControl::MBTN_X1;
				break;
			case 5:
				btn = UI::GUIControl::MBTN_X2;
				break;
		}
		me->EventButtonUp(Math::Coord2D<OSInt>(Double2OSInt(evt->x), Double2OSInt(evt->y)), btn);
	}
	return false;
}

gboolean UI::GTK::GTKPictureBox::SignalMotionNotify(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUIPictureBox *me = (UI::GUIPictureBox*)data;
	GdkEventMotion *evt = (GdkEventMotion*)event;
	me->EventMouseMove(Math::Coord2D<OSInt>(Double2OSInt(evt->x), Double2OSInt(evt->y)));
	return false;
}

void UI::GTK::GTKPictureBox::UpdatePreview()
{
	gtk_image_set_from_pixbuf((GtkImage*)this->gtkImage, 0);
	if (this->pixbuf)
	{
		g_object_unref(this->pixbuf);
		this->pixbuf = 0;
	}
	this->tmpImage.Delete();
	NN<Media::StaticImage> img;
	if (this->currImage.SetTo(img))
	{
		if (this->allowResize)
		{
			NN<Media::StaticImage> tmpImage;
			if (resizer->ProcessToNew(img).SetTo(tmpImage))
			{
				GdkPixbuf *buf = gdk_pixbuf_new_from_data(tmpImage->data.Ptr(), GDK_COLORSPACE_RGB, tmpImage->info.storeBPP == 32, 8, (int)(OSInt)tmpImage->info.dispSize.x, (int)(OSInt)tmpImage->info.dispSize.y, (int)(OSInt)tmpImage->info.storeSize.x << 2, 0, 0);
				guchar *pixels = gdk_pixbuf_get_pixels(buf);
				ImageUtil_SwapRGB(pixels, (UInt32)gdk_pixbuf_get_rowstride(buf) / 4 * img->info.dispSize.y, 32);
				if (img->info.atype != Media::AT_ALPHA)
				{
					ImageUtil_ImageFillAlpha32(pixels, img->info.dispSize.x, img->info.dispSize.y, (UInt32)gdk_pixbuf_get_rowstride(buf), 255);
				}
				this->pixbuf = buf;
				this->tmpImage = tmpImage;
				gtk_image_set_from_pixbuf((GtkImage*)this->gtkImage, buf);
			}
		}
		else
		{
			GdkPixbuf *buf = gdk_pixbuf_new_from_data(img->data.Ptr(), GDK_COLORSPACE_RGB, img->info.storeBPP == 32, 8, (int)(OSInt)img->info.dispSize.x, (int)(OSInt)img->info.dispSize.y, (int)(OSInt)img->info.storeSize.x << 2, 0, 0);
			guchar *pixels = gdk_pixbuf_get_pixels(buf);
			ImageUtil_SwapRGB(pixels, (UInt32)gdk_pixbuf_get_rowstride(buf) / 4 * img->info.dispSize.y, 32);
			if (img->info.atype != Media::AT_ALPHA)
			{
				ImageUtil_ImageFillAlpha32(pixels, img->info.dispSize.x, img->info.dispSize.y, (UInt32)gdk_pixbuf_get_rowstride(buf), 255);
			}
			this->pixbuf = buf;
			gtk_image_set_from_pixbuf((GtkImage*)this->gtkImage, buf);
		}
	}
	this->Redraw();
}

UI::GTK::GTKPictureBox::GTKPictureBox(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder, Bool allowResize) : UI::GUIPictureBox(ui, parent, eng, hasBorder, allowResize)
{
	this->pixbuf = 0;
	this->tmpImage = 0;
	this->gtkImage = gtk_image_new_from_pixbuf(0);
	this->eventBox = gtk_event_box_new ();
	gtk_container_add(GTK_CONTAINER(this->eventBox), this->gtkImage);

	g_signal_connect(G_OBJECT(this->eventBox), "button-press-event", G_CALLBACK(SignalButtonPress), this);
	g_signal_connect(G_OBJECT(this->eventBox), "button-release-event", G_CALLBACK(SignalButtonRelease), this);
	g_signal_connect(G_OBJECT(this->eventBox), "motion-notify-event", G_CALLBACK(SignalMotionNotify), this);

	this->hwnd = (ControlHandle*)this->eventBox;
	parent->AddChild(*this);
	gtk_widget_show((GtkWidget*)this->gtkImage);
	this->Show();
}

UI::GTK::GTKPictureBox::~GTKPictureBox()
{
	if (this->pixbuf)
	{
		g_object_unref(this->pixbuf);
		this->pixbuf = 0;
	}
	this->tmpImage.Delete();
}

OSInt UI::GTK::GTKPictureBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}
