#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "SSWR/AVIRead/AVIRWMTSForm.h"

void __stdcall SSWR::AVIRead::AVIRWMTSForm::OnLoadClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWMTSForm *me = (SSWR::AVIRead::AVIRWMTSForm *)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtWMTSURL->GetText(&sb);
	SDEL_CLASS(me->wmts);
	NEW_CLASS(me->wmts, Map::WebMapTileServiceSource(me->core->GetSocketFactory(), me->core->GetEncFactory(), sb.ToCString()));
	if (me->wmts->IsError())
	{
		me->txtStatus->SetText(CSTR("Error"));
	}
	else
	{
		me->txtStatus->SetText(CSTR("Success"));
		Data::ArrayList<Text::String*> nameList;
		UOSInt i = 0;
		UOSInt j = me->wmts->GetLayerNames(&nameList);
		me->cboLayer->ClearItems();
		while (i < j)
		{
			me->cboLayer->AddItem(nameList.GetItem(i), 0);
			i++;
		}
		if (j > 0)
		{
			me->cboLayer->SetSelectedIndex(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRWMTSForm::OnOKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWMTSForm *me = (SSWR::AVIRead::AVIRWMTSForm *)userObj;
	if (me->wmts && !me->wmts->IsError())
	{
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
}

void __stdcall SSWR::AVIRead::AVIRWMTSForm::OnLayerSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRWMTSForm *me = (SSWR::AVIRead::AVIRWMTSForm *)userObj;
	if (me->wmts && !me->wmts->IsError())
	{
		me->wmts->SetLayer(me->cboLayer->GetSelectedIndex());
		Data::ArrayList<Text::String*> nameList;
		UOSInt i = 0;
		UOSInt j = me->wmts->GetMatrixSetNames(&nameList);
		me->cboMatrixSet->ClearItems();
		while (i < j)
		{
			me->cboMatrixSet->AddItem(nameList.GetItem(i), 0);
			i++;
		}
		if (j > 0)
		{
			me->cboMatrixSet->SetSelectedIndex(0);
		}

		nameList.Clear();
		i = 0;
		j = me->wmts->GetResourceTypeNames(&nameList);
		me->cboResourceType->ClearItems();
		while (i < j)
		{
			me->cboResourceType->AddItem(nameList.GetItem(i), 0);
			i++;
		}
		if (j > 0)
		{
			me->cboResourceType->SetSelectedIndex(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRWMTSForm::OnMatrixSetSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRWMTSForm *me = (SSWR::AVIRead::AVIRWMTSForm *)userObj;
	if (me->wmts && !me->wmts->IsError())
	{
		me->wmts->SetMatrixSet(me->cboMatrixSet->GetSelectedIndex());
	}
}

void __stdcall SSWR::AVIRead::AVIRWMTSForm::OnResourceTypeSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRWMTSForm *me = (SSWR::AVIRead::AVIRWMTSForm *)userObj;
	if (me->wmts && !me->wmts->IsError())
	{
		me->wmts->SetResourceType(me->cboResourceType->GetSelectedIndex());
	}
}

SSWR::AVIRead::AVIRWMTSForm::AVIRWMTSForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 640, 240, ui)
{
	this->core = core;
	this->wmts = 0;
	this->SetText(CSTR("Web Map Tile Service"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblWMTSURL, UI::GUILabel(ui, this, CSTR("WMTS URL")));
	this->lblWMTSURL->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtWMTSURL, UI::GUITextBox(ui, this, CSTR("")));
	this->txtWMTSURL->SetRect(104, 4, 500, 23, false);
	NEW_CLASS(this->btnLoad, UI::GUIButton(ui, this, CSTR("Load")));
	this->btnLoad->SetRect(104, 28, 75, 23, false);
	this->btnLoad->HandleButtonClick(OnLoadClicked, this);
	NEW_CLASS(this->lblStatus, UI::GUILabel(ui, this, CSTR("Status")));
	this->lblStatus->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtStatus, UI::GUITextBox(ui, this, CSTR("")));
	this->txtStatus->SetRect(104, 52, 100, 23, false);
	this->txtStatus->SetReadOnly(true);
	NEW_CLASS(this->lblLayer, UI::GUILabel(ui, this, CSTR("Layer")));
	this->lblLayer->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->cboLayer, UI::GUIComboBox(ui, this, false));
	this->cboLayer->SetRect(104, 76, 200, 23, false);
	this->cboLayer->HandleSelectionChange(OnLayerSelChg, this);
	NEW_CLASS(this->lblMatrixSet, UI::GUILabel(ui, this, CSTR("Matrix Set")));
	this->lblMatrixSet->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->cboMatrixSet, UI::GUIComboBox(ui, this, false));
	this->cboMatrixSet->SetRect(104, 100, 200, 23, false);
	this->cboMatrixSet->HandleSelectionChange(OnMatrixSetSelChg, this);
	NEW_CLASS(this->lblResourceType, UI::GUILabel(ui, this, CSTR("Resource Type")));
	this->lblResourceType->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->cboResourceType, UI::GUIComboBox(ui, this, false));
	this->cboResourceType->SetRect(104, 124, 200, 23, false);
	this->cboResourceType->HandleSelectionChange(OnResourceTypeSelChg, this);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, CSTR("OK")));
	this->btnOK->SetRect(104, 148, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
}

SSWR::AVIRead::AVIRWMTSForm::~AVIRWMTSForm()
{
	SDEL_CLASS(this->wmts);
}

void SSWR::AVIRead::AVIRWMTSForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Map::TileMap *SSWR::AVIRead::AVIRWMTSForm::GetTileMap()
{
	Map::TileMap *tile = this->wmts;
	this->wmts = 0;
	return tile;
}
