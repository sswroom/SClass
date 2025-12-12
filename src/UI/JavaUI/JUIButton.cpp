#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Text/CSSBuilder.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "UI/GUIClientControl.h"
#include "UI/JavaUI/JUIButton.h"

UI::JavaUI::JUIButton::JUIButton(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN label) : UI::GUIButton(ui, parent)
{
}

UI::JavaUI::JUIButton::~JUIButton()
{
}

void UI::JavaUI::JUIButton::SetText(Text::CStringNN text)
{
}

void UI::JavaUI::JUIButton::SetFont(UnsafeArrayOpt<const UTF8Char> name, UOSInt nameLen, Double fontHeightPt, Bool isBold)
{
}

OSInt UI::JavaUI::JUIButton::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}
