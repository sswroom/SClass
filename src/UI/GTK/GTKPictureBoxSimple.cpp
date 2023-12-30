#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Interlocked.h"
#include "Media/GTKDrawEngine.h"
#include "Media/ImageUtil.h"
#include "UI/GUIClientControl.h"
#include "UI/GTK/GTKPictureBoxSimple.h"

void UI::GTK::GTKPictureBoxSimple::UpdatePreview()
{
	gtk_image_clear((GtkImage*)this->hwnd);
	if (this->pixbuf)
	{
		g_object_unref(this->pixbuf);
		this->pixbuf = 0;
	}
	if (this->tmpImage)
	{
		DEL_CLASS(this->tmpImage);
		this->tmpImage = 0;
	}
	NotNullPtr<Media::StaticImage> simg;
	if (this->currImage.SetTo(simg))
	{
		GdkPixbuf *buf;
		this->tmpImage = (Media::StaticImage*)simg->Clone();
		this->tmpImage->To32bpp();
		ImageUtil_SwapRGB(this->tmpImage->data, this->tmpImage->info.storeSize.x * this->tmpImage->info.storeSize.y, this->tmpImage->info.storeBPP);
		if (this->tmpImage->info.atype == Media::AT_ALPHA)
		{
			buf = gdk_pixbuf_new_from_data(this->tmpImage->data, GDK_COLORSPACE_RGB, true, 8, (int)(UInt32)this->tmpImage->info.dispSize.x, (int)(UInt32)this->tmpImage->info.dispSize.y, (int)(UInt32)(this->tmpImage->info.storeSize.x << 2), 0, 0);
		}
		else if (simg->info.atype == Media::AT_PREMUL_ALPHA)
		{
			buf = gdk_pixbuf_new_from_data(this->tmpImage->data, GDK_COLORSPACE_RGB, true, 8, (int)(UInt32)this->tmpImage->info.dispSize.x, (int)(UInt32)this->tmpImage->info.dispSize.y, (int)(UInt32)(this->tmpImage->info.storeSize.x << 2), 0, 0);
		}
		else
		{
			buf = gdk_pixbuf_new_from_data(this->tmpImage->data, GDK_COLORSPACE_RGB, true, 8, (int)(UInt32)this->tmpImage->info.dispSize.x, (int)(UInt32)this->tmpImage->info.dispSize.y, (int)(UInt32)(this->tmpImage->info.storeSize.x << 2), 0, 0);
		}
		this->pixbuf = buf;
		gtk_image_set_from_pixbuf((GtkImage*)this->hwnd, buf);
	}
	else if (this->prevImageD)
	{
		gtk_image_set_from_surface((GtkImage*)this->hwnd, (cairo_surface_t*)((Media::GTKDrawImage*)this->prevImageD)->GetSurface());
	}
	this->Redraw();
}

UI::GTK::GTKPictureBoxSimple::GTKPictureBoxSimple(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, NotNullPtr<Media::DrawEngine> eng, Bool hasBorder) : UI::GUIPictureBoxSimple(ui, parent, eng, hasBorder)
{
	this->currImage = 0;
	this->prevImageD = 0;

	this->pixbuf = 0;
	this->tmpImage = 0;

	this->hwnd = (ControlHandle*)gtk_image_new();
	parent->AddChild(*this);
	this->Show();
}

UI::GTK::GTKPictureBoxSimple::~GTKPictureBoxSimple()
{
	NotNullPtr<Media::DrawImage> img;
	if (img.Set(this->prevImageD))
	{
		this->eng->DeleteImage(img);
		this->prevImageD = 0;
	}
	if (this->pixbuf)
	{
		g_object_unref(this->pixbuf);
		this->pixbuf = 0;
	}
	if (this->tmpImage)
	{
		DEL_CLASS(this->tmpImage);
		this->tmpImage = 0;
	}
}

OSInt UI::GTK::GTKPictureBoxSimple::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GTK::GTKPictureBoxSimple::SetImage(Optional<Media::StaticImage> currImage)
{
	this->currImage = currImage;
	this->UpdatePreview();
}

void UI::GTK::GTKPictureBoxSimple::SetImageDImg(Media::DrawImage *img)
{
	this->currImage = 0;
	NotNullPtr<Media::DrawImage> imgnn;
	if (imgnn.Set(this->prevImageD))
	{
		this->eng->DeleteImage(imgnn);
		this->prevImageD = 0;
	}
	if (imgnn.Set(img))
	{
		this->prevImageD = this->eng->CloneImage(imgnn);
		if (this->prevImageD == 0)
		{
		}
		else
		{
			gtk_image_set_from_surface((GtkImage*)this->hwnd, (cairo_surface_t*)((Media::GTKDrawImage*)this->prevImageD)->GetSurface());
		}
	}
	this->Redraw();
}
