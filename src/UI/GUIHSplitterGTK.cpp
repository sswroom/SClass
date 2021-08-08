#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIHSplitter.h"
#include <gtk/gtk.h>

UI::GUIHSplitter::GUIHSplitter(UI::GUICore *ui, UI::GUIClientControl *parent, Int32 width, Bool isRight) : UI::GUIControl(ui, parent)
{
	this->dragMode = false;
	this->isRight = isRight;
	this->hwnd = (ControlHandle*)gtk_drawing_area_new();
	parent->AddChild(this);
	this->Show();

	this->SetRect(0, 0, width, 100, false);
	this->SetDockType(isRight?DOCK_RIGHT:DOCK_LEFT);
}

UI::GUIHSplitter::~GUIHSplitter()
{
}

const UTF8Char *UI::GUIHSplitter::GetObjectClass()
{
	return (const UTF8Char*)"HSplitter";
}

OSInt UI::GUIHSplitter::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}
