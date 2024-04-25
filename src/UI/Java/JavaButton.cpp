#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Text/CSSBuilder.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "UI/GUIClientControl.h"
#include "UI/Java/JavaButton.h"

UI::Java::JavaButton::JavaButton(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN label) : UI::GUIButton(ui, parent)
{
}

UI::Java::JavaButton::~JavaButton()
{
}

void UI::Java::JavaButton::SetText(Text::CStringNN text)
{
}

void UI::Java::JavaButton::SetFont(const UTF8Char *name, UOSInt nameLen, Double fontHeightPt, Bool isBold)
{
}

OSInt UI::Java::JavaButton::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}
