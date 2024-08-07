#include "Stdafx.h"
#include "SSWR/SHPConv/SHPConvAddFilterForm.h"
#include "SSWR/SHPConv/SHPConvElevationFilterForm.h"
#include "SSWR/SHPConv/SHPConvRangeFilterForm.h"
#include "SSWR/SHPConv/SHPConvValueFilterForm.h"

void __stdcall SSWR::SHPConv::SHPConvAddFilterForm::OnOKClicked(AnyType userObj)
{
	NN<SSWR::SHPConv::SHPConvAddFilterForm> me = userObj.GetNN<SSWR::SHPConv::SHPConvAddFilterForm>();
	UOSInt i = me->cboFilter->GetSelectedIndex();
	if (i == 0)
	{
		SSWR::SHPConv::SHPConvValueFilterForm frm(0, me->ui, me->dbf);
		if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			me->filter = frm.GetFilter();
			me->SetDialogResult(UI::GUIForm::DR_OK);
		}
	}
	else if (i == 1)
	{
		SSWR::SHPConv::SHPConvRangeFilterForm frm(0, me->ui, me->deng);
		if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			me->filter = frm.GetFilter();
			me->SetDialogResult(UI::GUIForm::DR_OK);
		}
	}
	else if (i == 2)
	{
		SSWR::SHPConv::SHPConvElevationFilterForm frm(0, me->ui, me->dbf);
		if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			me->filter = frm.GetFilter();
			me->SetDialogResult(UI::GUIForm::DR_OK);
		}
	}
}

void __stdcall SSWR::SHPConv::SHPConvAddFilterForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::SHPConv::SHPConvAddFilterForm> me = userObj.GetNN<SSWR::SHPConv::SHPConvAddFilterForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::SHPConv::SHPConvAddFilterForm::SHPConvAddFilterForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<DB::DBFFile> dbf, NN<Media::DrawEngine> deng) : UI::GUIForm(parent, 222, 118, ui)
{
	this->SetText(CSTR("Add Filter"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	NEW_CLASSNN(this->monMgr, Media::MonitorMgr());
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
	this->deng = deng;
	this->dbf = dbf;
	this->filter = 0;
	
	this->lblFilter = ui->NewLabel(*this, CSTR("Filter Type"));
	this->lblFilter->SetRect(8, 16, 64, 23, false);
	this->cboFilter = ui->NewComboBox(*this, false);
	this->cboFilter->SetRect(80, 16, 121, 21, false);
	this->cboFilter->AddItem(CSTR("Value Filter"), 0);
	this->cboFilter->AddItem(CSTR("Range Filter"), 0);
	this->cboFilter->AddItem(CSTR("Elevation Filter"), 0);
	this->cboFilter->SetSelectedIndex(0);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(8, 56, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(120, 56, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
}

SSWR::SHPConv::SHPConvAddFilterForm::~SHPConvAddFilterForm()
{
	this->monMgr.Delete();
}

void SSWR::SHPConv::SHPConvAddFilterForm::OnMonitorChanged()
{
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
}

Optional<SSWR::SHPConv::MapFilter> SSWR::SHPConv::SHPConvAddFilterForm::GetFilter()
{
	return this->filter;
}
