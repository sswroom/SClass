#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/GTK/GTKVSplitter.h"

gboolean UI::GTK::GTKVSplitter::SignalMouseDown(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GTK::GTKVSplitter *me = (UI::GTK::GTKVSplitter*)data;
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
			me->EventMouseDown(btn, Math::Coord2D<OSInt>(Double2OSInt(evt->x), Double2OSInt(evt->y)));
		}
	}
	return false;
}

gboolean UI::GTK::GTKVSplitter::SignalMouseUp(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GTK::GTKVSplitter *me = (UI::GTK::GTKVSplitter*)data;
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
			me->EventMouseUp(btn, Math::Coord2D<OSInt>(Double2OSInt(evt->x), Double2OSInt(evt->y)));
		}
	}
	return false;
}

UI::GTK::GTKVSplitter::GTKVSplitter(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Int32 height, Bool isBottom) : UI::GUIVSplitter(ui, parent)
{
	this->dragMode = false;
	this->isBottom = isBottom;
	this->hwnd = (ControlHandle*)gtk_drawing_area_new();
	g_signal_connect(G_OBJECT(this->hwnd), "button-press-event", G_CALLBACK(SignalMouseDown), this);
	g_signal_connect(G_OBJECT(this->hwnd), "button-release-event", G_CALLBACK(SignalMouseUp), this);
	gtk_widget_set_events((GtkWidget*)this->hwnd, GDK_ALL_EVENTS_MASK);
	parent->AddChild(*this);
	this->Show();

	this->SetRect(0, 0, 100, height, false);
	this->SetDockType(isBottom?DOCK_BOTTOM:DOCK_TOP);
}

UI::GTK::GTKVSplitter::~GTKVSplitter()
{
}

OSInt UI::GTK::GTKVSplitter::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GTK::GTKVSplitter::EventMouseDown(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos)
{
	if (btn == UI::GUIControl::MBTN_LEFT)
	{
		this->dragMode = true;
		this->dragX = pos.x;
		this->dragY = pos.y;
		this->SetCapture();
	}
}

void UI::GTK::GTKVSplitter::EventMouseUp(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos)
{
	if (btn == UI::GUIControl::MBTN_LEFT)
	{
		NotNullPtr<UI::GUIClientControl> nnparent;
		if (this->dragMode && this->parent.SetTo(nnparent))
		{
			NotNullPtr<UI::GUIControl> ctrl;
			Bool foundThis = false;
			OSInt drawY = pos.y - this->dragY;
			pos = this->GetPositionP();
			drawY += pos.y;
			Math::Size2D<UOSInt> sz;
			UOSInt i = nnparent->GetChildCount();
			while (i-- > 0)
			{
				if (nnparent->GetChild(i).SetTo(ctrl))
				{
					if (ctrl.Ptr() == this)
					{
						foundThis = true;
					}
					else if (foundThis)
					{
						dockType = ctrl->GetDockType();
						if (dockType == UI::GUIControl::DOCK_BOTTOM && this->isBottom)
						{
							pos = ctrl->GetPositionP();
							sz = ctrl->GetSizeP();
							ctrl->SetAreaP(pos.x, drawY, pos.x + (OSInt)sz.x, pos.y + (OSInt)sz.y, false);
							nnparent->UpdateChildrenSize(true);
							break;
						}
						else if (dockType == UI::GUIControl::DOCK_TOP && !this->isBottom)
						{
							pos = ctrl->GetPositionP();
							sz = ctrl->GetSizeP();
							ctrl->SetAreaP(pos.x, pos.y, pos.x + (OSInt)sz.x, drawY, false);
							nnparent->UpdateChildrenSize(true);
							break;
						}
					}
				}
			}
		}
		this->ReleaseCapture();
	}
}
