#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GUIGroupBox.h"

UI::GUIGroupBox::GUIGroupBox(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent) : UI::GUIClientControl(ui, parent)
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
		Data::CallbackStorage<UI::UIEvent> cb = this->resizeHandlers.GetItem(i);
		cb.func(cb.userObj);
	}
}

