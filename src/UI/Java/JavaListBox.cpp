#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/Java/JavaListBox.h"

UI::Java::JavaListBox::JavaListBox(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Bool multiSelect) : UI::GUIListBox(ui, parent)
{
}

UI::Java::JavaListBox::~JavaListBox()
{
}

UOSInt UI::Java::JavaListBox::AddItem(NN<Text::String> itemText, void *itemObj)
{
	return 0;
}

UOSInt UI::Java::JavaListBox::AddItem(Text::CStringNN itemText, void *itemObj)
{
	return 0;
}

UOSInt UI::Java::JavaListBox::InsertItem(UOSInt index, Text::String *itemText, void *itemObj)
{
	return 0;
}

UOSInt UI::Java::JavaListBox::InsertItem(UOSInt index, Text::CStringNN itemText, void *itemObj)
{
	return 0;
}

void *UI::Java::JavaListBox::RemoveItem(UOSInt index)
{
	return 0;
}

void *UI::Java::JavaListBox::GetItem(UOSInt index)
{
	return 0;
}

void UI::Java::JavaListBox::ClearItems()
{
}

UOSInt UI::Java::JavaListBox::GetCount()
{
	return 0;
}

void UI::Java::JavaListBox::SetSelectedIndex(UOSInt index)
{
}

UOSInt UI::Java::JavaListBox::GetSelectedIndex()
{
	return INVALID_INDEX;
}

Bool UI::Java::JavaListBox::GetSelectedIndices(Data::ArrayList<UInt32> *indices)
{
	return false;
}

void *UI::Java::JavaListBox::GetSelectedItem()
{
	return 0;
}

UTF8Char *UI::Java::JavaListBox::GetSelectedItemText(UTF8Char *buff)
{
	return 0;
}

Optional<Text::String> UI::Java::JavaListBox::GetSelectedItemTextNew()
{
	return 0;
}

UTF8Char *UI::Java::JavaListBox::GetItemText(UTF8Char *buff, UOSInt index)
{
	return 0;
}

void UI::Java::JavaListBox::SetItemText(UOSInt index, Text::CStringNN text)
{
}

Optional<Text::String> UI::Java::JavaListBox::GetItemTextNew(UOSInt index)
{
	return 0;
}

OSInt UI::Java::JavaListBox::GetItemHeight()
{
	return 24;
}

OSInt UI::Java::JavaListBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}
