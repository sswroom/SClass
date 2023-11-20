#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"

UI::GUITabPage::GUITabPage(NotNullPtr<UI::GUICore> ui, Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUITabControl> ctrl, UOSInt index) : UI::GUIPanel(ui, ctrl->GetHandle())
{
	this->ctrl = ctrl;
	this->index = index;
	this->custObj = 0;
}

UI::GUITabPage::~GUITabPage()
{
}

void *UI::GUITabPage::GetFont()
{
	if (hFont)
	{
		return hFont;
	}
	else
	{
		return this->ctrl->GetFont();
	}
}

Bool UI::GUITabPage::IsChildVisible()
{
	return true;
}

Bool UI::GUITabPage::IsFormFocused()
{
	return this->ctrl->IsFormFocused();
}

Text::CStringNN UI::GUITabPage::GetObjectClass() const
{
	return CSTR("TabPage");
}

OSInt UI::GUITabPage::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

Optional<UI::GUIClientControl> UI::GUITabPage::GetParent()
{
	return this->ctrl->GetParent();
}

void UI::GUITabPage::SetCustObj(void *custObj)
{
	this->custObj = custObj;
}

void *UI::GUITabPage::GetCustObj()
{
	return this->custObj;
}

