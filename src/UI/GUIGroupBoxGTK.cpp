#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GUIGroupBox.h"
#include <gtk/gtk.h>

UI::GUIGroupBox::GUIGroupBox(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Text::CStringNN text) : UI::GUIClientControl(ui, parent)
{
	this->hwnd = (ControlHandle*)gtk_frame_new((const Char*)text.v);
	parent->AddChild(this);
	this->Show();
}

Math::Coord2DDbl UI::GUIGroupBox::GetClientOfst()
{
	return Math::Coord2DDbl(0, 0);
}

Math::Size2DDbl UI::GUIGroupBox::GetClientSize()
{
	return this->GetSize() - Math::Double2D(6, 18);
}

Bool UI::GUIGroupBox::IsChildVisible()
{
	return true;
}

UI::GUIGroupBox::~GUIGroupBox()
{
}

Text::CStringNN UI::GUIGroupBox::GetObjectClass() const
{
	return CSTR("GroupBox");
}

OSInt UI::GUIGroupBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}


void UI::GUIGroupBox::OnSizeChanged(Bool updateScn)
{
	this->UpdateChildrenSize(updateScn);
	UOSInt i = this->resizeHandlers.GetCount();
	while (i-- > 0)
	{
		this->resizeHandlers.GetItem(i)(this->resizeHandlersObjs.GetItem(i));
	}
}

