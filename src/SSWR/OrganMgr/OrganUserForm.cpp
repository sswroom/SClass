#include "Stdafx.h"
#include "SSWR/OrganMgr/OrganUserEditForm.h"
#include "SSWR/OrganMgr/OrganUserForm.h"

void __stdcall SSWR::OrganMgr::OrganUserForm::OnAddClicked(void *userObj)
{
	OrganUserForm *me = (OrganUserForm*)userObj;
	OrganUserEditForm frm(0, me->ui, me->env, 0);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		me->UpdateUserList();
	}
}
void __stdcall SSWR::OrganMgr::OrganUserForm::OnModifyClicked(void *userObj)
{
	OrganUserForm *me = (OrganUserForm*)userObj;
	OrganWebUser *user = (OrganWebUser*)me->lvUser->GetSelectedItem();
	if (user)
	{
		OrganUserEditForm frm(0, me->ui, me->env, user);
		if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			me->UpdateUserList();
		}
	}
}

void SSWR::OrganMgr::OrganUserForm::UpdateUserList()
{
	this->lvUser->ClearItems();
	this->env->ReleaseWebUsers(&this->userList);
	this->env->GetWebUsers(&this->userList);
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UTF8Char sbuff[12];
	UTF8Char *sptr;
	OrganWebUser *user;
	i = 0;
	j = this->userList.GetCount();
	while (i < j)
	{
		user = this->userList.GetItem(i);
		sptr = Text::StrInt32(sbuff, user->id);
		k = this->lvUser->AddItem(CSTRP(sbuff, sptr), user);
		this->lvUser->SetSubItem(k, 1, user->userName);
		this->lvUser->SetSubItem(k, 2, user->watermark);
		i++;
	}
}

SSWR::OrganMgr::OrganUserForm::OrganUserForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, OrganEnv *env) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 10.5, false);

	this->env = env;

	this->SetText(this->env->GetLang(CSTR("UserFormTitle")));

	NEW_CLASSNN(this->pnlCtrl, UI::GUIPanel(ui, *this));
	this->pnlCtrl->SetRect(0, 0, 100, 32, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnAdd = ui->NewButton(this->pnlCtrl, this->env->GetLang(CSTR("UserFormAdd")));
	this->btnAdd->SetRect(4, 4, 75, 23, false);
	this->btnAdd->HandleButtonClick(OnAddClicked, this);
	this->btnModify = ui->NewButton(this->pnlCtrl, this->env->GetLang(CSTR("UserFormModify")));
	this->btnModify->SetRect(84, 4, 75, 23, false);
	this->btnModify->HandleButtonClick(OnModifyClicked, this);
	NEW_CLASS(this->lvUser, UI::GUIListView(ui, *this, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvUser->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvUser->SetShowGrid(true);
	this->lvUser->SetFullRowSelect(true);
	this->lvUser->AddColumn(this->env->GetLang(CSTR("UserFormColId")), 60);
	this->lvUser->AddColumn(this->env->GetLang(CSTR("UserFormColUserName")), 200);
	this->lvUser->AddColumn(this->env->GetLang(CSTR("UserFormColWatermark")), 200);
	this->UpdateUserList();
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

SSWR::OrganMgr::OrganUserForm::~OrganUserForm()
{
	this->env->ReleaseWebUsers(&this->userList);
}

void SSWR::OrganMgr::OrganUserForm::OnMonitorChanged()
{
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}
