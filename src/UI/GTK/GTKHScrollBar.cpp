#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/GTK/GTKHScrollBar.h"

gboolean UI::GTK::GTKHScrollBar::SignalValueChanged(void *window, void *userObj)
{
	UI::GUIHScrollBar *me = (UI::GUIHScrollBar*)userObj;
	me->EventPosChanged(me->GetPos());
	return FALSE;
}

UI::GTK::GTKHScrollBar::GTKHScrollBar(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Double width) : UI::GUIHScrollBar(ui, parent)
{
	this->hwnd = (ControlHandle*)gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, 0);
	g_signal_connect((GtkButton*)this->hwnd, "value-changed", G_CALLBACK(SignalValueChanged), this);
	parent->AddChild(*this);
	this->Show();
}

UI::GTK::GTKHScrollBar::~GTKHScrollBar()
{
}

void UI::GTK::GTKHScrollBar::InitScrollBar(UOSInt minVal, UOSInt maxVal, UOSInt currVal, UOSInt largeChg)
{
	GtkAdjustment *adj = gtk_range_get_adjustment((GtkRange*)this->hwnd);
	gtk_adjustment_configure(adj, UOSInt2Double(currVal), UOSInt2Double(minVal), UOSInt2Double(maxVal), 1, UOSInt2Double(largeChg), UOSInt2Double(largeChg));
	this->EventPosChanged(this->GetPos());
}

void UI::GTK::GTKHScrollBar::SetPos(UOSInt pos)
{
	GtkAdjustment *adj = gtk_range_get_adjustment((GtkRange*)this->hwnd);
	gtk_adjustment_set_value(adj, UOSInt2Double(pos));
	this->EventPosChanged(this->GetPos());
}

UOSInt UI::GTK::GTKHScrollBar::GetPos()
{
	GtkAdjustment *adj = gtk_range_get_adjustment((GtkRange*)this->hwnd);
	return (UOSInt)Double2OSInt(gtk_adjustment_get_value(adj));
}

void UI::GTK::GTKHScrollBar::SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn)
{
	if (left == this->lxPos && top == this->lyPos && right == this->lxPos2 && bottom == this->lyPos2)
		return;
	Math::Coord2DDbl ofst = Math::Coord2DDbl(0, 0);
	NotNullPtr<UI::GUIClientControl> nnparent;
	if (this->parent.SetTo(nnparent))
	{
		ofst = nnparent->GetClientOfst();
	}
	this->lxPos = left;
	this->lyPos = top;
	this->selfResize = true;

	if (this->parent.SetTo(nnparent))
	{
		void *container = nnparent->GetContainer();
		gtk_fixed_move((GtkFixed*)container, (GtkWidget*)this->hwnd, Double2Int32((left + ofst.x) * this->hdpi / this->ddpi), Double2Int32((top + ofst.y) * this->hdpi / this->ddpi));
	}
	if (right < left)
	{
		right = left;
	}
	if (bottom < top)
	{
		bottom = top;
	}
	gtk_widget_set_size_request((GtkWidget*)this->hwnd, Double2Int32((right - left) * this->hdpi / this->ddpi), Double2Int32((bottom - top) * this->hdpi / this->ddpi));

	gint outW;
	gint outH;
	gtk_widget_get_size_request((GtkWidget*)this->hwnd, &outW, &outH);
	if (outW == -1)
	{
		this->lxPos2 = right;
	}
	else
	{
		this->lxPos2 = left + outW * this->ddpi / this->hdpi;
	}
	if (outH == -1)
	{
		this->lyPos2 = bottom;
	}
	else
	{
		this->lyPos2 = top + outH * this->ddpi / this->hdpi;
	}
	this->selfResize = false;
	this->OnSizeChanged(updateScn);
}

void UI::GTK::GTKHScrollBar::SetAreaP(OSInt left, OSInt top, OSInt right, OSInt bottom, Bool updateScn)
{
	if (OSInt2Double(left) == this->lxPos && OSInt2Double(top) == this->lyPos && OSInt2Double(right) == this->lxPos2 && OSInt2Double(bottom) == this->lyPos2)
		return;
	Math::Coord2DDbl ofst = Math::Coord2DDbl(0, 0);
	NotNullPtr<UI::GUIClientControl> nnparent;
	if (this->parent.SetTo(nnparent))
	{
		ofst = nnparent->GetClientOfst();
	}
	this->lxPos = OSInt2Double(left) * this->ddpi / this->hdpi;
	this->lyPos = OSInt2Double(top) * this->ddpi / this->hdpi;
	this->selfResize = true;

	if (this->parent.SetTo(nnparent))
	{
		void *container = nnparent->GetContainer();
		gtk_fixed_move((GtkFixed*)container, (GtkWidget*)this->hwnd, Double2Int32(OSInt2Double(left) + ofst.x * this->hdpi / this->ddpi), Double2Int32(OSInt2Double(top) + ofst.y * this->hdpi / this->ddpi));
	}
	if (right < left)
	{
		right = left;
	}
	if (bottom < top)
	{
		bottom = top;
	}
	gtk_widget_set_size_request((GtkWidget*)this->hwnd, (gint)(right - left), (gint)(bottom - top));

	gint outW;
	gint outH;
	gtk_widget_get_size_request((GtkWidget*)this->hwnd, &outW, &outH);
	if (outW == -1)
	{
		this->lxPos2 = OSInt2Double(right) * this->ddpi / this->hdpi;
	}
	else
	{
		this->lxPos2 = OSInt2Double(left + outW) * this->ddpi / this->hdpi;
	}
	if (outH == -1)
	{
		this->lyPos2 = OSInt2Double(bottom) * this->ddpi / this->hdpi;
	}
	else
	{
		this->lyPos2 = OSInt2Double(top + outH) * this->ddpi / this->hdpi;
	}
	this->selfResize = false;
	this->OnSizeChanged(updateScn);
}

OSInt UI::GTK::GTKHScrollBar::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GTK::GTKHScrollBar::UpdatePos(Bool redraw)
{
/*	WINDOWINFO wi;
	wi.cbSize = sizeof(WINDOWINFO);
	if (GetWindowInfo((HWND)hwnd, &wi))
	{
		if (wi.dwStyle & (WS_MAXIMIZE | WS_MINIMIZE))
		{
			return;
		}
	}
	Int32 minSize = GetSystemMetrics(SM_CYHSCROLL) + 1;
	Int32 newSize;

	if (this->parent)
	{
		Math::Coord2DDbl ofst = Math::Coord2DDbl(0, 0);
		ofst = this->parent->GetClientOfst();
		newSize = Double2Int32((this->lyPos2 - this->lyPos) * this->hdpi / this->ddpi);
		if (newSize < minSize)
			newSize = minSize;
		MoveWindow((HWND)hwnd, Double2Int32((this->lxPos + ofst.x) * this->hdpi / this->ddpi), Double2Int32((this->lyPos + ofst.y) * this->hdpi / this->ddpi), Double2Int32((this->lxPos2 - this->lxPos) * this->hdpi / this->ddpi), newSize, redraw?TRUE:FALSE);
	}
	else
	{
		Double newW = (this->lxPos2 - this->lxPos) * this->hdpi / this->ddpi;
		Double newH = (this->lyPos2 - this->lyPos) * this->hdpi / this->ddpi;
		int maxX = GetSystemMetrics(SM_CXSCREEN);
		int maxY = GetSystemMetrics(SM_CYSCREEN);
		if (newW > maxX)
			newW = maxX;
		if (newH > maxY)
			newH = maxY;
		RECT rc;
		GetWindowRect((HWND)this->hwnd, &rc);
		Double newX = (rc.left + rc.right - newW) * 0.5;
		Double newY = (rc.top + rc.bottom - newH) * 0.5;
		if (newY < 0)
		{
			newY = 0;
		}
		newSize = Double2Int32(newH);
		if (newSize < minSize)
			newSize = minSize;
		MoveWindow((HWND)this->hwnd, Double2Int32(newX), Double2Int32(newY), Double2Int32(newW), newSize, redraw?TRUE:FALSE);
	}*/
}
