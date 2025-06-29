#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIForm.h"
#include <gtk/gtk.h>

typedef struct
{
	GtkWidget *container;
	GtkWidget *scrolledWin;
	GtkWidget *vbox;
} ClientControlData;

void UI::GUIClientControl::InitContainer()
{
	ClientControlData *data = MemAlloc(ClientControlData, 1);
	data->container = gtk_fixed_new();
	data->scrolledWin = gtk_scrolled_window_new(0, 0);
	data->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	this->container = data;
	gtk_container_add(GTK_CONTAINER(data->scrolledWin), data->container);
	gtk_box_pack_start(GTK_BOX(data->vbox), data->scrolledWin, true, true, 0);
	gtk_container_add(GTK_CONTAINER(this->hwnd.OrNull()), data->vbox);

	Math::Size2DDbl outSz = this->GetClientSize();
	if (outSz.x < 0)
	{
		outSz.x = 0;
	}
	if (outSz.y < 0)
	{
		outSz.y = 0;
	}
	if (!this->GetObjectClass().Equals(UTF8STRC("WinForm")))
	{
		gtk_widget_set_size_request(data->scrolledWin, Double2Int32(outSz.x * this->hdpi / this->ddpi), Double2Int32(outSz.y * this->hdpi / this->ddpi));
	}
	gtk_widget_set_size_request(data->container, Double2Int32(outSz.x * this->hdpi / this->ddpi), Double2Int32(outSz.y * this->hdpi / this->ddpi));

	gtk_widget_show(data->container);
	gtk_widget_show(data->vbox);
	gtk_widget_show(data->scrolledWin);
}

UI::GUIClientControl::GUIClientControl(NN<UI::GUICore> ui, Optional<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
{
	this->container = 0;
}

UI::GUIClientControl::~GUIClientControl()
{
	this->ClearChildren();
	if (this->container)
	{
		ClientControlData *data = (ClientControlData*)this->container;
		MemFree(data);
	}
}

void UI::GUIClientControl::UpdateFont()
{
//	void *font = GetFont();
/*	if (font)
	{
		SendMessage((HWND)this->hwnd, WM_SETFONT, (WPARAM)font, TRUE);
	}*/
	Data::ArrayIterator<NN<GUIControl>> it = this->children.Iterator();
	while (it.HasNext())
	{
		it.Next()->UpdateFont();
	}
}

void UI::GUIClientControl::ClearChildren()
{
	Data::ArrayIterator<NN<GUIControl>> it = this->children.Iterator();
	while (it.HasNext())
	{
		NN<GUIControl> ctrl = it.Next();
		ctrl->DestroyObject();
		ctrl.Delete();
	}
	this->children.Clear();
}


Math::Coord2DDbl UI::GUIClientControl::GetClientOfst()
{
	return Math::Coord2DDbl(0, 0);
}

Math::Size2DDbl UI::GUIClientControl::GetClientSize()
{
	return this->GetSize();
}

void UI::GUIClientControl::AddChild(NN<GUIControl> child)
{
	if (this->container == 0) InitContainer();
	this->selfResize = true;
	ClientControlData *data = (ClientControlData*)this->container;
	gtk_fixed_put((GtkFixed*)data->container, (GtkWidget*)child->GetHandle().OrNull(), 0, 0);
	this->children.Add(child);
	this->selfResize = false;
}

UOSInt UI::GUIClientControl::GetChildCount() const
{
	return this->children.GetCount();
}

Optional<UI::GUIControl> UI::GUIClientControl::GetChild(UOSInt index) const
{
	return this->children.GetItem(index);
}

Data::ArrayIterator<NN<UI::GUIControl>> UI::GUIClientControl::ChildIterator() const
{
	return this->children.Iterator();
}

void UI::GUIClientControl::FocusChild(GUIControl *child)
{
	NN<GUIClientControl> nnparent;
	if (this->parent.SetTo(nnparent))
	{
		nnparent->FocusChild(child);
	}
	else
	{
		gtk_window_set_focus((GtkWindow*)this->hwnd.OrNull(), (GtkWidget*)child->GetHandle().OrNull());
	}
}

void UI::GUIClientControl::UpdateChildrenSize(Bool redraw)
{
	Double left = 0;
	Double top = 0;
	Math::Coord2DDbl br;
	Math::Size2DDbl ctrlSz;
	Bool hasFill = false;
	NN<GUIControl> ctrl;
	DockType dt;

	this->selfResize = true;
	br = GetClientSize();
//	printf("UpdateChildrenSize: %lf, %lf, %lf, %lf\r\n", right, bottom, this->hdpi, this->ddpi);
	Data::ArrayIterator<NN<GUIControl>> it = this->children.Iterator();
	while (it.HasNext())
	{
		ctrl = it.Next();
		dt = ctrl->GetDockType();
		if (dt == UI::GUIControl::DOCK_NONE)
		{
			ctrl->UpdatePos(false);
		}
		else if (dt == UI::GUIControl::DOCK_FILL)
		{
//			printf("UpdateChildrenSize: %s Fill: %lf, %lf, %lf, %lf\r\n", ctrl->GetObjectClass(), left, top, right, bottom);
			ctrl->SetArea(left, top, br.x, br.y, false);
			ctrlSz = ctrl->GetSize();
//			printf("UpdateChildrenSize: %s Size: %lf, %lf\r\n", ctrl->GetObjectClass(), ctrlW, ctrlH);
			//top += ctrlH;
			hasFill = true;
		}
		else
		{
			ctrlSz = ctrl->GetSize();
			if (dt == UI::GUIControl::DOCK_LEFT)
			{
//				printf("UpdateChildrenSize: %s Left: %lf, %lf, %lf, %lf\r\n", ctrl->GetObjectClass(), left, top, left + ctrlW, bottom);
				ctrl->SetArea(left, top, left + ctrlSz.x, br.y, false);
				ctrlSz = ctrl->GetSize();
//				printf("UpdateChildrenSize: %s Size: %lf, %lf\r\n", ctrl->GetObjectClass(), ctrlW, ctrlH);
				left += ctrlSz.x;
			}
			else if (dt == UI::GUIControl::DOCK_TOP)
			{
//				printf("UpdateChildrenSize: %s Top: %lf, %lf, %lf, %lf\r\n", ctrl->GetObjectClass(), left, top, right, top + ctrlH);
				ctrl->SetArea(left, top, br.x, top + ctrlSz.y, false);
				ctrlSz = ctrl->GetSize();
//				printf("UpdateChildrenSize: %s Size: %lf, %lf\r\n", ctrl->GetObjectClass(), ctrlW, ctrlH);
				top += ctrlSz.y;
			}
			else if (dt == UI::GUIControl::DOCK_RIGHT)
			{
//				printf("UpdateChildrenSize: %s Right: %lf, %lf, %lf, %lf\r\n", ctrl->GetObjectClass(), right - ctrlW, top, right, bottom);
				ctrl->SetArea(br.x - ctrlSz.x, top, br.x, br.y, false);
				ctrlSz = ctrl->GetSize();
//				printf("UpdateChildrenSize: %s Size: %lf, %lf\r\n", ctrl->GetObjectClass(), ctrlW, ctrlH);
				br.x -= ctrlSz.x;
			}
			else if (dt == UI::GUIControl::DOCK_BOTTOM)
			{
//				printf("UpdateChildrenSize: %s Bottom: %lf, %lf, %lf, %lf\r\n", ctrl->GetObjectClass(), left, bottom - ctrlH, right, bottom);
				ctrl->SetArea(left, br.y - ctrlSz.y, br.x, br.y , false);
				ctrlSz = ctrl->GetSize();
//				printf("UpdateChildrenSize: %s Size: %lf, %lf\r\n", ctrl->GetObjectClass(), ctrlW, ctrlH);
				br.y -= ctrlSz.y;
			}
		}
	}
	this->selfResize = false;
	this->undockLeft = left;
	this->undockTop = top;
	this->undockRight = br.x;
	this->undockBottom = br.y;
	this->hasFillCtrl = hasFill;
/*	if (redraw && j > 0 && this->hwnd)
	{
		InvalidateRect((HWND)this->hwnd, 0, TRUE);
	}*/
}

void UI::GUIClientControl::OnSizeChanged(Bool updateScn)
{
	if (this->selfResize)
	{
		return;
	}

	gint outW;
	gint outH;
	gtk_widget_get_size_request((GtkWidget*)this->hwnd.OrNull(), &outW, &outH);
	if (outW != -1 && outH != -1)
	{
		if (outW < 0 || outH < 0)
		{
			outW = 0;
			outH = 0;
		}
		ClientControlData *data = (ClientControlData*)this->container;
		if (data)
		{
			gtk_widget_set_size_request(data->scrolledWin, outW, outH);
			gtk_widget_set_size_request(data->container, outW, outH);
		}
	}
	if (outW != -1)
	{
		this->lxPos2 = this->lxPos + outW * this->ddpi / this->hdpi;
	}
	if (outH != -1)
	{
		this->lyPos2 = this->lyPos + outH * this->ddpi / this->hdpi;
	}

	this->UpdateChildrenSize(false);
	UOSInt i = this->resizeHandlers.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UIEvent> cb = this->resizeHandlers.GetItem(i);
		cb.func(cb.userObj);
	}
}

void UI::GUIClientControl::SetDPI(Double hdpi, Double ddpi)
{
	this->hdpi = hdpi;
	this->ddpi = ddpi;
	this->UpdatePos(true);
	if (this->hFont)
	{
		this->InitFont();
	}
	else
	{
		this->UpdateFont();
	}

	Data::ArrayIterator<NN<GUIControl>> it = this->children.Iterator();
	while (it.HasNext())
	{
		it.Next()->SetDPI(hdpi, ddpi);
	}
	this->UpdateChildrenSize(true);
}

void *UI::GUIClientControl::GetContainer()
{
	if (this->container == 0) this->InitContainer();
	ClientControlData *data = (ClientControlData*)this->container;
	return data->container;
}

void UI::GUIClientControl::DestroyObject()
{
	this->ClearChildren();
}
