#include "Stdafx.h"
#include "SSWR/OrganMgr/OrganLocationForm.h"
#include "UI/MessageDialog.h"

void SSWR::OrganMgr::OrganLocationForm::DispId(Int32 id)
{
	Data::ArrayList<Location *> locList;
	Location *l;
	UOSInt i;
	while ((l = this->env->LocationGet(id)) != 0)
	{
		locList.Add(l);
		id = l->parId;
	}
	this->lbLocation->ClearItems();
	i = locList.GetCount();
	while (i-- > 0)
	{
		l = locList.GetItem(i);
		this->lbLocation->AddItem(l->cname->v, l);
	}
	i = locList.GetCount();
	if (i > 0)
	{
		this->lbLocation->SetSelectedIndex(i - 1);
	}
}

void SSWR::OrganMgr::OrganLocationForm::UpdateSubloc()
{
	Location *parLoc = this->GetParentLoc();
	Int32 parId;
	if (parLoc == 0)
		parId = 0;
	else
		parId = parLoc->id;

	this->sublocUpdating = true;
	this->lbSublocations->ClearItems();
	this->currLoc = 0;
	this->currLocInd = (UOSInt)-1;
	this->txtCName->SetText((const UTF8Char*)"");
	this->txtEName->SetText((const UTF8Char*)"");
	
	Data::ArrayList<Location*> *locSubList = this->env->LocationGetSub(parId);
	if (locSubList)
	{
		Location *l;
		UOSInt i = 0;
		UOSInt j = locSubList->GetCount();
		while (i < j)
		{
			l = locSubList->GetItem(i);
			this->lbSublocations->AddItem(l->cname->v, l);
			i++;
		}
		DEL_CLASS(locSubList);
	}
	this->sublocUpdating = false;
}

Bool SSWR::OrganMgr::OrganLocationForm::ToSave()
{
	UTF8Char sbuff[256];
	UTF8Char sbuff2[256];
	if (this->currLoc == 0)
		return false;

	if (this->txtEName->GetText(sbuff) == sbuff)
	{
		Bool ret = UI::MessageDialog::ShowYesNoDialog(this->env->GetLang((const UTF8Char*)"LocationQuestionEName"), this->env->GetLang((const UTF8Char*)"LocationQuestion"), this);
		return !ret;
	}
	if (this->txtCName->GetText(sbuff2) == sbuff2)
	{
		Bool ret = UI::MessageDialog::ShowYesNoDialog(this->env->GetLang((const UTF8Char*)"LocationQuestionCName"), this->env->GetLang((const UTF8Char*)"LocationQuestion"), this);
		return !ret;
	}

	if (this->env->LocationUpdate(currLoc->id, sbuff, sbuff2))
	{
		SDEL_STRING(this->currLoc->ename);
		SDEL_STRING(this->currLoc->cname);
		this->currLoc->ename = Text::String::New(sbuff);
		this->currLoc->cname = Text::String::New(sbuff2);
		return false;
	}
	else
	{
		Bool ret = UI::MessageDialog::ShowYesNoDialog(this->env->GetLang((const UTF8Char*)"LocationQuestionError"), this->env->GetLang((const UTF8Char*)"LocationQuestion"), this);
		return !ret;
	}
}

SSWR::OrganMgr::Location *SSWR::OrganMgr::OrganLocationForm::GetParentLoc()
{
	UOSInt i = this->lbLocation->GetCount();
	if (i == 0)
		return 0;
	else
		return (Location*)this->lbLocation->GetItem(i - 1);
}

void __stdcall SSWR::OrganMgr::OrganLocationForm::OnLocSelChg(void *userObj)
{
	OrganLocationForm *me = (OrganLocationForm*)userObj;
	if (me->ToSave())
	{
		me->lbLocation->SetSelectedIndex(me->lbLocation->GetCount() - 1);
		return;
	}
	
	UOSInt i = (UOSInt)me->lbLocation->GetSelectedIndex();
	UOSInt j = me->lbLocation->GetCount() - 1;
	while (j > i)
	{
		me->lbLocation->RemoveItem(j);
		j -= 1;
	}
	me->UpdateSubloc();
}

void __stdcall SSWR::OrganMgr::OrganLocationForm::OnSubLocSelChg(void *userObj)
{
	OrganLocationForm *me = (OrganLocationForm*)userObj;
	if (!me->sublocUpdating)
	{
		if (me->ToSave())
		{
			me->lbSublocations->SetSelectedIndex(me->currLocInd);
			return;
		}
	}

	me->currLocInd = me->lbSublocations->GetSelectedIndex();
	me->currLoc = (Location*)me->lbSublocations->GetItem(me->currLocInd);
	if (me->currLoc == 0)
	{
		me->txtCName->SetText((const UTF8Char*)"");
		me->txtEName->SetText((const UTF8Char*)"");
		me->txtID->SetText((const UTF8Char*)"");
	}
	else
	{
		UTF8Char sbuff[16];
		Text::StrInt32(sbuff, me->currLoc->id);
		me->txtCName->SetText(me->currLoc->cname->v);
		me->txtEName->SetText(me->currLoc->ename->v);
		me->txtID->SetText(sbuff);
	}
}

void __stdcall SSWR::OrganMgr::OrganLocationForm::OnSubLocDblClk(void *userObj)
{
	OrganLocationForm *me = (OrganLocationForm*)userObj;
	UOSInt i = me->lbSublocations->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		if (me->ToSave())
			return;
		Location *loc = (Location*)me->lbSublocations->GetItem(i);
		i = me->lbLocation->AddItem(loc->cname->v, loc);
		me->lbLocation->SetSelectedIndex(i);
	}
}

void __stdcall SSWR::OrganMgr::OrganLocationForm::OnAddClicked(void *userObj)
{
	OrganLocationForm *me = (OrganLocationForm*)userObj;
	UTF8Char sbuff[256];
	UTF8Char sbuff2[256];
	if (me->txtEName->GetText(sbuff) == sbuff)
	{
		UI::MessageDialog::ShowDialog(me->env->GetLang((const UTF8Char*)"LocationInputEName"), me->env->GetLang((const UTF8Char*)"LocationTitle"), me);
		return;
	}
	if (me->txtCName->GetText(sbuff2) == sbuff2)
	{
		UI::MessageDialog::ShowDialog(me->env->GetLang((const UTF8Char*)"LocationInputCName"), me->env->GetLang((const UTF8Char*)"LocationTitle"), me);
		return;
	}
	Location *parLoc = me->GetParentLoc();
	Int32 parId;
	if (parLoc == 0)
	{
		parId = 0;
	}
	else
	{
		parId = parLoc->id;
	}
	
	if (me->env->LocationAdd(parId, sbuff, sbuff2))
	{
		me->currLoc = 0;
		me->txtCName->SetText((const UTF8Char*)"");
		me->txtEName->SetText((const UTF8Char*)"");
		me->UpdateSubloc();
	}
	else
	{
		UI::MessageDialog::ShowDialog(me->env->GetLang((const UTF8Char*)"LocationAddError"), me->env->GetLang((const UTF8Char*)"LocationTitle"), me);
		return;
	}
}

void __stdcall SSWR::OrganMgr::OrganLocationForm::OnOkClicked(void *userObj)
{
	OrganLocationForm *me = (OrganLocationForm*)userObj;
	if (me->ToSave())
		return;
	if (me->selMode == SM_CHILD)
	{
		if (me->lbSublocations->GetCount() > 0)
		{
			UI::MessageDialog::ShowDialog(me->env->GetLang((const UTF8Char*)"LocationSelectLocation"), me->env->GetLang((const UTF8Char*)"LocationTitle"), me);
			return;
		}
		else
		{
			Location *o = me->GetParentLoc();
			if (o == 0)
			{
				UI::MessageDialog::ShowDialog(me->env->GetLang((const UTF8Char*)"LocationSelectLocation"), me->env->GetLang((const UTF8Char*)"LocationTitle"), me);
				return;
			}
			else
			{
				me->currLoc = o;
			}
		}
	}
	else if (me->selMode == SM_ANY)
	{
		Location *l = (Location*)me->lbSublocations->GetSelectedItem();
		if (l)
		{
			me->currLoc = l;
		}
		else if (me->lbSublocations->GetCount() > 0)
		{
			UI::MessageDialog::ShowDialog(me->env->GetLang((const UTF8Char*)"LocationSelectLocation"), me->env->GetLang((const UTF8Char*)"LocationTitle"), me);
			return;
		}
		else
		{
			l = me->GetParentLoc();
			if (l == 0)
			{
				UI::MessageDialog::ShowDialog(me->env->GetLang((const UTF8Char*)"LocationSelectLocation"), me->env->GetLang((const UTF8Char*)"LocationTitle"), me);
				return;
			}
			else
			{
				me->currLoc = l;
			}
		}
	}
	else
	{
		return;
	}
	me->selVal = me->currLoc;
	me->SetDialogResult(DR_OK);
}

void __stdcall SSWR::OrganMgr::OrganLocationForm::OnCancelClicked(void *userObj)
{
	OrganLocationForm *me = (OrganLocationForm*)userObj;
	if (me->ToSave())
		return;
	me->SetDialogResult(DR_CANCEL);
}

SSWR::OrganMgr::OrganLocationForm::OrganLocationForm(UI::GUIClientControl *parent, UI::GUICore *ui, OrganEnv *env, SelectMode selMode, Int32 initId) : UI::GUIForm(parent, 420, 440, ui)
{
	this->SetFont(0, 10.5, false);

	this->env = env;
	this->selMode = selMode;
	this->initId = initId;
	this->sublocUpdating = false;
	this->currLoc = 0;
	this->currLocInd = 0;
	this->selVal = 0;

	this->SetText(this->env->GetLang((const UTF8Char*)"LocationTitle"));

	NEW_CLASS(this->lbLocation, UI::GUIListBox(ui, this, false));
	this->lbLocation->SetRect(0, 0, 96, 100, false);
	this->lbLocation->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbLocation->HandleSelectionChange(OnLocSelChg, this);
	NEW_CLASS(this->lbSublocations, UI::GUIListBox(ui, this, false));
	this->lbSublocations->SetRect(0, 0, 96, 100, false);
	this->lbSublocations->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbSublocations->HandleSelectionChange(OnSubLocSelChg, this);
	this->lbSublocations->HandleDoubleClicked(OnSubLocDblClk, this);
	NEW_CLASS(this->pnlLocation, UI::GUIPanel(ui, this));
	this->pnlLocation->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblID, UI::GUILabel(ui, this->pnlLocation, this->env->GetLang((const UTF8Char*)"LocationId")));
	this->lblID->SetRect(11, 16, 80, 23, false);
	NEW_CLASS(this->lblEName, UI::GUILabel(ui, this->pnlLocation, this->env->GetLang((const UTF8Char*)"LocationEName")));
	this->lblEName->SetRect(11, 48, 80, 23, false);
	NEW_CLASS(this->lblCName, UI::GUILabel(ui, this->pnlLocation, this->env->GetLang((const UTF8Char*)"LocationCName")));
	this->lblCName->SetRect(11, 80, 80, 23, false);
	NEW_CLASS(this->txtID, UI::GUITextBox(ui, this->pnlLocation, (const UTF8Char*)""));	
	this->txtID->SetRect(99, 16, 88, 23, false);
	this->txtID->SetReadOnly(true);
	NEW_CLASS(this->txtEName, UI::GUITextBox(ui, this->pnlLocation, (const UTF8Char*)""));	
	this->txtEName->SetRect(99, 48, 96, 23, false);
	NEW_CLASS(this->txtCName, UI::GUITextBox(ui, this->pnlLocation, (const UTF8Char*)""));	
	this->txtCName->SetRect(99, 80, 96, 23, false);
	NEW_CLASS(this->btnAdd, UI::GUIButton(ui, this->pnlLocation, this->env->GetLang((const UTF8Char*)"LocationAdd")));
	this->btnAdd->SetRect(99, 120, 75, 23, false);
	this->btnAdd->HandleButtonClick(OnAddClicked, this);
	NEW_CLASS(this->btnOk, UI::GUIButton(ui, this->pnlLocation, this->env->GetLang((const UTF8Char*)"LocationOk")));
	this->btnOk->SetRect(41, 368, 75, 23, false);
	this->btnOk->HandleButtonClick(OnOkClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this->pnlLocation, this->env->GetLang((const UTF8Char*)"LocationCancel")));
	this->btnCancel->SetRect(131, 368, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	if (selMode == SM_NONE)
	{
		this->btnOk->SetVisible(false);
		this->btnCancel->SetText(this->env->GetLang((const UTF8Char*)"LocationClose"));
	}

	if (initId > 0)
	{
		this->DispId(initId);
	}
	this->UpdateSubloc();
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

SSWR::OrganMgr::OrganLocationForm::~OrganLocationForm()
{
}

void SSWR::OrganMgr::OrganLocationForm::OnMonitorChanged()
{
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

SSWR::OrganMgr::Location *SSWR::OrganMgr::OrganLocationForm::GetSelVal()
{
	return this->selVal;
}
