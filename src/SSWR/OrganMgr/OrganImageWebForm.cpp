#include "Stdafx.h"
#include "SSWR/OrganMgr/OrganImageItem.h"
#include "SSWR/OrganMgr/OrganImageWebForm.h"
#include "Text/MyString.h"

void __stdcall SSWR::OrganMgr::OrganImageWebForm::OnOKClicked(void *userObj)
{
	OrganImageWebForm *me = (OrganImageWebForm *)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtSourceURL->GetText(&sb);
	SDEL_STRING(me->srcURL);
	me->srcURL = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();

	sb.ClearStr();
	me->txtLocation->GetText(&sb);
	SDEL_STRING(me->location);
	me->location = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::OrganMgr::OrganImageWebForm::OnCancelClicked(void *userObj)
{
	OrganImageWebForm *me = (OrganImageWebForm *)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::OrganMgr::OrganImageWebForm::OrganImageWebForm(UI::GUIClientControl *parent, UI::GUICore *ui, OrganEnv *env, OrganImageItem *imgItem, SSWR::OrganMgr::WebFileInfo *wfile) : UI::GUIForm(parent, 800, 176, ui)
{
	this->SetNoResize(true);
	this->SetFont(0, 0, 10.5, false);

	this->env = env;
	this->srcURL = 0;
	this->location = 0;

	this->SetText(this->env->GetLang(UTF8STRC("ImageWebTitle")));

	NEW_CLASS(this->lblId, UI::GUILabel(ui, this, this->env->GetLang(UTF8STRC("ImageWebId"))));
	this->lblId->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->txtId, UI::GUITextBox(ui, this, imgItem->GetDispName()->ToCString()));
	this->txtId->SetRect(100, 0, 200, 23, false);
	this->txtId->SetReadOnly(true);
	NEW_CLASS(this->lblFileName, UI::GUILabel(ui, this, this->env->GetLang(UTF8STRC("ImageWebFileName"))));
	this->lblFileName->SetRect(0, 24, 100, 23, false);
	NEW_CLASS(this->txtFileName, UI::GUITextBox(ui, this, imgItem->GetFullName()->ToCString()));
	this->txtFileName->SetRect(100, 24, 680, 23, false);
	this->txtFileName->SetReadOnly(true);
	NEW_CLASS(this->lblImageURL, UI::GUILabel(ui, this, this->env->GetLang(UTF8STRC("ImageWebImageURL"))));
	this->lblImageURL->SetRect(0, 48, 100, 23, false);
	NEW_CLASS(this->txtImageURL, UI::GUITextBox(ui, this, Text::String::OrEmpty(imgItem->GetImgURL())->ToCString()));
	this->txtImageURL->SetRect(100, 48, 680, 23, false);
	this->txtImageURL->SetReadOnly(true);
	NEW_CLASS(this->lblSourceURL, UI::GUILabel(ui, this, this->env->GetLang(UTF8STRC("ImageWebSourceURL"))));
	this->lblSourceURL->SetRect(0, 72, 100, 23, false);
	NEW_CLASS(this->txtSourceURL, UI::GUITextBox(ui, this, Text::String::OrEmpty(imgItem->GetSrcURL())->ToCString()));
	this->txtSourceURL->SetRect(100, 72, 680, 23, false);
	NEW_CLASS(this->lblLocation, UI::GUILabel(ui, this, this->env->GetLang(UTF8STRC("ImageWebLocation"))));
	this->lblLocation->SetRect(0, 96, 100, 23, false);
	NEW_CLASS(this->txtLocation, UI::GUITextBox(ui, this, wfile->location->ToCString()));
	this->txtLocation->SetRect(100, 96, 680, 23, false);
	if (wfile == 0)
	{
		this->txtLocation->SetReadOnly(true);
	}
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, this->env->GetLang(UTF8STRC("ImageWebOk"))));
	this->btnOK->SetRect(120, 120, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, this->env->GetLang(UTF8STRC("ImageWebCancel"))));
	this->btnCancel->SetRect(200, 120, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	if (wfile == 0 || wfile->srcUrl->Equals(wfile->imgUrl.Ptr()) || wfile->srcUrl->Equals(UTF8STRC("chrome://browser/content/browser.xhtml")))
	{
		this->txtSourceURL->Focus();
		this->txtSourceURL->SelectAll();
	}
	else
	{
		this->txtLocation->Focus();
		this->txtLocation->SelectAll();
	}
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

SSWR::OrganMgr::OrganImageWebForm::~OrganImageWebForm()
{
	SDEL_STRING(this->srcURL);
	SDEL_STRING(this->location);
}

void SSWR::OrganMgr::OrganImageWebForm::OnMonitorChanged()
{
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

Text::String *SSWR::OrganMgr::OrganImageWebForm::GetSrcURL()
{
	return this->srcURL;
}

Text::String *SSWR::OrganMgr::OrganImageWebForm::GetLocation()
{
	return this->location;
}
