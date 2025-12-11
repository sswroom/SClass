#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/JavaUI/JavaListBox.h"

UI::JavaUI::JavaListBox::JavaListBox(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Bool multiSelect) : UI::GUIListBox(ui, parent)
{
}

UI::JavaUI::JavaListBox::~JavaListBox()
{
}

UOSInt UI::JavaUI::JavaListBox::AddItem(NN<Text::String> itemText, AnyType itemObj)
{
	return 0;
}

UOSInt UI::JavaUI::JavaListBox::AddItem(Text::CStringNN itemText, AnyType itemObj)
{
	return 0;
}

UOSInt UI::JavaUI::JavaListBox::InsertItem(UOSInt index, NN<Text::String> itemText, AnyType itemObj)
{
	return 0;
}

UOSInt UI::JavaUI::JavaListBox::InsertItem(UOSInt index, Text::CStringNN itemText, AnyType itemObj)
{
	return 0;
}

AnyType UI::JavaUI::JavaListBox::RemoveItem(UOSInt index)
{
	return 0;
}

AnyType UI::JavaUI::JavaListBox::GetItem(UOSInt index)
{
	return 0;
}

void UI::JavaUI::JavaListBox::ClearItems()
{
}

UOSInt UI::JavaUI::JavaListBox::GetCount()
{
	return 0;
}

void UI::JavaUI::JavaListBox::SetSelectedIndex(UOSInt index)
{
}

UOSInt UI::JavaUI::JavaListBox::GetSelectedIndex()
{
	return INVALID_INDEX;
}

Bool UI::JavaUI::JavaListBox::GetSelectedIndices(NN<Data::ArrayList<UInt32>> indices)
{
	return false;
}

AnyType UI::JavaUI::JavaListBox::GetSelectedItem()
{
	return 0;
}

UnsafeArrayOpt<UTF8Char> UI::JavaUI::JavaListBox::GetSelectedItemText(UnsafeArray<UTF8Char> buff)
{
	return 0;
}

Optional<Text::String> UI::JavaUI::JavaListBox::GetSelectedItemTextNew()
{
	return 0;
}

UnsafeArrayOpt<UTF8Char> UI::JavaUI::JavaListBox::GetItemText(UnsafeArray<UTF8Char> buff, UOSInt index)
{
	return 0;
}

void UI::JavaUI::JavaListBox::SetItemText(UOSInt index, Text::CStringNN text)
{
}

Optional<Text::String> UI::JavaUI::JavaListBox::GetItemTextNew(UOSInt index)
{
	return 0;
}

OSInt UI::JavaUI::JavaListBox::GetItemHeight()
{
	return 24;
}

OSInt UI::JavaUI::JavaListBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}
