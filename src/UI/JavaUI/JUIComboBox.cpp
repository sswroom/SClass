#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIClientControl.h"
#include "UI/JavaUI/JUIComboBox.h"

UI::JavaUI::JUIComboBox::JUIComboBox(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Bool allowEdit) : UI::GUIComboBox(ui, parent)
{
	this->allowEdit = allowEdit;
	parent->AddChild(*this);
	this->Show();
}

UI::JavaUI::JUIComboBox::~JUIComboBox()
{
}

void UI::JavaUI::JUIComboBox::SetText(Text::CStringNN text)
{
}

UnsafeArrayOpt<UTF8Char> UI::JavaUI::JUIComboBox::GetText(UnsafeArray<UTF8Char> buff)
{
	return 0;
}

Bool UI::JavaUI::JUIComboBox::GetText(NN<Text::StringBuilderUTF8> sb)
{
	return false;
}

void UI::JavaUI::JUIComboBox::BeginUpdate()
{
/*	printf("wrap width = %d\r\n", gtk_combo_box_get_wrap_width(GTK_COMBO_BOX((GtkWidget*)this->hwnd)));
	gtk_combo_box_set_wrap_width(GTK_COMBO_BOX((GtkWidget*)this->hwnd), 0);

	this->clsData->model = gtk_combo_box_get_model(GTK_COMBO_BOX((GtkWidget*)this->hwnd));
	g_object_ref(this->clsData->model);
	gtk_combo_box_set_model(GTK_COMBO_BOX((GtkWidget*)this->hwnd), 0);*/
//	gtk_container_foreach(GTK_CONTAINER((GtkWidget*)this->hwnd), GUIComboBox_OnChild, this);
}

void UI::JavaUI::JUIComboBox::EndUpdate()
{
/*	gtk_combo_box_set_model(GTK_COMBO_BOX((GtkWidget*)this->hwnd), this->clsData->model);
	g_object_unref(this->clsData->model);
	gtk_combo_box_set_wrap_width(GTK_COMBO_BOX((GtkWidget*)this->hwnd), 0);
	this->clsData->model = 0;*/
}

UIntOS UI::JavaUI::JUIComboBox::AddItem(NN<Text::String> itemText, AnyType itemObj)
{
	return 0;
}

UIntOS UI::JavaUI::JUIComboBox::AddItem(Text::CStringNN itemText, AnyType itemObj)
{
	return 0;
}

UIntOS UI::JavaUI::JUIComboBox::InsertItem(UIntOS index, NN<Text::String> itemText, AnyType itemObj)
{
	return 0;
}

UIntOS UI::JavaUI::JUIComboBox::InsertItem(UIntOS index, Text::CStringNN itemText, AnyType itemObj)
{
	return 0;
}

AnyType UI::JavaUI::JUIComboBox::RemoveItem(UIntOS index)
{
	return 0;
}

void UI::JavaUI::JUIComboBox::ClearItems()
{
}

UIntOS UI::JavaUI::JUIComboBox::GetCount()
{
	return this->itemTexts.GetCount();
}

void UI::JavaUI::JUIComboBox::SetSelectedIndex(UIntOS index)
{
}

UIntOS UI::JavaUI::JUIComboBox::GetSelectedIndex()
{
	return 0;
}

AnyType UI::JavaUI::JUIComboBox::GetSelectedItem()
{
	return 0;
}

AnyType UI::JavaUI::JUIComboBox::GetItem(UIntOS index)
{
	return 0;
}

Math::Size2DDbl UI::JavaUI::JUIComboBox::GetSize()
{
	return Math::Size2DDbl(0, 0);
}

void UI::JavaUI::JUIComboBox::SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn)
{
	this->SetAreaP(Double2Int32(left * this->hdpi / this->ddpi),
		Double2Int32(top * this->hdpi / this->ddpi),
		Double2Int32(right * this->hdpi / this->ddpi),
		Double2Int32(bottom * this->hdpi / this->ddpi), updateScn);
}

IntOS UI::JavaUI::JUIComboBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::JavaUI::JUIComboBox::UpdatePos(Bool redraw)
{
	/////////////////////////	
}

void UI::JavaUI::JUIComboBox::SetTextSelection(UIntOS startPos, UIntOS endPos)
{
}
