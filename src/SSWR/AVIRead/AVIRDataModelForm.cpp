#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRDataModelForm.h"
#include "UI/Clipboard.h"

void __stdcall SSWR::AVIRead::AVIRDataModelForm::OnPasteDataClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRDataModelForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDataModelForm>();
	Text::StringBuilderUTF8 sb;
	if (UI::Clipboard::GetString(me->GetHandle(), sb))
	{
		NotNullPtr<Data::Class> cls;
		if (Data::Class::ParseFromStr(sb.ToCString()).SetTo(cls))
		{
			me->cls.Delete();
			me->cls = cls;
			me->UpdateClassDisp();
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Error in parsing Clipboard string"), CSTR("Data Model"), me);
		}
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("No Data from Clipboard"), CSTR("Data Model"), me);
	}
}

void __stdcall SSWR::AVIRead::AVIRDataModelForm::OnCancelClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRDataModelForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDataModelForm>();

}

void __stdcall SSWR::AVIRead::AVIRDataModelForm::OnColumnsDblClk(AnyType userObj, UOSInt index)
{
	NotNullPtr<SSWR::AVIRead::AVIRDataModelForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDataModelForm>();
	UOSInt fieldIndex = (UOSInt)me->lvColumns->GetItem(index);
	NotNullPtr<Data::Class> cls;
	NotNullPtr<Data::Class::FieldInfo> field;
	if (me->cls.SetTo(cls) && cls->GetFieldInfo(fieldIndex).SetTo(field))
	{
		if (field->typeName.NotNull())
		{
			if (field->itemType == Data::VariItem::ItemType::Str)
			{
				field->itemType = Data::VariItem::ItemType::I64;
				me->lvColumns->SetSubItem(index, 1, CSTR("Enum(Int)"));
			}
			else
			{
				field->itemType = Data::VariItem::ItemType::Str;
				me->lvColumns->SetSubItem(index, 1, CSTR("Enum(Str)"));
			}
		}
		else
		{
			if (field->notNull)
			{
				field->notNull = false;
				me->lvColumns->SetSubItem(index, 1, CSTR("Null"));
			}
			else
			{
				field->notNull = true;
				me->lvColumns->SetSubItem(index, 1, CSTR("NotNull"));
			}
		}
	}
}

void SSWR::AVIRead::AVIRDataModelForm::UpdateClassDisp()
{
	this->lvColumns->ClearItems();
	NotNullPtr<Data::Class> cls;
	if (this->cls.SetTo(cls))
	{
		UOSInt i = 0;
		UOSInt j = cls->GetFieldCount();
		UOSInt k;
		NotNullPtr<Data::Class::FieldInfo> field;
		NotNullPtr<Text::String> typeName;
		while (i < j)
		{
			if (cls->GetFieldInfo(i).SetTo(field))
			{
				if (field->typeName.SetTo(typeName))
				{
					k = this->lvColumns->AddItem(typeName, (void*)i);
					this->lvColumns->SetSubItem(k, 1, (field->itemType == Data::VariItem::ItemType::Str)?CSTR("Enum(Str)"):CSTR("Enum(Int)"));
				}
				else
				{
					k = this->lvColumns->AddItem(Data::VariItem::ItemTypeGetName(field->itemType), (void*)i);
					this->lvColumns->SetSubItem(k, 1, field->notNull?CSTR("NotNull"):CSTR("Null"));
				}
				this->lvColumns->SetSubItem(k, 2, field->name);
			}
			i++;
		}
	}
}

SSWR::AVIRead::AVIRDataModelForm::AVIRDataModelForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(CSTR("Data Model"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->cls = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lvColumns = ui->NewListView(*this, UI::ListViewStyle::Table, 3);
	this->lvColumns->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvColumns->AddColumn(CSTR("Data Type"), 200);
	this->lvColumns->AddColumn(CSTR("SubType"), 50);
	this->lvColumns->AddColumn(CSTR("Name"), 300);
	this->lvColumns->SetFullRowSelect(true);
	this->lvColumns->SetShowGrid(true);
	this->lvColumns->HandleDblClk(OnColumnsDblClk, this);
	this->btnPasteData = ui->NewButton(this->pnlControl, CSTR("Paste Data"));
	this->btnPasteData->SetRect(4, 4, 75, 23, false);
	this->btnPasteData->HandleButtonClick(OnPasteDataClicked, this);
	this->btnCancel = ui->NewButton(this->pnlControl, CSTR("Cancel"));
	this->btnCancel->SetRect(84, 4, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
}

SSWR::AVIRead::AVIRDataModelForm::~AVIRDataModelForm()
{
	this->cls.Delete();
}

void SSWR::AVIRead::AVIRDataModelForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
