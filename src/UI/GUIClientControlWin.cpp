#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Math/Math.h"
#include "UI/GUIClientControl.h"
#include <windows.h>

UI::GUIClientControl::GUIClientControl(GUICore *ui, UI::GUIClientControl *parent) : UI::GUIControl(ui, parent)
{
	NEW_CLASS(this->children, Data::ArrayList<GUIControl*>());
	this->hasFillCtrl = false;
	this->undockLeft = 0;
	this->undockRight = 0;
	this->undockTop = 0;
	this->undockBottom = 0;
}

UI::GUIClientControl::GUIClientControl(Bool forVirtualUse) : UI::GUIControl(0, 0)
{
	NEW_CLASS(this->children, Data::ArrayList<GUIControl*>());
	this->parent = 0;
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
	void *font = GetFont();
	if (font)
	{
		SendMessage((HWND)this->hwnd, WM_SETFONT, (WPARAM)font, TRUE);
	}
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
		DEL_CLASS(ctrl);
	}
}

Bool UI::GUIClientControl::MyEraseBkg(void *hdc)
{
	if (this->hasFillCtrl)
		return true;
	if (this->undockRight == 0 && this->undockBottom == 0 && this->undockLeft == 0 && this->undockTop == 0)
		return false;
	HBRUSH hbr = (HBRUSH)this->GetBGBrush();
	if (hbr == 0)
	{
		hbr = ((HBRUSH)COLOR_BTNSHADOW);
	}
	RECT rc;
	rc.left = Math::Double2Int32(this->undockLeft * this->hdpi / this->ddpi);
	rc.top = Math::Double2Int32(this->undockTop * this->hdpi / this->ddpi);
	rc.right = Math::Double2Int32(this->undockRight * this->hdpi / this->ddpi);
	rc.bottom = Math::Double2Int32(this->undockBottom * this->hdpi / this->ddpi);
	FillRect((HDC)hdc, &rc, (HBRUSH)hbr);
	return true;
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
	RECT rc;
	GetClientRect((HWND)this->hwnd, &rc);
	if (w)
		*w = (rc.right - rc.left) * this->ddpi / this->hdpi;
	if (h)
		*h = (rc.bottom - rc.top) * this->ddpi / this->hdpi;
}

void UI::GUIClientControl::AddChild(GUIControl *child)
{
	this->children->Add(child);
}

OSInt UI::GUIClientControl::GetChildCount()
{
	return this->children->GetCount();
}

UI::GUIControl *UI::GUIClientControl::GetChild(OSInt index)
{
	return this->children->GetItem(index);
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

	GetClientSize(&right, &bottom);
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
			ctrl->SetArea(left, top, right, bottom, false);
			ctrl->GetSize(&ctrlW, &ctrlH);
			//top += ctrlH;
			hasFill = true;
		}
		else
		{
			ctrl->GetSize(&ctrlW, &ctrlH);
			if (dt == UI::GUIControl::DOCK_LEFT)
			{
				ctrl->SetArea(left, top, left + ctrlW, bottom, false);
				ctrl->GetSize(&ctrlW, &ctrlH);
				left += ctrlW;
			}
			else if (dt == UI::GUIControl::DOCK_TOP)
			{
				ctrl->SetArea(left, top, right, top + ctrlH, false);
				ctrl->GetSize(&ctrlW, &ctrlH);
				top += ctrlH;
			}
			else if (dt == UI::GUIControl::DOCK_RIGHT)
			{
				ctrl->SetArea(right - ctrlW, top, right, bottom, false);
				ctrl->GetSize(&ctrlW, &ctrlH);
				right -= ctrlW;
			}
			else if (dt == UI::GUIControl::DOCK_BOTTOM)
			{
				ctrl->SetArea(left, bottom - ctrlH, right, bottom, false);
				ctrl->GetSize(&ctrlW, &ctrlH);
				bottom -= ctrlH;
			}
		}

		i++;
	}
	this->undockLeft = left;
	this->undockTop = top;
	this->undockRight = right;
	this->undockBottom = bottom;
	this->hasFillCtrl = hasFill;
	if (redraw && j > 0 && this->hwnd)
	{
		InvalidateRect((HWND)this->hwnd, 0, TRUE);
	}
}

void UI::GUIClientControl::OnSizeChanged(Bool updateScn)
{
	if (this->selfResize)
	{
		return;
	}
	HMONITOR hMon = MonitorFromWindow((HWND)this->hwnd, MONITOR_DEFAULTTONEAREST);
	if (hMon != this->currHMon)
	{
		this->currHMon = hMon;
		this->OnMonitorChanged();
	}
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

