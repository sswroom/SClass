#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GUIButton.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIForm.h"
#include "UI/Java/JavaTextBox.h"

UI::Java::JavaTextBox::JavaTextBox(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText, Bool isMultiline) : UI::GUITextBox(ui, parent)
{
}

UI::Java::JavaTextBox::~JavaTextBox()
{
//	gtk_widget_destroy((GtkWidget*)this->hwnd);
}

void UI::Java::JavaTextBox::SetReadOnly(Bool isReadOnly)
{
}

void UI::Java::JavaTextBox::SetPasswordChar(WChar c)
{
}

void UI::Java::JavaTextBox::SetText(Text::CStringNN lbl)
{
}

UnsafeArrayOpt<UTF8Char> UI::Java::JavaTextBox::GetText(UnsafeArray<UTF8Char> buff)
{
	return 0;
}

Bool UI::Java::JavaTextBox::GetText(NN<Text::StringBuilderUTF8> sb)
{
	return false;
}

OSInt UI::Java::JavaTextBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::Java::JavaTextBox::SetWordWrap(Bool wordWrap)
{
}

void UI::Java::JavaTextBox::SelectAll()
{
}
