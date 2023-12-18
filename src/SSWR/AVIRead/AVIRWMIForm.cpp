#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRWMIForm.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilder.h"
#include "Win32/WMIQuery.h"

void __stdcall SSWR::AVIRead::AVIRWMIForm::OnOKClick(void *userObj)
{
	SSWR::AVIRead::AVIRWMIForm *me = (SSWR::AVIRead::AVIRWMIForm*)userObj;
	NotNullPtr<Text::String> ns;
	if (me->lbNS->GetSelectedItemTextNew().SetTo(ns))
	{
		NotNullPtr<Win32::WMIQuery> db;
		const WChar *wptr = Text::StrToWCharNew(ns->v);
		NEW_CLASSNN(db, Win32::WMIQuery(wptr));
		Text::StrDelNew(wptr);
		ns->Release();
		if (db->IsError())
		{
			db.Delete();
			me->ui->ShowMsgOK(CSTR("Error in opening the namespace"), CSTR("Error"), me);
		}
		else
		{
			me->core->OpenObject(db);
			me->SetDialogResult(UI::GUIForm::DR_OK);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRWMIForm::OnCancelClick(void *userObj)
{
	SSWR::AVIRead::AVIRWMIForm *me = (SSWR::AVIRead::AVIRWMIForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRWMIForm::OnDblClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWMIForm *me = (SSWR::AVIRead::AVIRWMIForm*)userObj;
	NotNullPtr<Text::String> ns;
	if (me->lbNS->GetSelectedItemTextNew().SetTo(ns))
	{
		NotNullPtr<Win32::WMIQuery> db;
		const WChar *wptr = Text::StrToWCharNew(ns->v);
		NEW_CLASSNN(db, Win32::WMIQuery(wptr));
		Text::StrDelNew(wptr);
		ns->Release();
		if (db->IsError())
		{
			db.Delete();
			me->ui->ShowMsgOK(CSTR("Error in opening the namespace"), CSTR("Error"), me);
		}
		else
		{
			me->core->OpenObject(db);
			me->SetDialogResult(UI::GUIForm::DR_OK);
		}
	}
}

SSWR::AVIRead::AVIRWMIForm::AVIRWMIForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 456, 200, ui)
{
	this->SetText(CSTR("Select WMI Namespace"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASS(this->lblNS, UI::GUILabel(ui, *this, CSTR("WMI Namespace")));
	this->lblNS->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->lbNS, UI::GUIListBox(ui, *this, false));
	this->lbNS->SetRect(112, 8, 256, 112, false);
	this->lbNS->HandleDoubleClicked(OnDblClicked, this);

	this->btnOK = ui->NewButton(*this, CSTR("&OK"));
	this->btnOK->SetRect(112, 144, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClick, this);
	this->btnCancel = ui->NewButton(*this, CSTR("&Cancel"));
	this->btnCancel->SetRect(192, 144, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClick, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	Data::ArrayList<const WChar *> nsList;
	Win32::WMIQuery::GetNSList(&nsList);
	UOSInt i;
	UOSInt j;
	i = 0;
	j = nsList.GetCount();
	while (i < j)
	{
		this->lbNS->AddItem(nsList.GetItem(i), 0);
		i++;
	}
	Win32::WMIQuery::FreeNSList(&nsList);
}

SSWR::AVIRead::AVIRWMIForm::~AVIRWMIForm()
{
}

void SSWR::AVIRead::AVIRWMIForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
