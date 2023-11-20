#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include <gtk/gtk.h>


void UI::GUIPanel::Init(void *hInst)
{
}

void UI::GUIPanel::Deinit(void *hInst)
{
}

void UI::GUIPanel::UpdateScrollBars()
{
}

UI::GUIPanel::GUIPanel(NotNullPtr<UI::GUICore> ui, ControlHandle *parentHWnd) : UI::GUIClientControl(ui, 0)
{
	this->minW = 0;
	this->minH = 0;
	this->scrollH = false;
	this->scrollV = false;
	this->currScrX = 0;
	this->currScrY = 0;
	this->hwnd = (ControlHandle*)gtk_fixed_new();
}

UI::GUIPanel::GUIPanel(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent) : UI::GUIClientControl(ui, parent)
{
	this->minW = 0;
	this->minH = 0;
	this->scrollH = false;
	this->scrollV = false;
	this->currScrX = 0;
	this->currScrY = 0;

	this->hwnd = (ControlHandle*)gtk_fixed_new();
	parent->AddChild(this);
	this->Show();
}

UI::GUIPanel::~GUIPanel()
{
}

Bool UI::GUIPanel::IsChildVisible()
{
	return true;
}

Math::Coord2DDbl UI::GUIPanel::GetClientOfst()
{
	return Math::Coord2DDbl(-this->currScrX, -this->currScrY);
}

Math::Size2DDbl UI::GUIPanel::GetClientSize()
{
	return this->GetSize();
}

Text::CStringNN UI::GUIPanel::GetObjectClass() const
{
	return CSTR("Panel");
}

OSInt UI::GUIPanel::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUIPanel::SetMinSize(Int32 minW, Int32 minH)
{
	this->minW = minW;
	this->minH = minH;
	this->UpdateScrollBars();
}
