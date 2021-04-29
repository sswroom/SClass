#include "Stdafx.h"
#include "Text/MyString.h"
#include "UI/FileDialog.h"
#include "UtilUI/TextGotoDialog.h"

void __stdcall UtilUI::TextGotoDialog::OnOKClicked(void *userObj)
{
	UTF8Char sbuff[20];
	UtilUI::TextGotoDialog *me = (UtilUI::TextGotoDialog*)userObj;

	UOSInt lineNum;
	me->txtLine->GetText(sbuff);
	if (Text::StrToUOSInt(sbuff, &lineNum))
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

UtilUI::TextGotoDialog::TextGotoDialog(UI::GUIClientControl *parent, UI::GUICore *ui, Media::MonitorMgr *monMgr) : UI::GUIForm(parent, 320, 120, ui)
{
	this->currLine = 0;
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"Go To Line");
	this->SetNoResize(true);
	this->monMgr = monMgr;

	NEW_CLASS(this->lblLine, UI::GUILabel(ui, this, (const UTF8Char*)"Line Number:"));
	this->lblLine->SetRect(8, 16, 80, 21, false);
	NEW_CLASS(this->txtLine, UI::GUITextBox(ui, this, (const UTF8Char*)"0"));
	this->txtLine->SetRect(88, 14, 100, 23, false);
	NEW_CLASS(this->btnOk, UI::GUIButton(ui, this, (const UTF8Char*)"&Ok"));
	this->btnOk->SetRect(8, 56, 100, 23, false);
	this->btnOk->HandleButtonClick(OnOKClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, (const UTF8Char*)"&Cancel"));
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
	this->currLine = currLine;
	Text::StrUOSInt(sbuff, currLine);
	this->txtLine->SetText(sbuff);
}

UOSInt UtilUI::TextGotoDialog::GetLineNumber()
{
	return this->currLine;
}
