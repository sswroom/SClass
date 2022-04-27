#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIHSplitter.h"
#include <gtk/gtk.h>

gboolean GUIHSplitter_OnMouseDown(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUIHSplitter *me = (UI::GUIHSplitter*)data;
	GdkEventButton *evt = (GdkEventButton*)event;
	if (evt->type == GDK_BUTTON_PRESS)
	{
		UI::GUIControl::MouseButton btn;
		switch (evt->button)
		{
			default:
			case 1:
				btn = UI::GUIControl::MBTN_LEFT;
				break;
			case 2:
				btn = UI::GUIControl::MBTN_MIDDLE;
				break;
			case 3:
				btn = UI::GUIControl::MBTN_RIGHT;
				break;
			case 4:
				btn = UI::GUIControl::MBTN_X1;
				break;
			case 5:
				btn = UI::GUIControl::MBTN_X2;
				break;
		}
		if (btn == UI::GUIControl::MBTN_LEFT)
		{
			me->EventMouseDown(btn, Double2OSInt(evt->x), Double2OSInt(evt->y));
		}
	}
	return false;
}

gboolean GUIHSplitter_OnMouseUp(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUIHSplitter *me = (UI::GUIHSplitter*)data;
	GdkEventButton *evt = (GdkEventButton*)event;
	if (evt->type == GDK_BUTTON_RELEASE)
	{
		UI::GUIControl::MouseButton btn;
		switch (evt->button)
		{
			default:
			case 1:
				btn = UI::GUIControl::MBTN_LEFT;
				break;
			case 2:
				btn = UI::GUIControl::MBTN_MIDDLE;
				break;
			case 3:
				btn = UI::GUIControl::MBTN_RIGHT;
				break;
			case 4:
				btn = UI::GUIControl::MBTN_X1;
				break;
			case 5:
				btn = UI::GUIControl::MBTN_X2;
				break;
		}
		if (btn == UI::GUIControl::MBTN_LEFT)
		{
			me->EventMouseUp(btn, Double2OSInt(evt->x), Double2OSInt(evt->y));
		}
	}
	return false;
}

UI::GUIHSplitter::GUIHSplitter(UI::GUICore *ui, UI::GUIClientControl *parent, Int32 width, Bool isRight) : UI::GUIControl(ui, parent)
{
	this->dragMode = false;
	this->isRight = isRight;
	this->hwnd = (ControlHandle*)gtk_drawing_area_new();
	g_signal_connect(G_OBJECT(this->hwnd), "button-press-event", G_CALLBACK(GUIHSplitter_OnMouseDown), this);
	g_signal_connect(G_OBJECT(this->hwnd), "button-release-event", G_CALLBACK(GUIHSplitter_OnMouseUp), this);
	gtk_widget_set_events((GtkWidget*)this->hwnd, GDK_ALL_EVENTS_MASK);
	parent->AddChild(this);
	this->Show();

	this->SetRect(0, 0, width, 100, false);
	this->SetDockType(isRight?DOCK_RIGHT:DOCK_LEFT);
}

UI::GUIHSplitter::~GUIHSplitter()
{
}

Text::CString UI::GUIHSplitter::GetObjectClass()
{
	return CSTR("HSplitter");
}

OSInt UI::GUIHSplitter::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUIHSplitter::EventMouseDown(UI::GUIControl::MouseButton btn, OSInt x, OSInt y)
{
	if (btn == UI::GUIControl::MBTN_LEFT)
	{
		this->dragMode = true;
		this->dragX = (Int32)x;
		this->dragY = (Int32)y;
		this->SetCapture();
	}
}

void UI::GUIHSplitter::EventMouseUp(UI::GUIControl::MouseButton btn, OSInt x, OSInt y)
{
	if (btn == UI::GUIControl::MBTN_LEFT)
	{
		if (this->dragMode)
		{
			UI::GUIControl *ctrl;
			Bool foundThis = false;
			OSInt drawX = x - this->dragX;
			this->GetPositionP(&x, &y);
			drawX += x;
			Math::Size2D<UOSInt> sz;
			UOSInt i = this->parent->GetChildCount();
			while (i-- > 0)
			{
				ctrl = this->parent->GetChild(i);
				if (ctrl == this)
				{
					foundThis = true;
				}
				else if (foundThis)
				{
					dockType = ctrl->GetDockType();
					if (dockType == UI::GUIControl::DOCK_RIGHT && this->isRight)
					{
						ctrl->GetPositionP(&x, &y);
						sz = ctrl->GetSizeP();
						ctrl->SetAreaP(drawX, y, x + (OSInt)sz.width, y + (OSInt)sz.height, false);
						this->parent->UpdateChildrenSize(true);
						break;
					}
					else if (dockType == UI::GUIControl::DOCK_LEFT && !this->isRight)
					{
						ctrl->GetPositionP(&x, &y);
						sz = ctrl->GetSizeP();
						ctrl->SetAreaP(x, y, drawX, y + (OSInt)sz.height, false);
						this->parent->UpdateChildrenSize(true);
						break;
					}
				}
			}
		}
		this->ReleaseCapture();
	}
}
