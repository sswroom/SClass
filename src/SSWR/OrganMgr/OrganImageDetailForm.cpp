#include "Stdafx.h"
#include "SSWR/OrganMgr/OrganImageDetailForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::OrganMgr::OrganImageDetailForm::OnOKClicked(void *userObj)
{
	OrganImageDetailForm *me = (OrganImageDetailForm *)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtDescript->GetText(sb);
	SDEL_TEXT(me->descript);
	me->descript = Text::StrCopyNew(sb.ToString()).Ptr();
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::OrganMgr::OrganImageDetailForm::OnCancelClicked(void *userObj)
{
	OrganImageDetailForm *me = (OrganImageDetailForm *)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::OrganMgr::OrganImageDetailForm::OrganImageDetailForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, OrganEnv *env, UserFileInfo *userFile) : UI::GUIForm(parent, 420, 224, ui)
{
	this->SetNoResize(true);
	this->SetFont(0, 0, 10.5, false);

	this->env = env;
	this->descript = 0;

	this->SetText(this->env->GetLang(CSTR("ImageDetailTitle")));

	UTF8Char sbuff[64];
	UTF8Char *sptr;
	sptr = Text::StrInt32(sbuff, userFile->id);
	this->lblId = ui->NewLabel(*this, this->env->GetLang(CSTR("ImageDetailId")));
	this->lblId->SetRect(0, 0, 100, 23, false);
	this->txtId = ui->NewTextBox(*this, CSTRP(sbuff, sptr));
	this->txtId->SetRect(100, 0, 100, 23, false);
	this->txtId->SetReadOnly(true);
	this->lblFileName = ui->NewLabel(*this, this->env->GetLang(CSTR("ImageDetailFileName")));
	this->lblFileName->SetRect(0, 24, 100, 23, false);
	this->txtFileName = ui->NewTextBox(*this, userFile->oriFileName->ToCString());
	this->txtFileName->SetRect(100, 24, 300, 23, false);
	this->txtFileName->SetReadOnly(true);
	this->lblDescript = ui->NewLabel(*this, this->env->GetLang(CSTR("ImageDetailDescript")));
	this->lblDescript->SetRect(0, 48, 100, 23, false);
	this->txtDescript = ui->NewTextBox(*this, Text::String::OrEmpty(userFile->descript)->ToCString());
	this->txtDescript->SetRect(100, 48, 300, 23, false);
	this->lblCamera = ui->NewLabel(*this, this->env->GetLang(CSTR("ImageDetailCamera")));
	this->lblCamera->SetRect(0, 72, 100, 23, false);
	this->txtCamera = ui->NewTextBox(*this, Text::String::OrEmpty(userFile->camera)->ToCString());
	this->txtCamera->SetRect(100, 72, 300, 23, false);
	this->txtCamera->SetReadOnly(true);
	sptr = userFile->fileTime.ToLocalTime().ToString(sbuff);
	this->lblFileTime = ui->NewLabel(*this, this->env->GetLang(CSTR("ImageDetailFileTime")));
	this->lblFileTime->SetRect(0, 96, 100, 23, false);
	this->txtFileTime = ui->NewTextBox(*this, CSTRP(sbuff, sptr));
	this->txtFileTime->SetRect(100, 96, 200, 23, false);
	this->txtFileTime->SetReadOnly(true);
	sptr = Text::StrDouble(sbuff, userFile->lat);
	this->lblLat = ui->NewLabel(*this, this->env->GetLang(CSTR("ImageDetailLat")));
	this->lblLat->SetRect(0, 120, 100, 23, false);
	this->txtLat = ui->NewTextBox(*this, CSTRP(sbuff, sptr));
	this->txtLat->SetRect(100, 120, 150, 23, false);
	this->txtLat->SetReadOnly(true);
	sptr = Text::StrDouble(sbuff, userFile->lon);
	this->lblLon = ui->NewLabel(*this, this->env->GetLang(CSTR("ImageDetailLon")));
	this->lblLon->SetRect(0, 144, 100, 23, false);
	this->txtLon = ui->NewTextBox(*this, CSTRP(sbuff, sptr));
	this->txtLon->SetRect(100, 144, 150, 23, false);
	this->txtLon->SetReadOnly(true);
	this->btnOK = ui->NewButton(*this, this->env->GetLang(CSTR("ImageDetailOk")));
	this->btnOK->SetRect(120, 168, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->btnCancel = ui->NewButton(*this, this->env->GetLang(CSTR("ImageDetailCancel")));
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
