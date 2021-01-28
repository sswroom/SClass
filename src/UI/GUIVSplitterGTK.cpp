#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIVSplitter.h"
#include <gtk/gtk.h>

UI::GUIVSplitter::GUIVSplitter(UI::GUICore *ui, UI::GUIClientControl *parent, Int32 height, Bool isBottom) : UI::GUIControl(ui, parent)
{
	this->dragMode = false;
	this->isBottom = isBottom;
	this->hwnd = gtk_drawing_area_new();
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

OSInt UI::GUIVSplitter::OnNotify(Int32 code, void *lParam)
{
	return 0;
}
