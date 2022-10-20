#include "Stdafx.h"
#include "Map/BaseMapLayer.h"
#include "SSWR/AVIRead/AVIRGoogleTileMapForm.h"

void __stdcall SSWR::AVIRead::AVIRGoogleTileMapForm::OnOKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGoogleTileMapForm *me = (SSWR::AVIRead::AVIRGoogleTileMapForm*)userObj;
	switch (me->cboMapType->GetSelectedIndex())
	{
	case 0:
		me->layer = Map::BaseMapLayer::CreateLayer(Map::BaseMapLayer::BLT_GMAP_MAP, me->core->GetSocketFactory(), me->ssl, me->core->GetParserList());
		break;
	case 1:
		me->layer = Map::BaseMapLayer::CreateLayer(Map::BaseMapLayer::BLT_GMAP_TRAIN, me->core->GetSocketFactory(), me->ssl, me->core->GetParserList());
		break;
	case 2:
		me->layer = Map::BaseMapLayer::CreateLayer(Map::BaseMapLayer::BLT_GMAP_SATELITE, me->core->GetSocketFactory(), me->ssl, me->core->GetParserList());
		break;
	case 3:
		me->layer = Map::BaseMapLayer::CreateLayer(Map::BaseMapLayer::BLT_GMAP_HYBRID, me->core->GetSocketFactory(), me->ssl, me->core->GetParserList());
		break;
	}
	if (me->layer)
	{
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
}

SSWR::AVIRead::AVIRGoogleTileMapForm::AVIRGoogleTileMapForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Net::SSLEngine *ssl) : UI::GUIForm(parent, 400, 120, ui)
{
	this->SetText(CSTR("Google Tile Map"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->ssl = ssl;
	this->layer = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblMapType, UI::GUILabel(ui, this, CSTR("Map Type")));
	this->lblMapType->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboMapType, UI::GUIComboBox(ui, this, false));
	this->cboMapType->SetRect(104, 4, 200, 23, false);
	this->cboMapType->AddItem(CSTR("Map"), 0);
	this->cboMapType->AddItem(CSTR("Terrain"), 0);
	this->cboMapType->AddItem(CSTR("Satelite"), 0);
	this->cboMapType->AddItem(CSTR("Hybrid"), 0);
	this->cboMapType->SetSelectedIndex(0);
	NEW_CLASS(this->txtDeclare, UI::GUITextBox(ui, this, CSTR("Declaimer: There may be Legal issue in using Google Tile Map. Please use for Research purpose only."), true));
	this->txtDeclare->SetRect(4, 28, 350, 47, false);
	this->txtDeclare->SetReadOnly(true);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, CSTR("OK")));
	this->btnOK->SetRect(104, 76, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
}

SSWR::AVIRead::AVIRGoogleTileMapForm::~AVIRGoogleTileMapForm()
{
}

void SSWR::AVIRead::AVIRGoogleTileMapForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Map::IMapDrawLayer *SSWR::AVIRead::AVIRGoogleTileMapForm::GetMapLayer()
{
	return this->layer;
}
