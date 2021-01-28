#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GUIGroupBox.h"
#include <gtk/gtk.h>

UI::GUIGroupBox::GUIGroupBox(UI::GUICore *ui, UI::GUIClientControl *parent, const UTF8Char *text) : UI::GUIClientControl(ui, parent)
{
	this->hwnd = gtk_frame_new((const Char*)text);
	parent->AddChild(this);
	this->Show();
}

void UI::GUIGroupBox::GetClientOfst(Double *x, Double *y)
{
	if (x)
		*x = 0;
	if (y)
		*y = 0;
}

void UI::GUIGroupBox::GetClientSize(Double *w, Double *h)
{
	Double width;
	Double height;
	this->GetSize(&width, &height);
	if (w)
		*w = width - 6;
	if (h)
		*h = height - 18;
}

Bool UI::GUIGroupBox::IsChildVisible()
{
	return true;
}

UI::GUIGroupBox::~GUIGroupBox()
{
}

const UTF8Char *UI::GUIGroupBox::GetObjectClass()
{
	return (const UTF8Char*)"GroupBox";
}

OSInt UI::GUIGroupBox::OnNotify(Int32 code, void *lParam)
{
	return 0;
}


void UI::GUIGroupBox::OnSizeChanged(Bool updateScn)
{
	this->UpdateChildrenSize(updateScn);
	OSInt i = this->resizeHandlers->GetCount();
	while (i-- > 0)
	{
		this->resizeHandlers->GetItem(i)(this->resizeHandlersObjs->GetItem(i));
	}
}

