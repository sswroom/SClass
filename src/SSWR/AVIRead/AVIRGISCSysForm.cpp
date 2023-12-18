#include "Stdafx.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/GeographicCoordinateSystem.h"
#include "Math/ProjectedCoordinateSystem.h"
#include "SSWR/AVIRead/AVIRGISCSysForm.h"

void __stdcall SSWR::AVIRead::AVIRGISCSysForm::OnOKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISCSysForm *me = (SSWR::AVIRead::AVIRGISCSysForm*)userObj;
	if (me->radGeo->IsSelected())
	{
		UOSInt i = me->cboGeo->GetSelectedIndex();
		if (i != INVALID_INDEX)
		{
			Math::CoordinateSystemManager::GeoCoordSysType gcst = (Math::CoordinateSystemManager::GeoCoordSysType)(OSInt)me->cboGeo->GetItem(i);
			me->outCSys = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(gcst);
			me->SetDialogResult(UI::GUIForm::DR_OK);
		}
	}
	else if (me->radProj->IsSelected())
	{
		UOSInt i = me->cboProj->GetSelectedIndex();
		if (i != INVALID_INDEX)
		{
			const UTF8Char *projName = (const UTF8Char*)me->cboProj->GetItem(i);
			me->outCSys = Math::CoordinateSystemManager::CreateProjCoordinateSystem({projName, Text::StrCharCnt(projName)}, projName);
			if (me->outCSys)
			{
				me->SetDialogResult(UI::GUIForm::DR_OK);
			}
		}
	}
}
void __stdcall SSWR::AVIRead::AVIRGISCSysForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISCSysForm *me = (SSWR::AVIRead::AVIRGISCSysForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRGISCSysForm::OnGeoSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRGISCSysForm *me = (SSWR::AVIRead::AVIRGISCSysForm*)userObj;
	me->radGeo->Select();
}

void __stdcall SSWR::AVIRead::AVIRGISCSysForm::OnProjSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRGISCSysForm *me = (SSWR::AVIRead::AVIRGISCSysForm*)userObj;
	me->radProj->Select();
}

SSWR::AVIRead::AVIRGISCSysForm::AVIRGISCSysForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Math::CoordinateSystem *csys) : UI::GUIForm(parent, 640, 480, ui)
{
	this->SetText(CSTR("Coordinate System"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->oriCSys = csys;
	this->outCSys = 0;

	NEW_CLASS(this->txtCurrCSys, UI::GUITextBox(ui, *this, CSTR(""), true));
	this->txtCurrCSys->SetRect(0, 0, 100, 240, false);
	this->txtCurrCSys->SetDockType(UI::GUIControl::DOCK_TOP);
	this->txtCurrCSys->SetReadOnly(true);
	NEW_CLASSNN(this->pnlSel, UI::GUIPanel(ui, *this));
	this->pnlSel->SetRect(0, 0, 300, 23, false);
	this->pnlSel->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->radGeo, UI::GUIRadioButton(ui, this->pnlSel, CSTR("Geographic CS"), true));
	this->radGeo->SetRect(0, 4, 100, 23, false);
	NEW_CLASS(this->cboGeo, UI::GUIComboBox(ui, this->pnlSel, false));
	this->cboGeo->SetRect(100, 4, 200, 23, false);
	this->cboGeo->HandleSelectionChange(OnGeoSelChg, this);
	NEW_CLASS(this->radProj, UI::GUIRadioButton(ui, this->pnlSel, CSTR("Projected CS"), false));
	this->radProj->SetRect(0, 28, 100, 23, false);
	NEW_CLASS(this->cboProj, UI::GUIComboBox(ui, this->pnlSel, false));
	this->cboProj->SetRect(100, 28, 200, 23, false);
	this->cboProj->HandleSelectionChange(OnProjSelChg, this);
	this->btnOK = ui->NewButton(this->pnlSel, CSTR("OK"));
	this->btnOK->SetRect(70, 52, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->btnCancel = ui->NewButton(this->pnlSel, CSTR("Cancel"));
	this->btnCancel->SetRect(150, 52, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	NEW_CLASSNN(this->pnlInfo, UI::GUIPanel(ui, *this));
	this->pnlInfo->SetDockType(UI::GUIControl::DOCK_FILL);

	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
	UOSInt i;
	UOSInt j;
	Data::ArrayList<Math::CoordinateSystemManager::GeoCoordSysType> gcsysList;
	Data::ArrayList<const UTF8Char *> pcsysList;
	Math::CoordinateSystemManager::GetGeogCoordinateSystems(&gcsysList);
	Math::CoordinateSystemManager::GetProjCoordinateSystemNames(&pcsysList);

	Text::StringBuilderUTF8 sb;
	i = 0;
	j = gcsysList.GetCount();
	while (i < j)
	{
		this->cboGeo->AddItem(Math::CoordinateSystemManager::GeoCoordSysTypeGetName(gcsysList.GetItem(i)), (void*)(OSInt)gcsysList.GetItem(i));
		i++;
	}
	if (j > 0)
	{
		this->cboGeo->SetSelectedIndex(0);
	}
	i = 0;
	j = pcsysList.GetCount();
	while (i < j)
	{
		const UTF8Char *name = pcsysList.GetItem(i);
		this->cboProj->AddItem({name, Text::StrCharCnt(name)}, (void*)pcsysList.GetItem(i));
		i++;
	}
	if (j > 0)
	{
		this->cboProj->SetSelectedIndex(0);
	}

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Current Coordinate System:\r\n"));
	if (this->oriCSys == 0)
	{
		sb.AppendC(UTF8STRC("Unknown"));
	}
	else
	{
		this->oriCSys->ToString(sb);
	}
	this->txtCurrCSys->SetText(sb.ToCString());
}

SSWR::AVIRead::AVIRGISCSysForm::~AVIRGISCSysForm()
{
}

void SSWR::AVIRead::AVIRGISCSysForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Math::CoordinateSystem *SSWR::AVIRead::AVIRGISCSysForm::GetCSys()
{
	return this->outCSys;
}
