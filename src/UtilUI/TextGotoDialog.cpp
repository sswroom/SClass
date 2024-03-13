#include "Stdafx.h"
#include "Text/MyString.h"
#include "UtilUI/TextGotoDialog.h"

void __stdcall UtilUI::TextGotoDialog::OnOKClicked(void *userObj)
{
	UTF8Char sbuff[20];
	UtilUI::TextGotoDialog *me = (UtilUI::TextGotoDialog*)userObj;

	UOSInt lineNum;
	me->txtLine->GetText(sbuff);
	if (Text::StrToUOSInt(sbuff, lineNum))
	{
		if (lineNum > 0)
		{
			me->currLine = lineNum;
			me->SetDialogResult(UI::GUIForm::DR_OK);
		}
	}
}

void __stdcall UtilUI::TextGotoDialog::OnCancelClicked(void *userObj)
{
	UtilUI::TextGotoDialog *me = (UtilUI::TextGotoDialog*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

UtilUI::TextGotoDialog::TextGotoDialog(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, Media::MonitorMgr *monMgr) : UI::GUIForm(parent, 320, 120, ui)
{
	this->currLine = 0;
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Go To Line"));
	this->SetNoResize(true);
	this->monMgr = monMgr;

	this->lblLine = ui->NewLabel(*this, CSTR("Line Number:"));
	this->lblLine->SetRect(8, 16, 80, 21, false);
	this->txtLine = ui->NewTextBox(*this, CSTR("0"));
	this->txtLine->SetRect(88, 14, 100, 23, false);
	this->btnOk = ui->NewButton(*this, CSTR("&Ok"));
	this->btnOk->SetRect(8, 56, 100, 23, false);
	this->btnOk->HandleButtonClick(OnOKClicked, this);
	this->btnCancel = ui->NewButton(*this, CSTR("&Cancel"));
	this->btnCancel->SetRect(128, 56, 100, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->SetDefaultButton(this->btnOk);
	this->SetCancelButton(this->btnCancel);
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
}

UtilUI::TextGotoDialog::~TextGotoDialog()
{
}

void UtilUI::TextGotoDialog::OnShow()
{
	this->txtLine->Focus();
}

void UtilUI::TextGotoDialog::OnMonitorChanged()
{
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
}

void UtilUI::TextGotoDialog::SetLineNumber(UOSInt currLine)
{
	UTF8Char sbuff[22];
	UTF8Char *sptr;
	this->currLine = currLine;
	sptr = Text::StrUOSInt(sbuff, currLine);
	this->txtLine->SetText(CSTRP(sbuff, sptr));
}

UOSInt UtilUI::TextGotoDialog::GetLineNumber()
{
	return this->currLine;
}
