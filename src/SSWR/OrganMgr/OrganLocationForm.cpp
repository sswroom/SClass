#include "Stdafx.h"
#include "SSWR/OrganMgr/OrganLocationForm.h"

void SSWR::OrganMgr::OrganLocationForm::DispId(Int32 id)
{
	Data::ArrayListNN<Location> locList;
	NN<Location> l;
	UOSInt i;
	while (this->env->LocationGet(id).SetTo(l))
	{
		locList.Add(l);
		id = l->parId;
	}
	this->lbLocation->ClearItems();
	i = locList.GetCount();
	while (i-- > 0)
	{
		l = locList.GetItemNoCheck(i);
		this->lbLocation->AddItem(l->cname, l);
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
	this->txtCName->SetText(CSTR(""));
	this->txtEName->SetText(CSTR(""));
	
	NN<Data::ArrayListNN<Location>> locSubList = this->env->LocationGetSub(parId);
	NN<Location> l;
	UOSInt i = 0;
	UOSInt j = locSubList->GetCount();
	while (i < j)
	{
		l = locSubList->GetItemNoCheck(i);
		this->lbSublocations->AddItem(l->cname, l);
		i++;
	}
	locSubList.Delete();
	this->sublocUpdating = false;
}

Bool SSWR::OrganMgr::OrganLocationForm::ToSave()
{
	UTF8Char sbuff[256];
	UTF8Char *sbuffEnd;
	UTF8Char sbuff2[256];
	UTF8Char *sbuff2End;
	if (this->currLoc == 0)
		return false;

	if ((sbuffEnd = this->txtEName->GetText(sbuff)) == sbuff)
	{
		Bool ret = this->ui->ShowMsgYesNo(this->env->GetLang(CSTR("LocationQuestionEName")), this->env->GetLang(CSTR("LocationQuestion")), this);
		return !ret;
	}
	if ((sbuff2End = this->txtCName->GetText(sbuff2)) == sbuff2)
	{
		Bool ret = this->ui->ShowMsgYesNo(this->env->GetLang(CSTR("LocationQuestionCName")), this->env->GetLang(CSTR("LocationQuestion")), this);
		return !ret;
	}

	if (this->env->LocationUpdate(currLoc->id, CSTRP(sbuff, sbuffEnd), {sbuff2, (UOSInt)(sbuff2End - sbuff2)}))
	{
		this->currLoc->ename->Release();
		this->currLoc->cname->Release();
		this->currLoc->ename = Text::String::NewP(sbuff, sbuffEnd);
		this->currLoc->cname = Text::String::NewP(sbuff2, sbuff2End);
		return false;
	}
	else
	{
		Bool ret = this->ui->ShowMsgYesNo(this->env->GetLang(CSTR("LocationQuestionError")), this->env->GetLang(CSTR("LocationQuestion")), this);
		return !ret;
	}
}

SSWR::OrganMgr::Location *SSWR::OrganMgr::OrganLocationForm::GetParentLoc()
{
	UOSInt i = this->lbLocation->GetCount();
	if (i == 0)
		return 0;
	else
		return (Location*)this->lbLocation->GetItem(i - 1).p;
}

void __stdcall SSWR::OrganMgr::OrganLocationForm::OnLocSelChg(AnyType userObj)
{
	NotNullPtr<OrganLocationForm> me = userObj.GetNN<OrganLocationForm>();
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

void __stdcall SSWR::OrganMgr::OrganLocationForm::OnSubLocSelChg(AnyType userObj)
{
	NotNullPtr<OrganLocationForm> me = userObj.GetNN<OrganLocationForm>();
	if (!me->sublocUpdating)
	{
		if (me->ToSave())
		{
			me->lbSublocations->SetSelectedIndex(me->currLocInd);
			return;
		}
	}

	me->currLocInd = me->lbSublocations->GetSelectedIndex();
	me->currLoc = (Location*)me->lbSublocations->GetItem(me->currLocInd).p;
	if (me->currLoc == 0)
	{
		me->txtCName->SetText(CSTR(""));
		me->txtEName->SetText(CSTR(""));
		me->txtID->SetText(CSTR(""));
	}
	else
	{
		UTF8Char sbuff[16];
		UTF8Char *sptr;
		sptr = Text::StrInt32(sbuff, me->currLoc->id);
		me->txtCName->SetText(me->currLoc->cname->ToCString());
		me->txtEName->SetText(me->currLoc->ename->ToCString());
		me->txtID->SetText(CSTRP(sbuff, sptr));
	}
}

void __stdcall SSWR::OrganMgr::OrganLocationForm::OnSubLocDblClk(AnyType userObj)
{
	NotNullPtr<OrganLocationForm> me = userObj.GetNN<OrganLocationForm>();
	UOSInt i = me->lbSublocations->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		if (me->ToSave())
			return;
		Location *loc = (Location*)me->lbSublocations->GetItem(i).p;
		i = me->lbLocation->AddItem(loc->cname, loc);
		me->lbLocation->SetSelectedIndex(i);
	}
}

void __stdcall SSWR::OrganMgr::OrganLocationForm::OnAddClicked(AnyType userObj)
{
	NotNullPtr<OrganLocationForm> me = userObj.GetNN<OrganLocationForm>();
	UTF8Char sbuff[256];
	UTF8Char *sbuffEnd;
	UTF8Char sbuff2[256];
	UTF8Char *sbuff2End;
	if ((sbuffEnd = me->txtEName->GetText(sbuff)) == sbuff)
	{
		me->ui->ShowMsgOK(me->env->GetLang(CSTR("LocationInputEName")), me->env->GetLang(CSTR("LocationTitle")), me);
		return;
	}
	if ((sbuff2End = me->txtCName->GetText(sbuff2)) == sbuff2)
	{
		me->ui->ShowMsgOK(me->env->GetLang(CSTR("LocationInputCName")), me->env->GetLang(CSTR("LocationTitle")), me);
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
	
	if (me->env->LocationAdd(parId, CSTRP(sbuff, sbuffEnd), {sbuff2, (UOSInt)(sbuff2End - sbuff2)}))
	{
		me->currLoc = 0;
		me->txtCName->SetText(CSTR(""));
		me->txtEName->SetText(CSTR(""));
		me->UpdateSubloc();
	}
	else
	{
		me->ui->ShowMsgOK(me->env->GetLang(CSTR("LocationAddError")), me->env->GetLang(CSTR("LocationTitle")), me);
		return;
	}
}

void __stdcall SSWR::OrganMgr::OrganLocationForm::OnOkClicked(AnyType userObj)
{
	NotNullPtr<OrganLocationForm> me = userObj.GetNN<OrganLocationForm>();
	if (me->ToSave())
		return;
	if (me->selMode == SM_CHILD)
	{
		if (me->lbSublocations->GetCount() > 0)
		{
			me->ui->ShowMsgOK(me->env->GetLang(CSTR("LocationSelectLocation")), me->env->GetLang(CSTR("LocationTitle")), me);
			return;
		}
		else
		{
			Location *o = me->GetParentLoc();
			if (o == 0)
			{
				me->ui->ShowMsgOK(me->env->GetLang(CSTR("LocationSelectLocation")), me->env->GetLang(CSTR("LocationTitle")), me);
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
		Location *l = (Location*)me->lbSublocations->GetSelectedItem().p;
		if (l)
		{
			me->currLoc = l;
		}
		else if (me->lbSublocations->GetCount() > 0)
		{
			me->ui->ShowMsgOK(me->env->GetLang(CSTR("LocationSelectLocation")), me->env->GetLang(CSTR("LocationTitle")), me);
			return;
		}
		else
		{
			l = me->GetParentLoc();
			if (l == 0)
			{
				me->ui->ShowMsgOK(me->env->GetLang(CSTR("LocationSelectLocation")), me->env->GetLang(CSTR("LocationTitle")), me);
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

void __stdcall SSWR::OrganMgr::OrganLocationForm::OnCancelClicked(AnyType userObj)
{
	NotNullPtr<OrganLocationForm> me = userObj.GetNN<OrganLocationForm>();
	if (me->ToSave())
		return;
	me->SetDialogResult(DR_CANCEL);
}

SSWR::OrganMgr::OrganLocationForm::OrganLocationForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<OrganEnv> env, SelectMode selMode, Int32 initId) : UI::GUIForm(parent, 420, 440, ui)
{
	this->SetFont(0, 0, 10.5, false);

	this->env = env;
	this->selMode = selMode;
	this->initId = initId;
	this->sublocUpdating = false;
	this->currLoc = 0;
	this->currLocInd = 0;
	this->selVal = 0;

	this->SetText(this->env->GetLang(CSTR("LocationTitle")));

	this->lbLocation = ui->NewListBox(*this, false);
	this->lbLocation->SetRect(0, 0, 96, 100, false);
	this->lbLocation->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbLocation->HandleSelectionChange(OnLocSelChg, this);
	this->lbSublocations = ui->NewListBox(*this, false);
	this->lbSublocations->SetRect(0, 0, 96, 100, false);
	this->lbSublocations->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbSublocations->HandleSelectionChange(OnSubLocSelChg, this);
	this->lbSublocations->HandleDoubleClicked(OnSubLocDblClk, this);
	this->pnlLocation = ui->NewPanel(*this);
	this->pnlLocation->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblID = ui->NewLabel(this->pnlLocation, this->env->GetLang(CSTR("LocationId")));
	this->lblID->SetRect(11, 16, 80, 23, false);
	this->lblEName = ui->NewLabel(this->pnlLocation, this->env->GetLang(CSTR("LocationEName")));
	this->lblEName->SetRect(11, 48, 80, 23, false);
	this->lblCName = ui->NewLabel(this->pnlLocation, this->env->GetLang(CSTR("LocationCName")));
	this->lblCName->SetRect(11, 80, 80, 23, false);
	this->txtID = ui->NewTextBox(this->pnlLocation, CSTR(""));	
	this->txtID->SetRect(99, 16, 88, 23, false);
	this->txtID->SetReadOnly(true);
	this->txtEName = ui->NewTextBox(this->pnlLocation, CSTR(""));	
	this->txtEName->SetRect(99, 48, 96, 23, false);
	this->txtCName = ui->NewTextBox(this->pnlLocation, CSTR(""));
	this->txtCName->SetRect(99, 80, 96, 23, false);
	this->btnAdd = ui->NewButton(this->pnlLocation, this->env->GetLang(CSTR("LocationAdd")));
	this->btnAdd->SetRect(99, 120, 75, 23, false);
	this->btnAdd->HandleButtonClick(OnAddClicked, this);
	this->btnOk = ui->NewButton(this->pnlLocation, this->env->GetLang(CSTR("LocationOk")));
	this->btnOk->SetRect(41, 368, 75, 23, false);
	this->btnOk->HandleButtonClick(OnOkClicked, this);
	this->btnCancel = ui->NewButton(this->pnlLocation, this->env->GetLang(CSTR("LocationCancel")));
	this->btnCancel->SetRect(131, 368, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	if (selMode == SM_NONE)
	{
		this->btnOk->SetVisible(false);
		this->btnCancel->SetText(this->env->GetLang(CSTR("LocationClose")));
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
