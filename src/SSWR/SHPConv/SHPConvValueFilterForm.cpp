#include "Stdafx.h"
#include "SSWR/SHPConv/ValueFilter.h"
#include "SSWR/SHPConv/SHPConvValueFilterForm.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::SHPConv::SHPConvValueFilterForm::OnOKClicked(AnyType userObj)
{
	NN<SSWR::SHPConv::SHPConvValueFilterForm> me = userObj.GetNN<SSWR::SHPConv::SHPConvValueFilterForm>();
	Text::StringBuilderUTF8 sb;
	Int32 typ;
	me->txtValue->GetText(sb);
	if (me->radEqual->IsSelected())
	{
		typ = 1;
	}
	else
	{
		typ = 0;
	}
	NN<ValueFilter> filter;
	NEW_CLASSNN(filter, ValueFilter((UIntOS)me->cboCol->GetSelectedIndex(), sb.ToCString(), typ));
	me->filter = filter;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::SHPConv::SHPConvValueFilterForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::SHPConv::SHPConvValueFilterForm> me = userObj.GetNN<SSWR::SHPConv::SHPConvValueFilterForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::SHPConv::SHPConvValueFilterForm::SHPConvValueFilterForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<DB::DBFFile> dbf) : UI::GUIForm(parent, 256, 152, ui)
{
	this->SetText(CSTR("Value Filter"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);
	NEW_CLASSNN(this->monMgr, Media::MonitorMgr());
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
	this->dbf = dbf;
	this->filter = nullptr;
	
	this->lblCol = ui->NewLabel(*this, CSTR("Column"));
	this->lblCol->SetRect(8, 8, 56, 23, false);
	this->cboCol = ui->NewComboBox(*this, false);
	this->cboCol->SetRect(72, 8, 121, 21, false);
	this->lblType = ui->NewLabel(*this, CSTR("Type"));
	this->lblType->SetRect(8, 32, 56, 23, false);
	this->radEqual = ui->NewRadioButton(*this, CSTR("Equal"), true);
	this->radEqual->SetRect(72, 32, 64, 24, false);
	this->radNotEq = ui->NewRadioButton(*this, CSTR("Not Equal"), false);
	this->radNotEq->SetRect(136, 32, 96, 24, false);
	this->lblValue = ui->NewLabel(*this, CSTR("Value"));
	this->lblValue->SetRect(8, 56, 56, 23, false);
	this->txtValue = ui->NewTextBox(*this, CSTR(""));
	this->txtValue->SetRect(72, 56, 136, 20, false);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(24, 96, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(136, 96, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UIntOS i = 0;
	UIntOS j = this->dbf->GetColCount();
	this->cboCol->ClearItems();
	while (i < j)
	{
		if (this->dbf->GetColumnName(i, sbuff).SetTo(sptr))
		{
			this->cboCol->AddItem(CSTRP(sbuff, sptr), 0);
		}
		i++;
	}
	this->cboCol->SetSelectedIndex(0);
}

SSWR::SHPConv::SHPConvValueFilterForm::~SHPConvValueFilterForm()
{
	this->monMgr.Delete();
}

void SSWR::SHPConv::SHPConvValueFilterForm::OnMonitorChanged()
{
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
}

Optional<SSWR::SHPConv::MapFilter> SSWR::SHPConv::SHPConvValueFilterForm::GetFilter()
{
	return this->filter;
}
