#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Interlocked.h"
#include "Media/GTKDrawEngine.h"
#include "Media/ImageUtil.h"
#include "UI/GUIClientControl.h"
#include "UI/GTK/GTKPictureBoxSimple.h"

void UI::GTK::GTKPictureBoxSimple::UpdatePreview()
{
	gtk_image_clear((GtkImage*)this->hwnd.OrNull());
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
	NN<Media::StaticImage> simg;
	NN<Media::DrawImage> dimg;
	if (this->currImage.SetTo(simg))
	{
		GdkPixbuf *buf;
		this->tmpImage = (Media::StaticImage*)simg->Clone().Ptr();
		this->tmpImage->ToB8G8R8A8();
		ImageUtil_SwapRGB(this->tmpImage->data.Ptr(), this->tmpImage->info.storeSize.x * this->tmpImage->info.storeSize.y, this->tmpImage->info.storeBPP);
		if (this->tmpImage->info.atype == Media::AT_ALPHA)
		{
			buf = gdk_pixbuf_new_from_data(this->tmpImage->data.Ptr(), GDK_COLORSPACE_RGB, true, 8, (int)(UInt32)this->tmpImage->info.dispSize.x, (int)(UInt32)this->tmpImage->info.dispSize.y, (int)(UInt32)(this->tmpImage->info.storeSize.x << 2), 0, 0);
		}
		else if (simg->info.atype == Media::AT_PREMUL_ALPHA)
		{
			buf = gdk_pixbuf_new_from_data(this->tmpImage->data.Ptr(), GDK_COLORSPACE_RGB, true, 8, (int)(UInt32)this->tmpImage->info.dispSize.x, (int)(UInt32)this->tmpImage->info.dispSize.y, (int)(UInt32)(this->tmpImage->info.storeSize.x << 2), 0, 0);
		}
		else
		{
			buf = gdk_pixbuf_new_from_data(this->tmpImage->data.Ptr(), GDK_COLORSPACE_RGB, true, 8, (int)(UInt32)this->tmpImage->info.dispSize.x, (int)(UInt32)this->tmpImage->info.dispSize.y, (int)(UInt32)(this->tmpImage->info.storeSize.x << 2), 0, 0);
		}
		this->pixbuf = buf;
		gtk_image_set_from_pixbuf((GtkImage*)this->hwnd.OrNull(), buf);
	}
	else if (this->prevImageD.SetTo(dimg))
	{
		gtk_image_set_from_surface((GtkImage*)this->hwnd.OrNull(), (cairo_surface_t*)NN<Media::GTKDrawImage>::ConvertFrom(dimg)->GetSurface());
	}
	this->Redraw();
}

UI::GTK::GTKPictureBoxSimple::GTKPictureBoxSimple(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder) : UI::GUIPictureBoxSimple(ui, parent, eng, hasBorder)
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
	NN<Media::DrawImage> img;
	if (this->prevImageD.SetTo(img))
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

void UI::GTK::GTKPictureBoxSimple::SetImageDImg(Optional<Media::DrawImage> img)
{
	this->currImage = 0;
	NN<Media::DrawImage> imgnn;
	if (this->prevImageD.SetTo(imgnn))
	{
		this->eng->DeleteImage(imgnn);
		this->prevImageD = 0;
	}
	if (img.SetTo(imgnn))
	{
		this->prevImageD = this->eng->CloneImage(imgnn);
		if (!this->prevImageD.SetTo(imgnn))
		{
		}
		else
		{
			gtk_image_set_from_surface((GtkImage*)this->hwnd.OrNull(), (cairo_surface_t*)NN<Media::GTKDrawImage>::ConvertFrom(imgnn)->GetSurface());
		}
	}
	this->Redraw();
}
