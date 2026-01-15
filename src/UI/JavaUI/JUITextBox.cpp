#include "Stdafx.h"
#include "MyMemory.h"
#include "Java/JavaJPasswordField.h"
#include "Java/JavaJTextArea.h"
#include "Text/MyString.h"
#include "UI/GUIButton.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIForm.h"
#include "UI/JavaUI/JUITextBox.h"

UI::JavaUI::JUITextBox::JUITextBox(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText, Bool isMultiline) : UI::GUITextBox(ui, parent)
{
	NN<Java::JavaJTextComponent> txt;
	if (isMultiline)
	{
		NEW_CLASSNN(txt, Java::JavaJTextArea(initText));
	}
	else
	{
		NEW_CLASSNN(txt, Java::JavaJPasswordField(initText));
	}
	this->multiline = isMultiline;
	this->hwnd = NN<ControlHandle>::ConvertFrom(txt);
	parent->AddChild(*this);
}

UI::JavaUI::JUITextBox::~JUITextBox()
{
	Optional<Java::JavaJTextComponent>::ConvertFrom(this->hwnd).Delete();
}

void UI::JavaUI::JUITextBox::SetReadOnly(Bool isReadOnly)
{
	NN<Java::JavaJTextComponent> txt;
	if (Optional<Java::JavaJTextComponent>::ConvertFrom(this->hwnd).SetTo(txt))
	{
		txt->SetEditable(isReadOnly);
	}
}

void UI::JavaUI::JUITextBox::SetPasswordChar(UTF32Char c)
{
	NN<Java::JavaJPasswordField> txt;
	if (!this->multiline && Optional<Java::JavaJPasswordField>::ConvertFrom(this->hwnd).SetTo(txt))
	{
		txt->SetEchoChar((UTF16Char)c);
	}
}

void UI::JavaUI::JUITextBox::SetText(Text::CStringNN lbl)
{
	NN<Java::JavaJTextComponent> txt;
	if (Optional<Java::JavaJTextComponent>::ConvertFrom(this->hwnd).SetTo(txt))
	{
		txt->SetText(lbl);
	}
}

UnsafeArrayOpt<UTF8Char> UI::JavaUI::JUITextBox::GetText(UnsafeArray<UTF8Char> buff)
{
	NN<Java::JavaJTextComponent> txt;
	if (Optional<Java::JavaJTextComponent>::ConvertFrom(this->hwnd).SetTo(txt))
	{
		Java::JavaString s = txt->GetText();
		return s.Get(buff);
	}
	return 0;
}

Bool UI::JavaUI::JUITextBox::GetText(NN<Text::StringBuilderUTF8> sb)
{
	NN<Java::JavaJTextComponent> txt;
	if (Optional<Java::JavaJTextComponent>::ConvertFrom(this->hwnd).SetTo(txt))
	{
		Java::JavaString s = txt->GetText();
		s.Get(sb);
		return true;
	}
	return false;
}

IntOS UI::JavaUI::JUITextBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::JavaUI::JUITextBox::SetWordWrap(Bool wordWrap)
{
	NN<Java::JavaJTextArea> txt;
	if (this->multiline && Optional<Java::JavaJTextArea>::ConvertFrom(this->hwnd).SetTo(txt))
	{
		txt->SetLineWrap(wordWrap);
	}
}

void UI::JavaUI::JUITextBox::SelectAll()
{
	NN<Java::JavaJTextComponent> txt;
	if (Optional<Java::JavaJTextComponent>::ConvertFrom(this->hwnd).SetTo(txt))
	{
		txt->SelectAll();
	}
}
