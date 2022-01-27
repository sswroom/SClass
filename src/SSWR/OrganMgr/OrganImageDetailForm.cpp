#include "Stdafx.h"
#include "SSWR/OrganMgr/OrganImageDetailForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::OrganMgr::OrganImageDetailForm::OnOKClicked(void *userObj)
{
	OrganImageDetailForm *me = (OrganImageDetailForm *)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtDescript->GetText(&sb);
	SDEL_TEXT(me->descript);
	me->descript = Text::StrCopyNew(sb.ToString());
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::OrganMgr::OrganImageDetailForm::OnCancelClicked(void *userObj)
{
	OrganImageDetailForm *me = (OrganImageDetailForm *)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::OrganMgr::OrganImageDetailForm::OrganImageDetailForm(UI::GUIClientControl *parent, UI::GUICore *ui, OrganEnv *env, UserFileInfo *userFile) : UI::GUIForm(parent, 420, 224, ui)
{
	this->SetNoResize(true);
	this->SetFont(0, 0, 10.5, false);

	this->env = env;
	this->descript = 0;

	this->SetText(this->env->GetLang(UTF8STRC("ImageDetailTitle")).v);

	UTF8Char sbuff[64];
	Data::DateTime dt;
	Text::StrInt32(sbuff, userFile->id);
	NEW_CLASS(this->lblId, UI::GUILabel(ui, this, this->env->GetLang(UTF8STRC("ImageDetailId")).v));
	this->lblId->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->txtId, UI::GUITextBox(ui, this, sbuff));
	this->txtId->SetRect(100, 0, 100, 23, false);
	this->txtId->SetReadOnly(true);
	NEW_CLASS(this->lblFileName, UI::GUILabel(ui, this, this->env->GetLang(UTF8STRC("ImageDetailFileName")).v));
	this->lblFileName->SetRect(0, 24, 100, 23, false);
	NEW_CLASS(this->txtFileName, UI::GUITextBox(ui, this, userFile->oriFileName->v));
	this->txtFileName->SetRect(100, 24, 300, 23, false);
	this->txtFileName->SetReadOnly(true);
	NEW_CLASS(this->lblDescript, UI::GUILabel(ui, this, this->env->GetLang(UTF8STRC("ImageDetailDescript")).v));
	this->lblDescript->SetRect(0, 48, 100, 23, false);
	NEW_CLASS(this->txtDescript, UI::GUITextBox(ui, this, Text::String::OrEmpty(userFile->descript)->v));
	this->txtDescript->SetRect(100, 48, 300, 23, false);
	NEW_CLASS(this->lblCamera, UI::GUILabel(ui, this, this->env->GetLang(UTF8STRC("ImageDetailCamera")).v));
	this->lblCamera->SetRect(0, 72, 100, 23, false);
	NEW_CLASS(this->txtCamera, UI::GUITextBox(ui, this, Text::String::OrEmpty(userFile->camera)->v));
	this->txtCamera->SetRect(100, 72, 300, 23, false);
	this->txtCamera->SetReadOnly(true);
	dt.SetTicks(userFile->fileTimeTicks);
	dt.ToLocalTime();
	dt.ToString(sbuff);
	NEW_CLASS(this->lblFileTime, UI::GUILabel(ui, this, this->env->GetLang(UTF8STRC("ImageDetailFileTime")).v));
	this->lblFileTime->SetRect(0, 96, 100, 23, false);
	NEW_CLASS(this->txtFileTime, UI::GUITextBox(ui, this, sbuff));
	this->txtFileTime->SetRect(100, 96, 200, 23, false);
	this->txtFileTime->SetReadOnly(true);
	Text::StrDouble(sbuff, userFile->lat);
	NEW_CLASS(this->lblLat, UI::GUILabel(ui, this, this->env->GetLang(UTF8STRC("ImageDetailLat")).v));
	this->lblLat->SetRect(0, 120, 100, 23, false);
	NEW_CLASS(this->txtLat, UI::GUITextBox(ui, this, sbuff));
	this->txtLat->SetRect(100, 120, 150, 23, false);
	this->txtLat->SetReadOnly(true);
	Text::StrDouble(sbuff, userFile->lon);
	NEW_CLASS(this->lblLon, UI::GUILabel(ui, this, this->env->GetLang(UTF8STRC("ImageDetailLon")).v));
	this->lblLon->SetRect(0, 144, 100, 23, false);
	NEW_CLASS(this->txtLon, UI::GUITextBox(ui, this, sbuff));
	this->txtLon->SetRect(100, 144, 150, 23, false);
	this->txtLon->SetReadOnly(true);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, this->env->GetLang(UTF8STRC("ImageDetailOk")).v));
	this->btnOK->SetRect(120, 168, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, this->env->GetLang(UTF8STRC("ImageDetailCancel")).v));
	this->btnCancel->SetRect(200, 168, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	this->txtDescript->Focus();
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

SSWR::OrganMgr::OrganImageDetailForm::~OrganImageDetailForm()
{
	SDEL_TEXT(this->descript);
}

void SSWR::OrganMgr::OrganImageDetailForm::OnMonitorChanged()
{
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

const UTF8Char *SSWR::OrganMgr::OrganImageDetailForm::GetDescript()
{
	return this->descript;
}
