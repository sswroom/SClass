#include "Stdafx.h"
#include "SSWR/OrganMgr/OrganUserEditForm.h"

void __stdcall SSWR::OrganMgr::OrganUserEditForm::OnOKClicked(void *userObj)
{
	OrganUserEditForm *me = (OrganUserEditForm *)userObj;
	Text::StringBuilderUTF8 sb1;
	Text::StringBuilderUTF8 sb2;
	Text::StringBuilderUTF8 sb3;
	me->txtPassword->GetText(sb1);
	me->txtRetype->GetText(sb2);
	if (!sb1.Equals(sb2))
	{
		me->ui->ShowMsgOK(CSTR("Password and retype password do not match"), CSTR("Error"), me);
		return;
	}
	sb1.ClearStr();
	me->txtUserName->GetText(sb1);
	me->txtWatermark->GetText(sb3);
	if (sb1.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter user name"), CSTR("Error"), me);
		return;
	}
	if (sb3.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter watermark"), CSTR("Error"), me);
		return;
	}
	if (me->user == 0)
	{
		if (sb2.GetLength() == 0)
		{
			me->ui->ShowMsgOK(CSTR("Please enter password"), CSTR("Error"), me);
			return;
		}
		if (me->env->AddWebUser(sb1.ToString(), sb2.ToString(), sb3.ToString(), SSWR::OrganMgr::UT_USER))
		{
			me->SetDialogResult(UI::GUIForm::DR_OK);
		}
	}
	else
	{
		if (sb2.GetLength() == 0)
		{
			if (me->env->ModifyWebUser(me->user->id, sb1.ToString(), 0, sb3.ToString()))
			{
				me->SetDialogResult(UI::GUIForm::DR_OK);
			}
		}
		else
		{
			if (me->env->ModifyWebUser(me->user->id, sb1.ToString(), sb2.ToString(), sb3.ToString()))
			{
				me->SetDialogResult(UI::GUIForm::DR_OK);
			}
		}
	}
}

void __stdcall SSWR::OrganMgr::OrganUserEditForm::OnCancelClicked(void *userObj)
{
	OrganUserEditForm *me = (OrganUserEditForm *)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::OrganMgr::OrganUserEditForm::OrganUserEditForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, OrganEnv *env, OrganWebUser *user) : UI::GUIForm(parent, 480, 160, ui)
{
	this->SetFont(0, 0, 10.5, false);
	this->SetNoResize(true);

	this->env = env;
	this->user = user;

	if (user)
	{
		this->SetText(this->env->GetLang(CSTR("UserEditTitleModify")));
	}
	else
	{
		this->SetText(this->env->GetLang(CSTR("UserEditTitleModify")));
	}

	this->lblUserName = ui->NewLabel(*this, this->env->GetLang(CSTR("UserEditUserName")));
	this->lblUserName->SetRect(64, 4, 100, 23, false);
	this->txtUserName = ui->NewTextBox(*this, CSTR(""));
	this->txtUserName->SetRect(164, 4, 200, 23, false);
	this->lblPassword = ui->NewLabel(*this, this->env->GetLang(CSTR("UserEditPassword")));
	this->lblPassword->SetRect(64, 28, 100, 23, false);
	this->txtPassword = ui->NewTextBox(*this, CSTR(""));
	this->txtPassword->SetRect(164, 28, 200, 23, false);
	this->txtPassword->SetPasswordChar('*');
	this->lblRetype = ui->NewLabel(*this, this->env->GetLang(CSTR("UserEditRetype")));
	this->lblRetype->SetRect(64, 52, 100, 23, false);
	this->txtRetype = ui->NewTextBox(*this, CSTR(""));
	this->txtRetype->SetRect(164, 52, 200, 23, false);
	this->txtRetype->SetPasswordChar('*');
	this->lblWatermark = ui->NewLabel(*this, this->env->GetLang(CSTR("UserEditWatermark")));
	this->lblWatermark->SetRect(64, 76, 100, 23, false);
	this->txtWatermark = ui->NewTextBox(*this, CSTR(""));
	this->txtWatermark->SetRect(164, 76, 200, 23, false);
	this->btnOK = ui->NewButton(*this, this->env->GetLang(CSTR("UserEditOk")));
	this->btnOK->SetRect(164, 108, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->btnCancel = ui->NewButton(*this, this->env->GetLang(CSTR("UserEditCancel")));
	this->btnCancel->SetRect(244, 108, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	if (this->user)
	{
		this->txtUserName->SetText(this->user->userName->ToCString());
		this->txtWatermark->SetText(this->user->watermark->ToCString());
	}
	this->txtUserName->Focus();
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

SSWR::OrganMgr::OrganUserEditForm::~OrganUserEditForm()
{
}

void SSWR::OrganMgr::OrganUserEditForm::OnMonitorChanged()
{
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}
