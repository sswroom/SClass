#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/JavaUI/JUIListBox.h"

UI::JavaUI::JUIListBox::JUIListBox(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Bool multiSelect) : UI::GUIListBox(ui, parent)
{
}

UI::JavaUI::JUIListBox::~JUIListBox()
{
}

UOSInt UI::JavaUI::JUIListBox::AddItem(NN<Text::String> itemText, AnyType itemObj)
{
	return 0;
}

UOSInt UI::JavaUI::JUIListBox::AddItem(Text::CStringNN itemText, AnyType itemObj)
{
	return 0;
}

UOSInt UI::JavaUI::JUIListBox::InsertItem(UOSInt index, NN<Text::String> itemText, AnyType itemObj)
{
	return 0;
}

UOSInt UI::JavaUI::JUIListBox::InsertItem(UOSInt index, Text::CStringNN itemText, AnyType itemObj)
{
	return 0;
}

AnyType UI::JavaUI::JUIListBox::RemoveItem(UOSInt index)
{
	return 0;
}

AnyType UI::JavaUI::JUIListBox::GetItem(UOSInt index)
{
	return 0;
}

void UI::JavaUI::JUIListBox::ClearItems()
{
}

UOSInt UI::JavaUI::JUIListBox::GetCount()
{
	return 0;
}

void UI::JavaUI::JUIListBox::SetSelectedIndex(UOSInt index)
{
}

UOSInt UI::JavaUI::JUIListBox::GetSelectedIndex()
{
	return INVALID_INDEX;
}

Bool UI::JavaUI::JUIListBox::GetSelectedIndices(NN<Data::ArrayList<UInt32>> indices)
{
	return false;
}

AnyType UI::JavaUI::JUIListBox::GetSelectedItem()
{
	return 0;
}

UnsafeArrayOpt<UTF8Char> UI::JavaUI::JUIListBox::GetSelectedItemText(UnsafeArray<UTF8Char> buff)
{
	return 0;
}

Optional<Text::String> UI::JavaUI::JUIListBox::GetSelectedItemTextNew()
{
	return 0;
}

UnsafeArrayOpt<UTF8Char> UI::JavaUI::JUIListBox::GetItemText(UnsafeArray<UTF8Char> buff, UOSInt index)
{
	return 0;
}

void UI::JavaUI::JUIListBox::SetItemText(UOSInt index, Text::CStringNN text)
{
}

Optional<Text::String> UI::JavaUI::JUIListBox::GetItemTextNew(UOSInt index)
{
	return 0;
}

OSInt UI::JavaUI::JUIListBox::GetItemHeight()
{
	return 24;
}

OSInt UI::JavaUI::JUIListBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}
