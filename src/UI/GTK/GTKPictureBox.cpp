#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Media/ImageUtil_C.h"
#include "Media/Resizer/LanczosResizerRGB_C8.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/GTK/GTKPictureBox.h"

#if GTK_CHECK_VERSION(4, 0, 0)
void GTKPictureBox_SignalButtonPress(GtkGestureClick *gesture, int n_press, double x, double y, gpointer user_data)
{
	UI::GTK::GTKPictureBox *me = (UI::GTK::GTKPictureBox*)user_data;
	UI::GUIControl::MouseButton btn = UI::GUIControl::MBTN_LEFT;
	guint button = gtk_gesture_single_get_current_button (GTK_GESTURE_SINGLE (gesture));
	switch (button)
	{
		default:
		case GDK_BUTTON_PRIMARY:
			btn = UI::GUIControl::MBTN_LEFT;
			break;
		case GDK_BUTTON_MIDDLE:
			btn = UI::GUIControl::MBTN_MIDDLE;
			break;
		case GDK_BUTTON_SECONDARY:
			btn = UI::GUIControl::MBTN_RIGHT;
			break;
		case 4:
			btn = UI::GUIControl::MBTN_X1;
			break;
		case 5:
			btn = UI::GUIControl::MBTN_X2;
			break;
	}
	me->EventButtonDown(Math::Coord2D<IntOS>(Double2IntOS(x), Double2IntOS(y)), btn);
}

void GTKPictureBox_SignalButtonRelease(GtkGestureClick *gesture, int n_press, double x, double y, gpointer user_data)
{
	UI::GTK::GTKPictureBox *me = (UI::GTK::GTKPictureBox*)user_data;
	UI::GUIControl::MouseButton btn = UI::GUIControl::MBTN_LEFT;
	guint button = gtk_gesture_single_get_current_button (GTK_GESTURE_SINGLE (gesture));
	switch (button)
	{
		default:
		case GDK_BUTTON_PRIMARY:
			btn = UI::GUIControl::MBTN_LEFT;
			break;
		case GDK_BUTTON_MIDDLE:
			btn = UI::GUIControl::MBTN_MIDDLE;
			break;
		case GDK_BUTTON_SECONDARY:
			btn = UI::GUIControl::MBTN_RIGHT;
			break;
		case 4:
			btn = UI::GUIControl::MBTN_X1;
			break;
		case 5:
			btn = UI::GUIControl::MBTN_X2;
			break;
	}
	me->EventButtonUp(Math::Coord2D<IntOS>(Double2IntOS(x), Double2IntOS(y)), btn);
}

void GTKPictureBox_SignalMotion(GtkEventControllerMotion *motion_ctrl, double x, double y, gpointer user_data)
{
	UI::GTK::GTKPictureBox *me = (UI::GTK::GTKPictureBox*)user_data;
	me->EventMouseMove(Math::Coord2D<IntOS>(Double2IntOS(x), Double2IntOS(y)));
}
#else
gboolean GTKPictureBox_SignalButtonPress(GtkWidget *widget, GdkEvent  *event, gpointer user_data)
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
		me->EventButtonDown(Math::Coord2D<IntOS>(Double2IntOS(evt->x), Double2IntOS(evt->y)), btn);
	}
	return false;
}

gboolean GTKPictureBox_SignalButtonRelease(GtkWidget *widget, GdkEvent  *event, gpointer user_data)
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
		me->EventButtonUp(Math::Coord2D<IntOS>(Double2IntOS(evt->x), Double2IntOS(evt->y)), btn);
	}
	return false;
}

gboolean GTKPictureBox_SignalMotionNotify(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUIPictureBox *me = (UI::GUIPictureBox*)data;
	GdkEventMotion *evt = (GdkEventMotion*)event;
	me->EventMouseMove(Math::Coord2D<IntOS>(Double2IntOS(evt->x), Double2IntOS(evt->y)));
	return false;
}
#endif

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
				GdkPixbuf *buf = gdk_pixbuf_new_from_data(tmpImage->data.Ptr(), GDK_COLORSPACE_RGB, tmpImage->info.storeBPP == 32, 8, (int)(IntOS)tmpImage->info.dispSize.x, (int)(IntOS)tmpImage->info.dispSize.y, (int)(IntOS)tmpImage->info.storeSize.x << 2, 0, 0);
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
			GdkPixbuf *buf = gdk_pixbuf_new_from_data(img->data.Ptr(), GDK_COLORSPACE_RGB, img->info.storeBPP == 32, 8, (int)(IntOS)img->info.dispSize.x, (int)(IntOS)img->info.dispSize.y, (int)(IntOS)img->info.storeSize.x << 2, 0, 0);
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
	this->tmpImage = nullptr;
	this->gtkImage = gtk_image_new_from_pixbuf(0);
#if GTK_CHECK_VERSION(4, 0, 0)
    gtk_widget_set_can_target (this->gtkImage, TRUE);
    gtk_widget_set_focusable (this->gtkImage, TRUE);
	this->eventBox = gtk_gesture_click_new();
	gtk_gesture_single_set_button (GTK_GESTURE_SINGLE (this->eventBox), 0);
	g_signal_connect (this->eventBox, "pressed", G_CALLBACK (GTKPictureBox_SignalButtonPress), this);
	g_signal_connect (this->eventBox, "released", G_CALLBACK (GTKPictureBox_SignalButtonRelease), this);
	gtk_widget_add_controller (this->gtkImage, GTK_EVENT_CONTROLLER (this->eventBox));

	this->eventBox2 = gtk_event_controller_motion_new ();
	g_signal_connect (this->eventBox2, "motion", G_CALLBACK (GTKPictureBox_SignalMotion), this);
	gtk_widget_add_controller (this->gtkImage, GTK_EVENT_CONTROLLER (this->eventBox2));

#else
	this->eventBox = gtk_event_box_new ();
	gtk_container_add(GTK_CONTAINER(this->eventBox), this->gtkImage);

	g_signal_connect(G_OBJECT(this->eventBox), "button-press-event", G_CALLBACK(GTKPictureBox_SignalButtonPress), this);
	g_signal_connect(G_OBJECT(this->eventBox), "button-release-event", G_CALLBACK(GTKPictureBox_SignalButtonRelease), this);
	g_signal_connect(G_OBJECT(this->eventBox), "motion-notify-event", G_CALLBACK(GTKPictureBox_SignalMotionNotify), this);

	this->hwnd = (ControlHandle*)this->eventBox;
#endif
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

IntOS UI::GTK::GTKPictureBox::OnNotify(UInt32 code, IntOS lParam)
{
	return 0;
}
