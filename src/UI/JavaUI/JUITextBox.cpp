#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GUIButton.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIForm.h"
#include "UI/JavaUI/JUITextBox.h"

UI::JavaUI::JUITextBox::JUITextBox(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText, Bool isMultiline) : UI::GUITextBox(ui, parent)
{
}

UI::JavaUI::JUITextBox::~JUITextBox()
{
//	gtk_widget_destroy((GtkWidget*)this->hwnd);
}

void UI::JavaUI::JUITextBox::SetReadOnly(Bool isReadOnly)
{
}

void UI::JavaUI::JUITextBox::SetPasswordChar(UTF32Char c)
{
}

void UI::JavaUI::JUITextBox::SetText(Text::CStringNN lbl)
{
}

UnsafeArrayOpt<UTF8Char> UI::JavaUI::JUITextBox::GetText(UnsafeArray<UTF8Char> buff)
{
	return 0;
}

Bool UI::JavaUI::JUITextBox::GetText(NN<Text::StringBuilderUTF8> sb)
{
	return false;
}

OSInt UI::JavaUI::JUITextBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::JavaUI::JUITextBox::SetWordWrap(Bool wordWrap)
{
}

void UI::JavaUI::JUITextBox::SelectAll()
{
}
