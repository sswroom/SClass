#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIHScrollBar.h"
#include <gtk/gtk.h>

Int32 UI::GUIHScrollBar::useCnt = 0;

gboolean GUIHScrollBar_ValueChanged(void *window, void *userObj)
{
	UI::GUIHScrollBar *me = (UI::GUIHScrollBar*)userObj;
	me->EventPosChanged();
	return FALSE;
}

OSInt __stdcall UI::GUIHScrollBar::FormWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam)
{
	return 0;
}

void UI::GUIHScrollBar::Init(void *hInst)
{
}

void UI::GUIHScrollBar::Deinit(void *hInst)
{
}

UI::GUIHScrollBar::GUIHScrollBar(UI::GUICore *ui, UI::GUIClientControl *parent, Int32 width) : UI::GUIControl(ui, parent)
{
	NEW_CLASS(this->posChgHdlrs, Data::ArrayList<PosChgEvent>());
	NEW_CLASS(this->posChgObjs, Data::ArrayList<void *>());

	this->hwnd = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, 0);
	g_signal_connect((GtkButton*)this->hwnd, "value-changed", G_CALLBACK(GUIHScrollBar_ValueChanged), this);
	parent->AddChild(this);
	this->Show();
}

UI::GUIHScrollBar::~GUIHScrollBar()
{
	DEL_CLASS(this->posChgHdlrs);
	DEL_CLASS(this->posChgObjs);
}

void UI::GUIHScrollBar::InitScrollBar(UOSInt minVal, UOSInt maxVal, UOSInt currVal, UOSInt largeChg)
{
	GtkAdjustment *adj = gtk_range_get_adjustment((GtkRange*)this->hwnd);
	gtk_adjustment_configure(adj, Math::UOSInt2Double(currVal), Math::UOSInt2Double(minVal), Math::UOSInt2Double(maxVal), 1, Math::UOSInt2Double(largeChg), Math::UOSInt2Double(largeChg));
	this->EventPosChanged();
}

void UI::GUIHScrollBar::SetPos(UOSInt pos)
{
	GtkAdjustment *adj = gtk_range_get_adjustment((GtkRange*)this->hwnd);
	gtk_adjustment_set_value(adj, Math::UOSInt2Double(pos));
	this->EventPosChanged();
}

UOSInt UI::GUIHScrollBar::GetPos()
{
	GtkAdjustment *adj = gtk_range_get_adjustment((GtkRange*)this->hwnd);
	return (UOSInt)Math::Double2OSInt(gtk_adjustment_get_value(adj));
}

void UI::GUIHScrollBar::SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn)
{
	if (left == this->lxPos && top == this->lyPos && right == this->lxPos2 && bottom == this->lyPos2)
		return;
	Double xOfst = 0;
	Double yOfst = 0;
	if (this->parent)
	{
		this->parent->GetClientOfst(&xOfst, &yOfst);
	}
	this->lxPos = left;
	this->lyPos = top;
	this->selfResize = true;

	if (this->parent)
	{
		void *container = this->parent->GetContainer();
		gtk_fixed_move((GtkFixed*)container, (GtkWidget*)this->hwnd, Math::Double2Int32((left + xOfst) * this->hdpi / this->ddpi), Math::Double2Int32((top + yOfst) * this->hdpi / this->ddpi));
	}
	gtk_widget_set_size_request((GtkWidget*)this->hwnd, Math::Double2Int32((right - left) * this->hdpi / this->ddpi), Math::Double2Int32((bottom - top) * this->hdpi / this->ddpi));

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

void UI::GUIHScrollBar::SetAreaP(OSInt left, OSInt top, OSInt right, OSInt bottom, Bool updateScn)
{
	if (Math::OSInt2Double(left) == this->lxPos && Math::OSInt2Double(top) == this->lyPos && Math::OSInt2Double(right) == this->lxPos2 && Math::OSInt2Double(bottom) == this->lyPos2)
		return;
	Double xOfst = 0;
	Double yOfst = 0;
	if (this->parent)
	{
		this->parent->GetClientOfst(&xOfst, &yOfst);
	}
	this->lxPos = Math::OSInt2Double(left) * this->ddpi / this->hdpi;
	this->lyPos = Math::OSInt2Double(top) * this->ddpi / this->hdpi;
	this->selfResize = true;

	if (this->parent)
	{
		void *container = this->parent->GetContainer();
		gtk_fixed_move((GtkFixed*)container, (GtkWidget*)this->hwnd, Math::Double2Int32(Math::OSInt2Double(left) + xOfst * this->hdpi / this->ddpi), Math::Double2Int32(Math::OSInt2Double(top) + yOfst * this->hdpi / this->ddpi));
	}
	gtk_widget_set_size_request((GtkWidget*)this->hwnd, (gint)(right - left), (gint)(bottom - top));

	gint outW;
	gint outH;
	gtk_widget_get_size_request((GtkWidget*)this->hwnd, &outW, &outH);
	if (outW == -1)
	{
		this->lxPos2 = Math::OSInt2Double(right) * this->ddpi / this->hdpi;
	}
	else
	{
		this->lxPos2 = Math::OSInt2Double(left + outW) * this->ddpi / this->hdpi;
	}
	if (outH == -1)
	{
		this->lyPos2 = Math::OSInt2Double(bottom) * this->ddpi / this->hdpi;
	}
	else
	{
		this->lyPos2 = Math::OSInt2Double(top + outH) * this->ddpi / this->hdpi;
	}
	this->selfResize = false;
	this->OnSizeChanged(updateScn);
}

const UTF8Char *UI::GUIHScrollBar::GetObjectClass()
{
	return (const UTF8Char*)"HScrollBar";
}

OSInt UI::GUIHScrollBar::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUIHScrollBar::UpdatePos(Bool redraw)
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
		Double xOfst = 0;
		Double yOfst = 0;
		this->parent->GetClientOfst(&xOfst, &yOfst);
		newSize = Math::Double2Int32((this->lyPos2 - this->lyPos) * this->hdpi / this->ddpi);
		if (newSize < minSize)
			newSize = minSize;
		MoveWindow((HWND)hwnd, Math::Double2Int32((this->lxPos + xOfst) * this->hdpi / this->ddpi), Math::Double2Int32((this->lyPos + yOfst) * this->hdpi / this->ddpi), Math::Double2Int32((this->lxPos2 - this->lxPos) * this->hdpi / this->ddpi), newSize, redraw?TRUE:FALSE);
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
		newSize = Math::Double2Int32(newH);
		if (newSize < minSize)
			newSize = minSize;
		MoveWindow((HWND)this->hwnd, Math::Double2Int32(newX), Math::Double2Int32(newY), Math::Double2Int32(newW), newSize, redraw?TRUE:FALSE);
	}*/
}

void UI::GUIHScrollBar::EventPosChanged()
{
	UOSInt newPos = this->GetPos();
	UOSInt i;
	i = this->posChgHdlrs->GetCount();
	while (i-- > 0)
	{
		this->posChgHdlrs->GetItem(i)(this->posChgObjs->GetItem(i), newPos);
	}
}

void UI::GUIHScrollBar::HandlePosChanged(PosChgEvent hdlr, void *userObj)
{
	this->posChgHdlrs->Add(hdlr);
	this->posChgObjs->Add(userObj);
}

Int32 UI::GUIHScrollBar::GetSystemSize()
{
	return 16;
}
