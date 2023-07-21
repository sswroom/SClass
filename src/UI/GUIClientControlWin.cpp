#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Math/Math.h"
#include "UI/GUIClientControl.h"
#include <windows.h>

UI::GUIClientControl::GUIClientControl(NotNullPtr<GUICore> ui, UI::GUIClientControl *parent) : UI::GUIControl(ui, parent)
{
	this->hasFillCtrl = false;
	this->undockLeft = 0;
	this->undockRight = 0;
	this->undockTop = 0;
	this->undockBottom = 0;
}

UI::GUIClientControl::~GUIClientControl()
{
	this->ClearChildren();
}

void UI::GUIClientControl::UpdateFont()
{
	UOSInt i;
	UOSInt j;
	void *font = GetFont();
	if (font)
	{
		SendMessage((HWND)this->hwnd, WM_SETFONT, (WPARAM)font, TRUE);
	}
	i = 0;
	j = this->children.GetCount();
	while (i < j)
	{
		this->children.GetItem(i)->UpdateFont();
		i++;
	}
}

void UI::GUIClientControl::ClearChildren()
{
	UOSInt i = this->children.GetCount();
	while (i-- > 0)
	{
		GUIControl *ctrl = this->children.RemoveAt(i);
		ctrl->DestroyObject();
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
	rc.left = Double2Int32(this->undockLeft * this->hdpi / this->ddpi);
	rc.top = Double2Int32(this->undockTop * this->hdpi / this->ddpi);
	rc.right = Double2Int32(this->undockRight * this->hdpi / this->ddpi);
	rc.bottom = Double2Int32(this->undockBottom * this->hdpi / this->ddpi);
	FillRect((HDC)hdc, &rc, (HBRUSH)hbr);
	return true;
}

Math::Coord2DDbl UI::GUIClientControl::GetClientOfst()
{
	return Math::Coord2DDbl(0, 0);
}

Math::Size2DDbl UI::GUIClientControl::GetClientSize()
{
	RECT rc;
	GetClientRect((HWND)this->hwnd, &rc);
	return Math::Size2DDbl(rc.right - rc.left, rc.bottom - rc.top) * this->ddpi / this->hdpi;
}

void UI::GUIClientControl::AddChild(GUIControl *child)
{
	this->children.Add(child);
}

UOSInt UI::GUIClientControl::GetChildCount()
{
	return this->children.GetCount();
}

UI::GUIControl *UI::GUIClientControl::GetChild(UOSInt index)
{
	return this->children.GetItem(index);
}

void UI::GUIClientControl::UpdateChildrenSize(Bool redraw)
{
	UOSInt i;
	UOSInt j;
	Double left = 0;
	Double top = 0;
	Math::Size2DDbl br;
	Math::Size2DDbl ctrlSz;
	Bool hasFill = false;
	GUIControl *ctrl;
	DockType dt;

	br = GetClientSize();
	i = 0;
	j = this->children.GetCount();
	while (i < j)
	{
		ctrl = this->children.GetItem(i);
		dt = ctrl->GetDockType();
		if (dt == UI::GUIControl::DOCK_NONE)
		{
			ctrl->UpdatePos(false);
		}
		else if (dt == UI::GUIControl::DOCK_FILL)
		{
			ctrl->SetArea(left, top, br.x, br.y, false);
			ctrlSz = ctrl->GetSize();
			//top += ctrlH;
			hasFill = true;
		}
		else
		{
			ctrlSz = ctrl->GetSize();
			if (dt == UI::GUIControl::DOCK_LEFT)
			{
				ctrl->SetArea(left, top, left + ctrlSz.x, br.y, false);
				ctrlSz = ctrl->GetSize();
				left += ctrlSz.x;
			}
			else if (dt == UI::GUIControl::DOCK_TOP)
			{
				ctrl->SetArea(left, top, br.x, top + ctrlSz.y, false);
				ctrlSz = ctrl->GetSize();
				top += ctrlSz.y;
			}
			else if (dt == UI::GUIControl::DOCK_RIGHT)
			{
				ctrl->SetArea(br.x - ctrlSz.x, top, br.x, br.y, false);
				ctrlSz = ctrl->GetSize();
				br.x -= ctrlSz.x;
			}
			else if (dt == UI::GUIControl::DOCK_BOTTOM)
			{
				ctrl->SetArea(left, br.y - ctrlSz.y, br.x, br.y, false);
				ctrlSz = ctrl->GetSize();
				br.y -= ctrlSz.y;
			}
		}

		i++;
	}
	this->undockLeft = left;
	this->undockTop = top;
	this->undockRight = br.x;
	this->undockBottom = br.y;
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
	if (hMon != (HMONITOR)this->currHMon)
	{
		this->currHMon = (MonitorHandle*)hMon;
		this->OnMonitorChanged();
	}
	UOSInt i = this->resizeHandlers.GetCount();
	while (i-- > 0)
	{
		this->resizeHandlers.GetItem(i)(this->resizeHandlersObjs.GetItem(i));
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
		this->children.GetItem(i)->SetDPI(hdpi, ddpi);
	}
	this->UpdateChildrenSize(true);
}

void UI::GUIClientControl::DestroyObject()
{
	this->ClearChildren();
}
