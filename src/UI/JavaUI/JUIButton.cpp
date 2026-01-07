#include "Stdafx.h"
#include "MyMemory.h"
#include "Java/JavaJButton.h"
#include "Java/JavaMyActionListener.h"
#include "Java/JavaMyFocusListener.h"
#include "Java/JavaMyMouseListener.h"
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

Bool __stdcall JUIButton_ButtonDown(AnyType userObj, Math::Coord2D<OSInt> scnCoord, UI::GUIControl::MouseButton btn)
{
	userObj.GetNN<UI::JavaUI::JUIButton>()->EventButtonDown();
	return false;
}

Bool __stdcall JUIButton_ButtonUp(AnyType userObj, Math::Coord2D<OSInt> scnCoord, UI::GUIControl::MouseButton btn)
{
	userObj.GetNN<UI::JavaUI::JUIButton>()->EventButtonUp();
	return false;
}

UI::JavaUI::JUIButton::JUIButton(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN label) : UI::GUIButton(ui, parent)
{
	NN<Java::JavaJButton> btn;
	NEW_CLASSNN(btn, Java::JavaJButton(label));
	{
		Java::JavaMyActionListener l(JUIButton_ButtonClick, this);
		btn->AddActionListener(l);
	}
	{
		Java::JavaMyFocusListener l(this);
		l.HandleFocusGain(JUIButton_Focus);
		l.HandleFocusLost(JUIButton_FocusLost);
		btn->AddFocusListener(l);
	}
	{
		Java::JavaMyMouseListener l(this);
		l.HandleMousePressed(JUIButton_ButtonDown);
		l.HandleMouseReleased(JUIButton_ButtonUp);
		btn->AddMouseListener(l);
	}
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

void UI::JavaUI::JUIButton::SetFont(Text::CString fontName, Double fontHeightPt, Bool isBold)
{
	NN<Java::JavaJButton> btn;
	if (Optional<Java::JavaJButton>::ConvertFrom(this->hwnd).SetTo(btn))
	{
		Java::JavaFont font(fontName, isBold?Java::JavaFont::Style::BOLD:Java::JavaFont::Style::PLAIN, Double2Int32(fontHeightPt));
		btn->SetFont(font);
	}
}

OSInt UI::JavaUI::JUIButton::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}
