#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.hpp"
#include "Math/Math_C.h"
#include "UI/GUIClientControl.h"
#include <windows.h>

UI::GUIClientControl::GUIClientControl(NN<GUICore> ui, Optional<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
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
	void *font = GetFont();
	if (font)
	{
		SendMessage((HWND)this->hwnd.OrNull(), WM_SETFONT, (WPARAM)font, TRUE);
	}
	Data::ArrayIterator<NN<UI::GUIControl>> it = this->children.Iterator();
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
	GetClientRect((HWND)this->hwnd.OrNull(), &rc);
	return Math::Size2DDbl(rc.right - rc.left, rc.bottom - rc.top) * this->ddpi / this->hdpi;
}

void UI::GUIClientControl::AddChild(NN<GUIControl> child)
{
	this->children.Add(child);
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

void UI::GUIClientControl::UpdateChildrenSize(Bool redraw)
{
	Double left = 0;
	Double top = 0;
	Math::Size2DDbl br;
	Math::Size2DDbl ctrlSz;
	Bool hasFill = false;
	NN<GUIControl> ctrl;
	DockType dt;

	br = GetClientSize();
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
	}
	this->undockLeft = left;
	this->undockTop = top;
	this->undockRight = br.x;
	this->undockBottom = br.y;
	this->hasFillCtrl = hasFill;
	if (redraw && this->children.GetCount() > 0 && this->hwnd.NotNull())
	{
		InvalidateRect((HWND)this->hwnd.OrNull(), 0, TRUE);
	}
}

void UI::GUIClientControl::OnSizeChanged(Bool updateScn)
{
	if (this->selfResize)
	{
		return;
	}
	HMONITOR hMon = MonitorFromWindow((HWND)this->hwnd.OrNull(), MONITOR_DEFAULTTONEAREST);
	if (hMon != (HMONITOR)this->currHMon.OrNull())
	{
		this->currHMon = (MonitorHandle*)hMon;
		this->OnMonitorChanged();
	}
	UOSInt i = this->resizeHandlers.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UI::UIEvent> cb = this->resizeHandlers.GetItem(i);
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

void UI::GUIClientControl::DestroyObject()
{
	this->ClearChildren();
}
