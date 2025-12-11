#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIClientControl.h"
#include "UI/JavaUI/JavaComboBox.h"

UI::JavaUI::JavaComboBox::JavaComboBox(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Bool allowEdit) : UI::GUIComboBox(ui, parent)
{
	this->allowEdit = allowEdit;
	parent->AddChild(*this);
	this->Show();
}

UI::JavaUI::JavaComboBox::~JavaComboBox()
{
}

void UI::JavaUI::JavaComboBox::SetText(Text::CStringNN text)
{
}

UnsafeArrayOpt<UTF8Char> UI::JavaUI::JavaComboBox::GetText(UnsafeArray<UTF8Char> buff)
{
	return 0;
}

Bool UI::JavaUI::JavaComboBox::GetText(NN<Text::StringBuilderUTF8> sb)
{
	return false;
}

void UI::JavaUI::JavaComboBox::BeginUpdate()
{
/*	printf("wrap width = %d\r\n", gtk_combo_box_get_wrap_width(GTK_COMBO_BOX((GtkWidget*)this->hwnd)));
	gtk_combo_box_set_wrap_width(GTK_COMBO_BOX((GtkWidget*)this->hwnd), 0);

	this->clsData->model = gtk_combo_box_get_model(GTK_COMBO_BOX((GtkWidget*)this->hwnd));
	g_object_ref(this->clsData->model);
	gtk_combo_box_set_model(GTK_COMBO_BOX((GtkWidget*)this->hwnd), 0);*/
//	gtk_container_foreach(GTK_CONTAINER((GtkWidget*)this->hwnd), GUIComboBox_OnChild, this);
}

void UI::JavaUI::JavaComboBox::EndUpdate()
{
/*	gtk_combo_box_set_model(GTK_COMBO_BOX((GtkWidget*)this->hwnd), this->clsData->model);
	g_object_unref(this->clsData->model);
	gtk_combo_box_set_wrap_width(GTK_COMBO_BOX((GtkWidget*)this->hwnd), 0);
	this->clsData->model = 0;*/
}

UOSInt UI::JavaUI::JavaComboBox::AddItem(NN<Text::String> itemText, void *itemObj)
{
	return 0;
}

UOSInt UI::JavaUI::JavaComboBox::AddItem(Text::CStringNN itemText, void *itemObj)
{
	return 0;
}

UOSInt UI::JavaUI::JavaComboBox::InsertItem(UOSInt index, NN<Text::String> itemText, void *itemObj)
{
	return 0;
}

UOSInt UI::JavaUI::JavaComboBox::InsertItem(UOSInt index, Text::CStringNN itemText, void *itemObj)
{
	return 0;
}

AnyType UI::JavaUI::JavaComboBox::RemoveItem(UOSInt index)
{
	return 0;
}

void UI::JavaUI::JavaComboBox::ClearItems()
{
}

UOSInt UI::JavaUI::JavaComboBox::GetCount()
{
	return this->itemTexts.GetCount();
}

void UI::JavaUI::JavaComboBox::SetSelectedIndex(UOSInt index)
{
}

UOSInt UI::JavaUI::JavaComboBox::GetSelectedIndex()
{
	return 0;
}

AnyType UI::JavaUI::JavaComboBox::GetSelectedItem()
{
	return 0;
}

AnyType UI::JavaUI::JavaComboBox::GetItem(UOSInt index)
{
}

Math::Size2DDbl UI::JavaUI::JavaComboBox::GetSize()
{
	return Math::Size2DDbl(0, 0);
}

void UI::JavaUI::JavaComboBox::SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn)
{
	this->SetAreaP(Double2Int32(left * this->hdpi / this->ddpi),
		Double2Int32(top * this->hdpi / this->ddpi),
		Double2Int32(right * this->hdpi / this->ddpi),
		Double2Int32(bottom * this->hdpi / this->ddpi), updateScn);
}

OSInt UI::JavaUI::JavaComboBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::JavaUI::JavaComboBox::UpdatePos(Bool redraw)
{
	/////////////////////////	
}

void UI::JavaUI::JavaComboBox::SetTextSelection(UOSInt startPos, UOSInt endPos)
{
}
