#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "UI/GUIClientControl.h"
#include <jni.h>


void UI::GUIClientControl::InitContainer()
{
	this->container = 0;
}

UI::GUIClientControl::GUIClientControl(UI::GUICore *ui, UI::GUIClientControl *parent) : UI::GUIControl(ui, parent)
{
	NEW_CLASS(this->children, Data::ArrayList<GUIControl*>());
	this->container = 0;
}

UI::GUIClientControl::~GUIClientControl()
{
	this->ClearChildren();
	DEL_CLASS(this->children);
}

void UI::GUIClientControl::UpdateFont()
{
	OSInt i;
	OSInt j;
//	void *font = GetFont();
/*	if (font)
	{
		SendMessage((HWND)this->hwnd, WM_SETFONT, (WPARAM)font, TRUE);
	}*/
	i = 0;
	j = this->children->GetCount();
	while (i < j)
	{
		this->children->GetItem(i)->UpdateFont();
		i++;
	}
}

void UI::GUIClientControl::ClearChildren()
{
	OSInt i = this->children->GetCount();
	while (i-- > 0)
	{
		GUIControl *ctrl = this->children->RemoveAt(i);
		ctrl->DestroyObject();
		DEL_CLASS(ctrl);
	}
}


void UI::GUIClientControl::GetClientOfst(Double *x, Double *y)
{
	if (x)
		*x = 0;
	if (y)
		*y = 0;
}

void UI::GUIClientControl::GetClientSize(Double *w, Double *h)
{
//	JNIEnv *env = this->ui->GetEnv();
//	this->hwnd 
//	Container container = JFrame.getContentPanee();
//  container.getWidth() * this->ddpi / this->hdpi;
//	GdkRectangle rect;
//	GdkWindow *wnd = gtk_widget_get_window((GtkWidget*)this->hwnd);
//	if (wnd)
//	{
//		gdk_window_get_frame_extents(wnd, &rect);
//		*w = rect.width * this->ddpi / this->hdpi;
//		*h = rect.height * this->ddpi / this->hdpi;
//	}
//	else
//	{
		this->GetSize(w, h);
//	}
//	printf("ClientControl Client Size: %lf, %lf\r\n", *w, *h);
}

void UI::GUIClientControl::AddChild(GUIControl *child)
{
/*	if (this->container == 0) InitContainer();
	this->selfResize = true;
	ClientControlData *data = (ClientControlData*)this->container;
	gtk_fixed_put((GtkFixed*)data->container, (GtkWidget*)child->GetHandle(), 0, 0);
	this->children->Add(child);
	this->selfResize = false;
	child->SetDPI(this->hdpi, this->ddpi);*/
}

OSInt UI::GUIClientControl::GetChildCount()
{
	return this->children->GetCount();
}

UI::GUIControl *UI::GUIClientControl::GetChild(OSInt index)
{
	return this->children->GetItem(index);
}

void UI::GUIClientControl::FocusChild(GUIControl *child)
{
	if (this->parent)
	{
		this->parent->FocusChild(child);
	}
	else
	{
//		gtk_window_set_focus((GtkWindow*)this->hwnd, (GtkWidget*)child->GetHandle());
	}
}

void UI::GUIClientControl::UpdateChildrenSize(Bool redraw)
{
	OSInt i;
	OSInt j;
	Double left = 0;
	Double top = 0;
	Double right;
	Double bottom;
	Double ctrlW;
	Double ctrlH;
	Bool hasFill = false;
	GUIControl *ctrl;
	DockType dt;

	this->selfResize = true;
	GetClientSize(&right, &bottom);
//	printf("UpdateChildrenSize: %lf, %lf, %lf, %lf\r\n", right, bottom, this->hdpi, this->ddpi);
	i = 0;
	j = this->children->GetCount();
	while (i < j)
	{
		ctrl = this->children->GetItem(i);
		dt = ctrl->GetDockType();
		if (dt == UI::GUIControl::DOCK_NONE)
		{
			ctrl->UpdatePos(false);
		}
		else if (dt == UI::GUIControl::DOCK_FILL)
		{
//			printf("UpdateChildrenSize: Fill: %lf, %lf, %lf, %lf\r\n", left, top, right, bottom);
			ctrl->SetArea(left, top, right, bottom, false);
			ctrl->GetSize(&ctrlW, &ctrlH);
//			printf("UpdateChildrenSize: Size: %lf, %lf\r\n", ctrlW, ctrlH);
			//top += ctrlH;
			hasFill = true;
		}
		else
		{
			ctrl->GetSize(&ctrlW, &ctrlH);
			if (dt == UI::GUIControl::DOCK_LEFT)
			{
//				printf("UpdateChildrenSize: Left: %lf, %lf, %lf, %lf\r\n", left, top, left + ctrlW, bottom);
				ctrl->SetArea(left, top, left + ctrlW, bottom, false);
				ctrl->GetSize(&ctrlW, &ctrlH);
//				printf("UpdateChildrenSize: Size: %lf, %lf\r\n", ctrlW, ctrlH);
				left += ctrlW;
			}
			else if (dt == UI::GUIControl::DOCK_TOP)
			{
//				printf("UpdateChildrenSize: Top: %lf, %lf, %lf, %lf\r\n", left, top, right, top + ctrlH);
				ctrl->SetArea(left, top, right, top + ctrlH, false);
				ctrl->GetSize(&ctrlW, &ctrlH);
//				printf("UpdateChildrenSize: Size: %lf, %lf\r\n", ctrlW, ctrlH);
				top += ctrlH;
			}
			else if (dt == UI::GUIControl::DOCK_RIGHT)
			{
//				printf("UpdateChildrenSize: Right: %lf, %lf, %lf, %lf\r\n", right - ctrlW, top, right, bottom);
				ctrl->SetArea(right - ctrlW, top, right, bottom, false);
				ctrl->GetSize(&ctrlW, &ctrlH);
//				printf("UpdateChildrenSize: Size: %lf, %lf\r\n", ctrlW, ctrlH);
				right -= ctrlW;
			}
			else if (dt == UI::GUIControl::DOCK_BOTTOM)
			{
//				printf("UpdateChildrenSize: Bottom: %lf, %lf, %lf, %lf\r\n", left, bottom - ctrlH, right, bottom);
				ctrl->SetArea(left, bottom - ctrlH, right, bottom , false);
				ctrl->GetSize(&ctrlW, &ctrlH);
//				printf("UpdateChildrenSize: Size: %lf, %lf\r\n", ctrlW, ctrlH);
				bottom -= ctrlH;
			}
		}

		i++;
	}
	this->selfResize = false;
	this->undockLeft = left;
	this->undockTop = top;
	this->undockRight = right;
	this->undockBottom = bottom;
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

/*	gint outW;
	gint outH;
	gtk_widget_get_size_request((GtkWidget*)this->hwnd, &outW, &outH);
	if (outW != -1 && outH != -1)
	{
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
	}*/

	this->UpdateChildrenSize(false);
	OSInt i = this->resizeHandlers->GetCount();
	while (i-- > 0)
	{
		this->resizeHandlers->GetItem(i)(this->resizeHandlersObjs->GetItem(i));
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

	OSInt i = this->children->GetCount();
	while (i-- > 0)
	{
		this->children->GetItem(i)->SetDPI(hdpi, ddpi);
	}
	this->UpdateChildrenSize(true);
}

void *UI::GUIClientControl::GetContainer()
{
/*	if (this->container == 0) this->InitContainer();
	ClientControlData *data = (ClientControlData*)this->container;
	return data->container;*/
}

void UI::GUIClientControl::DestroyObject()
{
	this->ClearChildren();
}
