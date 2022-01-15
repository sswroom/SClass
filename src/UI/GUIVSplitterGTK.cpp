#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIVSplitter.h"
#include <gtk/gtk.h>


gboolean GUIVSplitter_OnMouseDown(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUIVSplitter *me = (UI::GUIVSplitter*)data;
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

gboolean GUIVSplitter_OnMouseUp(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUIVSplitter *me = (UI::GUIVSplitter*)data;
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

UI::GUIVSplitter::GUIVSplitter(UI::GUICore *ui, UI::GUIClientControl *parent, Int32 height, Bool isBottom) : UI::GUIControl(ui, parent)
{
	this->dragMode = false;
	this->isBottom = isBottom;
	this->hwnd = (ControlHandle*)gtk_drawing_area_new();
	g_signal_connect(G_OBJECT(this->hwnd), "button-press-event", G_CALLBACK(GUIVSplitter_OnMouseDown), this);
	g_signal_connect(G_OBJECT(this->hwnd), "button-release-event", G_CALLBACK(GUIVSplitter_OnMouseUp), this);
	gtk_widget_set_events((GtkWidget*)this->hwnd, GDK_ALL_EVENTS_MASK);
	parent->AddChild(this);
	this->Show();

	this->SetRect(0, 0, 100, height, false);
	this->SetDockType(isBottom?DOCK_BOTTOM:DOCK_TOP);
}

UI::GUIVSplitter::~GUIVSplitter()
{
}

const UTF8Char *UI::GUIVSplitter::GetObjectClass()
{
	return (const UTF8Char*)"VSplitter";
}

OSInt UI::GUIVSplitter::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUIVSplitter::EventMouseDown(UI::GUIControl::MouseButton btn, OSInt x, OSInt y)
{
	if (btn == UI::GUIControl::MBTN_LEFT)
	{
		this->dragMode = true;
		this->dragX = (Int32)x;
		this->dragY = (Int32)y;
		this->SetCapture();
	}
}

void UI::GUIVSplitter::EventMouseUp(UI::GUIControl::MouseButton btn, OSInt x, OSInt y)
{
	if (btn == UI::GUIControl::MBTN_LEFT)
	{
		if (this->dragMode)
		{
			UI::GUIControl *ctrl;
			Bool foundThis = false;
			OSInt drawY = y - this->dragY;
			this->GetPositionP(&x, &y);
			drawY += y;
			UOSInt w;
			UOSInt h;
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
					if (dockType == UI::GUIControl::DOCK_BOTTOM && this->isBottom)
					{
						ctrl->GetPositionP(&x, &y);
						ctrl->GetSizeP(&w, &h);
						ctrl->SetAreaP(x, drawY, x + (OSInt)w, y + (OSInt)h, false);
						this->parent->UpdateChildrenSize(true);
						break;
					}
					else if (dockType == UI::GUIControl::DOCK_TOP && !this->isBottom)
					{
						ctrl->GetPositionP(&x, &y);
						ctrl->GetSizeP(&w, &h);
						ctrl->SetAreaP(x, y, x + (OSInt)w, drawY, false);
						this->parent->UpdateChildrenSize(true);
						break;
					}
				}
			}
		}
		this->ReleaseCapture();
	}
}
