#include "Stdafx.h"
#include "DB/JavaDBUtil.h"
#include "SSWR/AVIRead/AVIRDataModelForm.h"
#include "Text/CharUtil.h"
#include "UI/Clipboard.h"

void __stdcall SSWR::AVIRead::AVIRDataModelForm::OnPasteDataClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRDataModelForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDataModelForm>();
	me->PasteData(true);
}

void __stdcall SSWR::AVIRead::AVIRDataModelForm::OnGenerateClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRDataModelForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDataModelForm>();
	NotNullPtr<Data::Class> cls;
	UOSInt i = me->cboType->GetSelectedIndex();
	if (me->cls.SetTo(cls))
	{
		Text::StringBuilderUTF8 sbClassName;
		Text::StringBuilderUTF8 sbPrefix;
		me->txtClassName->GetText(sbClassName);
		if (sbClassName.leng > 0)
		{
			me->txtPrefix->GetText(sbPrefix);
			Text::StringBuilderUTF8 sb;
			if (i == 0)
			{
				sbClassName.v[0] = Text::CharUtil::ToUpper(sbClassName.v[0]);
				cls->ToCppClassHeader(&sbClassName, 0, sb);
				if (UI::Clipboard::SetString(me->GetHandle(), sb.ToCString()))
				{
					me->lblStatus->SetText(CSTR("Generated to clipboard"));
				}
				else
				{
					me->lblStatus->SetText(CSTR("Failed in copying to clipboard"));
				}
			}
			else if (i == 1)
			{
				sbClassName.v[0] = Text::CharUtil::ToUpper(sbClassName.v[0]);
				cls->ToCppClassSource(&sbPrefix, &sbClassName, 0, sb);
				if (UI::Clipboard::SetString(me->GetHandle(), sb.ToCString()))
				{
					me->lblStatus->SetText(CSTR("Generated to clipboard"));
				}
				else
				{
					me->lblStatus->SetText(CSTR("Failed in copying to clipboard"));
				}
			}
			else if (i == 2)
			{
				sbClassName.v[0] = Text::CharUtil::ToUpper(sbClassName.v[0]);
				cls->ToJavaClass(&sbClassName, 0, sb);
				if (UI::Clipboard::SetString(me->GetHandle(), sb.ToCString()))
				{
					me->lblStatus->SetText(CSTR("Generated to clipboard"));
				}
				else
				{
					me->lblStatus->SetText(CSTR("Failed in copying to clipboard"));
				}
			}
		}
		else
		{
			me->lblStatus->SetText(CSTR("Class Name is empty"));
			me->txtClassName->Focus();
		}
	}
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

void SSWR::AVIRead::AVIRDataModelForm::PasteData(Bool showError)
{
	Text::StringBuilderUTF8 sb;
	if (UI::Clipboard::GetString(this->GetHandle(), sb))
	{
		NotNullPtr<Data::Class> cls;
		if (Data::Class::ParseFromStr(sb.ToCString()).SetTo(cls))
		{
			this->cls.Delete();
			this->cls = cls;
			this->UpdateClassDisp();
		}
		else
		{
			if (showError) this->ui->ShowMsgOK(CSTR("Error in parsing Clipboard string"), CSTR("Data Model"), this);
		}
	}
	else
	{
		if (showError) this->ui->ShowMsgOK(CSTR("No Data from Clipboard"), CSTR("Data Model"), this);
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
	this->pnlControl->SetRect(0, 0, 100, 55, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lvColumns = ui->NewListView(*this, UI::ListViewStyle::Table, 3);
	this->lvColumns->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvColumns->AddColumn(CSTR("Data Type"), 200);
	this->lvColumns->AddColumn(CSTR("SubType"), 70);
	this->lvColumns->AddColumn(CSTR("Name"), 300);
	this->lvColumns->SetFullRowSelect(true);
	this->lvColumns->SetShowGrid(true);
	this->lvColumns->HandleDblClk(OnColumnsDblClk, this);
	this->lblClassName = ui->NewLabel(this->pnlControl, CSTR("Class Name"));
	this->lblClassName->SetRect(4, 4, 100, 23, false);
	this->txtClassName = ui->NewTextBox(this->pnlControl, CSTR(""));
	this->txtClassName->SetRect(104, 4, 200, 23, false);
	this->lblPrefix = ui->NewLabel(this->pnlControl, CSTR("Prefix"));
	this->lblPrefix->SetRect(304, 4, 100, 23, false);
	this->txtPrefix = ui->NewTextBox(this->pnlControl, CSTR(""));
	this->txtPrefix->SetRect(404, 4, 200, 23, false);
	this->btnPasteData = ui->NewButton(this->pnlControl, CSTR("Paste Data"));
	this->btnPasteData->SetRect(4, 28, 75, 23, false);
	this->btnPasteData->HandleButtonClick(OnPasteDataClicked, this);
	this->cboType = ui->NewComboBox(this->pnlControl, false);
	this->cboType->SetRect(84, 28, 150, 23, false);
	this->cboType->AddItem(CSTR("C++ Header File"), 0);
	this->cboType->AddItem(CSTR("C++ Source File"), 0);
	this->cboType->AddItem(CSTR("Java Entity File"), 0);
	this->btnGenerate = ui->NewButton(this->pnlControl, CSTR("Generate"));
	this->btnGenerate->SetRect(234, 28, 75, 23, false);
	this->btnGenerate->HandleButtonClick(OnGenerateClicked, this);
	this->lblStatus = ui->NewLabel(this->pnlControl, CSTR(""));
	this->lblStatus->SetRect(314, 28, 150, 23, false);
	this->PasteData(false);
}

SSWR::AVIRead::AVIRDataModelForm::~AVIRDataModelForm()
{
	this->cls.Delete();
}

void SSWR::AVIRead::AVIRDataModelForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
