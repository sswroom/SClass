#include "Stdafx.h"
#include "MyMemory.h"
#include "Java/JavaContainer.h"
#include "Math/Math_C.h"
#include "UI/GUIClientControl.h"

void UI::GUIClientControl::InitContainer()
{
}

UI::GUIClientControl::GUIClientControl(NN<UI::GUICore> ui, Optional<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
{
	this->container = 0;
}

UI::GUIClientControl::~GUIClientControl()
{
	this->ClearChildren();
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
	j = this->children.GetCount();
	while (i < j)
	{
		this->children.GetItemNoCheck(i)->UpdateFont();
		i++;
	}
}

void UI::GUIClientControl::ClearChildren()
{
	OSInt i = this->children.GetCount();
	while (i-- > 0)
	{
		NN<GUIControl> ctrl;
		if (this->children.RemoveAt(i).SetTo(ctrl))
		{
			ctrl->DestroyObject();
			ctrl.Delete();
		}
	}
}


Math::Coord2DDbl UI::GUIClientControl::GetClientOfst()
{
	return Math::Coord2DDbl(0, 0);
}

Math::Size2DDbl UI::GUIClientControl::GetClientSize()
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
		return this->GetSize();
//	}
//	printf("ClientControl Client Size: %lf, %lf\r\n", *w, *h);
}

void UI::GUIClientControl::AddChild(NN<GUIControl> child)
{
	NN<Java::JavaContainer> container;
	NN<Java::JavaComponent> component;
	if (Optional<Java::JavaComponent>::ConvertFrom(child->GetHandle()).SetTo(component) && this->container.GetOpt<Java::JavaContainer>().SetTo(container))
	{
		container->Add(component);
		this->selfResize = false;
		child->SetDPI(this->hdpi, this->ddpi);
	}
}

UOSInt UI::GUIClientControl::GetChildCount() const
{
	return this->children.GetCount();
}

Optional<UI::GUIControl> UI::GUIClientControl::GetChild(UOSInt index) const
{
	return this->children.GetItem(index);
}

void UI::GUIClientControl::FocusChild(GUIControl *child)
{
	NN<GUIClientControl> parent;
	if (this->parent.SetTo(parent))
	{
		parent->FocusChild(child);
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
	Math::Coord2DDbl br;
	Math::Size2DDbl ctrlSize;
	Bool hasFill = false;
	NN<GUIControl> ctrl;
	DockType dt;

	this->selfResize = true;
	br = GetClientSize();
//	printf("UpdateChildrenSize: %lf, %lf, %lf, %lf\r\n", right, bottom, this->hdpi, this->ddpi);
	i = 0;
	j = this->children.GetCount();
	while (i < j)
	{
		ctrl = this->children.GetItemNoCheck(i);
		dt = ctrl->GetDockType();
		if (dt == UI::GUIControl::DOCK_NONE)
		{
			ctrl->UpdatePos(false);
		}
		else if (dt == UI::GUIControl::DOCK_FILL)
		{
//			printf("UpdateChildrenSize: Fill: %lf, %lf, %lf, %lf\r\n", left, top, right, bottom);
			ctrl->SetArea(left, top, br.x, br.y, false);
			ctrlSize = ctrl->GetSize();
//			printf("UpdateChildrenSize: Size: %lf, %lf\r\n", ctrlW, ctrlH);
			//top += ctrlH;
			hasFill = true;
		}
		else
		{
			ctrlSize = ctrl->GetSize();
			if (dt == UI::GUIControl::DOCK_LEFT)
			{
//				printf("UpdateChildrenSize: Left: %lf, %lf, %lf, %lf\r\n", left, top, left + ctrlW, bottom);
				ctrl->SetArea(left, top, left + ctrlSize.x, br.y, false);
				ctrlSize = ctrl->GetSize();
//				printf("UpdateChildrenSize: Size: %lf, %lf\r\n", ctrlW, ctrlH);
				left += ctrlSize.x;
			}
			else if (dt == UI::GUIControl::DOCK_TOP)
			{
//				printf("UpdateChildrenSize: Top: %lf, %lf, %lf, %lf\r\n", left, top, right, top + ctrlH);
				ctrl->SetArea(left, top, br.x, top + ctrlSize.y, false);
				ctrlSize = ctrl->GetSize();
//				printf("UpdateChildrenSize: Size: %lf, %lf\r\n", ctrlW, ctrlH);
				top += ctrlSize.y;
			}
			else if (dt == UI::GUIControl::DOCK_RIGHT)
			{
//				printf("UpdateChildrenSize: Right: %lf, %lf, %lf, %lf\r\n", right - ctrlW, top, right, bottom);
				ctrl->SetArea(br.x - ctrlSize.x, top, br.x, br.y, false);
				ctrlSize = ctrl->GetSize();
//				printf("UpdateChildrenSize: Size: %lf, %lf\r\n", ctrlW, ctrlH);
				br.x -= ctrlSize.x;
			}
			else if (dt == UI::GUIControl::DOCK_BOTTOM)
			{
//				printf("UpdateChildrenSize: Bottom: %lf, %lf, %lf, %lf\r\n", left, bottom - ctrlH, right, bottom);
				ctrl->SetArea(left, br.y - ctrlSize.y, br.x, br.y , false);
				ctrlSize = ctrl->GetSize();
//				printf("UpdateChildrenSize: Size: %lf, %lf\r\n", ctrlW, ctrlH);
				br.y -= ctrlSize.y;
			}
		}

		i++;
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
	UOSInt i = this->resizeHandlers.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UIEvent> evt = this->resizeHandlers.GetItem(i);
		evt.func(evt.userObj);
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

	UOSInt i = this->children.GetCount();
	while (i-- > 0)
	{
		this->children.GetItemNoCheck(i)->SetDPI(hdpi, ddpi);
	}
	this->UpdateChildrenSize(true);
}

AnyType UI::GUIClientControl::GetContainer()
{
	return 0;
/*	if (this->container == 0) this->InitContainer();
	ClientControlData *data = (ClientControlData*)this->container;
	return data->container;*/
}

void UI::GUIClientControl::DestroyObject()
{
	this->ClearChildren();
}
