#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GUIButton.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIForm.h"
#include "UI/JavaUI/JavaTextBox.h"

UI::JavaUI::JavaTextBox::JavaTextBox(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText, Bool isMultiline) : UI::GUITextBox(ui, parent)
{
}

UI::JavaUI::JavaTextBox::~JavaTextBox()
{
//	gtk_widget_destroy((GtkWidget*)this->hwnd);
}

void UI::JavaUI::JavaTextBox::SetReadOnly(Bool isReadOnly)
{
}

void UI::JavaUI::JavaTextBox::SetPasswordChar(UTF32Char c)
{
}

void UI::JavaUI::JavaTextBox::SetText(Text::CStringNN lbl)
{
}

UnsafeArrayOpt<UTF8Char> UI::JavaUI::JavaTextBox::GetText(UnsafeArray<UTF8Char> buff)
{
	return 0;
}

Bool UI::JavaUI::JavaTextBox::GetText(NN<Text::StringBuilderUTF8> sb)
{
	return false;
}

OSInt UI::JavaUI::JavaTextBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::JavaUI::JavaTextBox::SetWordWrap(Bool wordWrap)
{
}

void UI::JavaUI::JavaTextBox::SelectAll()
{
}
