#include "Stdafx.h"
#include "MyMemory.h"
#include "Java/JavaJButton.h"
#include "Math/Math_C.h"
#include "Text/CSSBuilder.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "UI/GUIClientControl.h"
#include "UI/JavaUI/JUIButton.h"

void __stdcall JUIButton_Focus(AnyType userObj)
{
	userObj.GetNN<UI::JavaUI::JUIButton>()->EventFocus();
}

void __stdcall JUIButton_FocusLost(AnyType userObj)
{
	userObj.GetNN<UI::JavaUI::JUIButton>()->EventFocusLost();
}

void __stdcall JUIButton_ButtonClick(AnyType userObj)
{
	userObj.GetNN<UI::JavaUI::JUIButton>()->EventButtonClick();
}

void __stdcall JUIButton_ButtonDown(AnyType userObj, Math::Coord2D<OSInt> scnCoord, UI::GUIControl::MouseButton btn)
{
	userObj.GetNN<UI::JavaUI::JUIButton>()->EventButtonDown();
}

void __stdcall JUIButton_ButtonUp(AnyType userObj, Math::Coord2D<OSInt> scnCoord, UI::GUIControl::MouseButton btn)
{
	userObj.GetNN<UI::JavaUI::JUIButton>()->EventButtonUp();
}

UI::JavaUI::JUIButton::JUIButton(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN label) : UI::GUIButton(ui, parent)
{
	NN<Java::JavaJButton> btn;
	NEW_CLASSNN(btn, Java::JavaJButton(label));
	//btn->AddActionListener()
	//btn->AddMouseListener()
	//btn->AddFocusListener()
	this->hwnd = NN<ControlHandle>::ConvertFrom(btn);
	parent->AddChild(*this);
}

UI::JavaUI::JUIButton::~JUIButton()
{
	Optional<Java::JavaJButton>::ConvertFrom(this->hwnd).Delete();
}

void UI::JavaUI::JUIButton::SetText(Text::CStringNN text)
{
	NN<Java::JavaJButton> btn;
	if (Optional<Java::JavaJButton>::ConvertFrom(this->hwnd).SetTo(btn))
	{
		btn->SetText(text);
	}
}

void UI::JavaUI::JUIButton::SetFont(UnsafeArrayOpt<const UTF8Char> name, UOSInt nameLen, Double fontHeightPt, Bool isBold)
{
	NN<Java::JavaJButton> btn;
	if (Optional<Java::JavaJButton>::ConvertFrom(this->hwnd).SetTo(btn))
	{
		Java::JavaFont font(Text::CString(name, nameLen), isBold?Java::JavaFont::Style::BOLD:Java::JavaFont::Style::PLAIN, Double2Int32(fontHeightPt));
		btn->SetFont(font);
	}
}

OSInt UI::JavaUI::JUIButton::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}
