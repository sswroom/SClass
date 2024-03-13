#include "Stdafx.h"
#include "SSWR/OrganMgr/OrganSelCategoryForm.h"

void __stdcall SSWR::OrganMgr::OrganSelCategoryForm::OnOKClicked(void *userObj)
{
	OrganSelCategoryForm *me = (OrganSelCategoryForm*)userObj;
	UOSInt i = me->lbCategory->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		Category *cate = (Category*)me->lbCategory->GetItem(i);
		me->env->SetCurrCategory(cate);
		me->SetDialogResult(DR_OK);
	}
	else
	{
	}
}

void __stdcall SSWR::OrganMgr::OrganSelCategoryForm::OnCancelClicked(void *userObj)
{
	OrganSelCategoryForm *me = (OrganSelCategoryForm*)userObj;
	me->SetDialogResult(DR_CANCEL);
}

SSWR::OrganMgr::OrganSelCategoryForm::OrganSelCategoryForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, OrganEnv *env) : UI::GUIForm(parent, 374, 262, ui)
{
	this->SetNoResize(true);
	this->SetFont(0, 0, 10.5, false);
	this->env = env;

	this->SetText(this->env->GetLang(CSTR("SelCategoryTitle")));

	this->lbl = ui->NewLabel(*this, this->env->GetLang(CSTR("SelCategoryDB")));
	this->lbl->SetRect(16, 8, 100, 23, false);
	this->lbCategory = ui->NewListBox(*this, false);
	this->lbCategory->SetRect(8, 32, 344, 136, false);
	this->lbCategory->HandleDoubleClicked(OnOKClicked, this);
	this->btnOK = ui->NewButton(*this, this->env->GetLang(CSTR("SelCategoryOk")));
	this->btnOK->SetRect(72, 192, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->btnCancel = ui->NewButton(*this, this->env->GetLang(CSTR("SelCategoryCancel")));
	this->btnCancel->SetRect(216, 192, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	UOSInt i;
	UOSInt j;
	Data::ArrayList<Category *> cates;
	env->GetCategories(&cates);
	i = 0;
	j = cates.GetCount();
	while (i < j)
	{
		Category *cate;
		cate = cates.GetItem(i);
		this->lbCategory->AddItem(cate->chiName, cate);
		i++;
	}
	this->lbCategory->Focus();
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

SSWR::OrganMgr::OrganSelCategoryForm::~OrganSelCategoryForm()
{
}

void SSWR::OrganMgr::OrganSelCategoryForm::OnMonitorChanged()
{
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}
