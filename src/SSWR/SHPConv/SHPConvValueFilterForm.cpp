#include "Stdafx.h"
#include "SSWR/SHPConv/ValueFilter.h"
#include "SSWR/SHPConv/SHPConvValueFilterForm.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::SHPConv::SHPConvValueFilterForm::OnOKClicked(void *userObj)
{
	SSWR::SHPConv::SHPConvValueFilterForm *me = (SSWR::SHPConv::SHPConvValueFilterForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Int32 typ;
	me->txtValue->GetText(&sb);
	if (me->radEqual->IsSelected())
	{
		typ = 1;
	}
	else
	{
		typ = 0;
	}
	NEW_CLASS(me->filter, ValueFilter((UOSInt)me->cboCol->GetSelectedIndex(), sb.ToCString(), typ));
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::SHPConv::SHPConvValueFilterForm::OnCancelClicked(void *userObj)
{
	SSWR::SHPConv::SHPConvValueFilterForm *me = (SSWR::SHPConv::SHPConvValueFilterForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::SHPConv::SHPConvValueFilterForm::SHPConvValueFilterForm(UI::GUIClientControl *parent, UI::GUICore *ui, DB::DBFFile *dbf) : UI::GUIForm(parent, 256, 152, ui)
{
	this->SetText(CSTR("Value Filter"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	NEW_CLASS(this->monMgr, Media::MonitorMgr());
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
	this->dbf = dbf;
	this->filter = 0;
	
	NEW_CLASS(this->lblCol, UI::GUILabel(ui, this, CSTR("Column")));
	this->lblCol->SetRect(8, 8, 56, 23, false);
	NEW_CLASS(this->cboCol, UI::GUIComboBox(ui, this, false));
	this->cboCol->SetRect(72, 8, 121, 21, false);
	NEW_CLASS(this->lblType, UI::GUILabel(ui, this, CSTR("Type")));
	this->lblType->SetRect(8, 32, 56, 23, false);
	NEW_CLASS(this->radEqual, UI::GUIRadioButton(ui, this, CSTR("Equal"), true));
	this->radEqual->SetRect(72, 32, 64, 24, false);
	NEW_CLASS(this->radNotEq, UI::GUIRadioButton(ui, this, CSTR("Not Equal"), false));
	this->radNotEq->SetRect(136, 32, 96, 24, false);
	NEW_CLASS(this->lblValue, UI::GUILabel(ui, this, CSTR("Value")));
	this->lblValue->SetRect(8, 56, 56, 23, false);
	NEW_CLASS(this->txtValue, UI::GUITextBox(ui, this, CSTR("")));
	this->txtValue->SetRect(72, 56, 136, 20, false);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, CSTR("OK")));
	this->btnOK->SetRect(24, 96, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, CSTR("Cancel")));
	this->btnCancel->SetRect(136, 96, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UOSInt i = 0;
	UOSInt j = this->dbf->GetColCount();
	this->cboCol->ClearItems();
	while (i < j)
	{
		sptr = this->dbf->GetColumnName(i, sbuff);
		this->cboCol->AddItem(CSTRP(sbuff, sptr), 0);
		i++;
	}
	this->cboCol->SetSelectedIndex(0);
}

SSWR::SHPConv::SHPConvValueFilterForm::~SHPConvValueFilterForm()
{
	DEL_CLASS(this->monMgr);
}

void SSWR::SHPConv::SHPConvValueFilterForm::OnMonitorChanged()
{
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
}

SSWR::SHPConv::MapFilter *SSWR::SHPConv::SHPConvValueFilterForm::GetFilter()
{
	return this->filter;
}
