#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/GTK/GTKHSplitter.h"

gboolean UI::GTK::GTKHSplitter::SignalMouseDown(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GTK::GTKHSplitter *me = (UI::GTK::GTKHSplitter*)data;
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

gboolean UI::GTK::GTKHSplitter::SignalMouseUp(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GTK::GTKHSplitter *me = (UI::GTK::GTKHSplitter*)data;
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

UI::GTK::GTKHSplitter::GTKHSplitter(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Int32 width, Bool isRight) : UI::GUIHSplitter(ui, parent)
{
	this->dragMode = false;
	this->isRight = isRight;
	this->hwnd = (ControlHandle*)gtk_drawing_area_new();
	g_signal_connect(G_OBJECT(this->hwnd.OrNull()), "button-press-event", G_CALLBACK(SignalMouseDown), this);
	g_signal_connect(G_OBJECT(this->hwnd.OrNull()), "button-release-event", G_CALLBACK(SignalMouseUp), this);
	gtk_widget_set_events((GtkWidget*)this->hwnd.OrNull(), GDK_ALL_EVENTS_MASK);
	parent->AddChild(*this);
	this->Show();

	this->SetRect(0, 0, width, 100, false);
	this->SetDockType(isRight?DOCK_RIGHT:DOCK_LEFT);
}

UI::GTK::GTKHSplitter::~GTKHSplitter()
{
}

OSInt UI::GTK::GTKHSplitter::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GTK::GTKHSplitter::EventMouseDown(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos)
{
	if (btn == UI::GUIControl::MBTN_LEFT)
	{
		this->dragMode = true;
		this->dragX = pos.x;
		this->dragY = pos.y;
		this->SetCapture();
	}
}

void UI::GTK::GTKHSplitter::EventMouseUp(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos)
{
	if (btn == UI::GUIControl::MBTN_LEFT)
	{
		NN<GUIClientControl> nnparent;
		if (this->dragMode && this->parent.SetTo(nnparent))
		{
			NN<UI::GUIControl> ctrl;
			Bool foundThis = false;
			OSInt drawX = pos.x - this->dragX;
			pos = this->GetPositionP();
			drawX += pos.x;
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
						if (dockType == UI::GUIControl::DOCK_RIGHT && this->isRight)
						{
							pos = ctrl->GetPositionP();
							sz = ctrl->GetSizeP();
							ctrl->SetAreaP(drawX, pos.y, pos.x + (OSInt)sz.x, pos.y + (OSInt)sz.y, false);
							nnparent->UpdateChildrenSize(true);
							break;
						}
						else if (dockType == UI::GUIControl::DOCK_LEFT && !this->isRight)
						{
							pos = ctrl->GetPositionP();
							sz = ctrl->GetSizeP();
							ctrl->SetAreaP(pos.x, pos.y, drawX, pos.y + (OSInt)sz.y, false);
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
