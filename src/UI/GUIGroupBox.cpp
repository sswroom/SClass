#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GUIGroupBox.h"

UI::GUIGroupBox::GUIGroupBox(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent) : UI::GUIClientControl(ui, parent)
{
}

UI::GUIGroupBox::~GUIGroupBox()
{
}


Bool UI::GUIGroupBox::IsChildVisible()
{
	return true;
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

