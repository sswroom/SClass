#include "Stdafx.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/FileDialog.h"
#include "UtilUI/TextSearchForm.h"

void __stdcall UtilUI::TextSearchForm::OnSearchClicked(void *userObj)
{
	UtilUI::TextSearchForm *me = (UtilUI::TextSearchForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtSearch->GetText(&sb);
	if (sb.GetLength() > 0)
	{
		me->mainFrm->SearchText(sb.ToString());
	}
}

void __stdcall UtilUI::TextSearchForm::OnCloseClicked(void *userObj)
{
	UtilUI::TextSearchForm *me = (UtilUI::TextSearchForm*)userObj;
	me->Close();
}

UtilUI::TextSearchForm::TextSearchForm(UI::GUIClientControl *parent, UI::GUICore *ui, Media::MonitorMgr *monMgr, UtilUI::TextViewerForm *mainFrm) : UI::GUIForm(parent, 320, 120, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText((const UTF8Char*)"Search");
	this->SetNoResize(true);
	this->monMgr = monMgr;
	this->mainFrm = mainFrm;

	NEW_CLASS(this->lblSearch, UI::GUILabel(ui, this, (const UTF8Char*)"Search Text:"));
	this->lblSearch->SetRect(8, 16, 80, 21, false);
	NEW_CLASS(this->txtSearch, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtSearch->SetRect(88, 14, 100, 23, false);
	NEW_CLASS(this->btnSearch, UI::GUIButton(ui, this, (const UTF8Char*)"&Search"));
	this->btnSearch->SetRect(8, 56, 100, 23, false);
	this->btnSearch->HandleButtonClick(OnSearchClicked, this);
	NEW_CLASS(this->btnClose, UI::GUIButton(ui, this, (const UTF8Char*)"&Close"));
	this->btnClose->SetRect(128, 56, 100, 23, false);
	this->btnClose->HandleButtonClick(OnCloseClicked, this);
	this->SetDefaultButton(this->btnSearch);
	this->SetCancelButton(this->btnClose);
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
}

UtilUI::TextSearchForm::~TextSearchForm()
{
}

void UtilUI::TextSearchForm::OnShow()
{
	this->txtSearch->Focus();
}

void UtilUI::TextSearchForm::OnMonitorChanged()
{
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
}

