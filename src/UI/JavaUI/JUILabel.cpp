#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/CSSBuilder.h"
#include "Text/MyString.h"
#include "UI/GUIClientControl.h"
#include "UI/JavaUI/JUILabel.h"

UI::JavaUI::JUILabel::JUILabel(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN label) : UI::GUILabel(ui, parent)
{
	NN<Java::JavaJLabel> lbl;
	NEW_CLASSNN(lbl, Java::JavaJLabel(label));
	this->hwnd = NN<ControlHandle>::ConvertFrom(lbl);
	parent->AddChild(*this);
}

UI::JavaUI::JUILabel::~JUILabel()
{
	Optional<Java::JavaJLabel>::ConvertFrom(this->hwnd).Delete();
}

void UI::JavaUI::JUILabel::SetText(Text::CStringNN text)
{
	NN<Java::JavaJLabel> lbl;
	if (Optional<Java::JavaJLabel>::ConvertFrom(this->hwnd).SetTo(lbl))
	{
		lbl->SetText(text);
	}
}

OSInt UI::JavaUI::JUILabel::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

Bool UI::JavaUI::JUILabel::HasTextColor()
{
	return this->hasTextColor;
}

UInt32 UI::JavaUI::JUILabel::GetTextColor()
{
	return this->textColor;
}

void UI::JavaUI::JUILabel::SetTextColor(UInt32 textColor)
{
	this->textColor = textColor;
	this->hasTextColor = true;
	NN<Java::JavaJLabel> lbl;
	if (Optional<Java::JavaJLabel>::ConvertFrom(this->hwnd).SetTo(lbl))
	{
		Java::JavaColor color((Int32)textColor, (textColor & 0xff000000) != 0xff000000);
		lbl->SetForeground(color);
	}
}

