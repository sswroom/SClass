#include "Stdafx.h"
#include "Map/BingMapsTile.h"
#include "SSWR/AVIRead/AVIRBingMapsForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRBingMapsForm::OnOKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRBingMapsForm *me = (SSWR::AVIRead::AVIRBingMapsForm*)userObj;
	Text::StringBuilderUTF8 sbKey;
	Text::StringBuilderUTF8 sb;
	Map::BingMapsTile::ImagerySet is = (Map::BingMapsTile::ImagerySet)(OSInt)me->cboImagerySet->GetSelectedItem();
	Map::BingMapsTile::GetDefaultCacheDir(is, &sb);
	me->txtKey->GetText(&sbKey);
	if (sbKey.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please enter the key"), CSTR("Bing Maps"), me);
		return;
	}
	Map::BingMapsTile *tileMap;
	NEW_CLASS(tileMap, Map::BingMapsTile(is, sbKey.ToCString(), sb.ToCString(), me->core->GetSocketFactory(), me->ssl));
	if (tileMap->IsError())
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in initializing Bing Maps"), CSTR("Bing Maps"), me);
		DEL_CLASS(tileMap);
		return;
	}
	me->tileMap = tileMap;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

SSWR::AVIRead::AVIRBingMapsForm::AVIRBingMapsForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core, Net::SSLEngine *ssl) : UI::GUIForm(parent, 1024, 120, ui)
{
	this->SetText(CSTR("Bing Maps"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->ssl = ssl;
	this->tileMap = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblKey, UI::GUILabel(ui, this, CSTR("Key")));
	this->lblKey->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtKey, UI::GUITextBox(ui, this, CSTR("")));
	this->txtKey->SetRect(104, 4, 800, 23, false);
	NEW_CLASS(this->lblImagerySet, UI::GUILabel(ui, this, CSTR("Min Level")));
	this->lblImagerySet->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->cboImagerySet, UI::GUIComboBox(ui, this, false));
	this->cboImagerySet->SetRect(104, 28, 200, 23, false);
	Map::BingMapsTile::ImagerySet is = (Map::BingMapsTile::ImagerySet)0;
	while (is <= Map::BingMapsTile::ImagerySet::Last)
	{
		this->cboImagerySet->AddItem(Map::BingMapsTile::ImagerySetGetName(is), (void*)(OSInt)is);
		is = (Map::BingMapsTile::ImagerySet)((OSInt)is + 1);
	}
	this->cboImagerySet->SetSelectedIndex(0);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, CSTR("OK")));
	this->btnOK->SetRect(104, 76, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
}

SSWR::AVIRead::AVIRBingMapsForm::~AVIRBingMapsForm()
{
}

void SSWR::AVIRead::AVIRBingMapsForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Map::TileMap *SSWR::AVIRead::AVIRBingMapsForm::GetTileMap()
{
	return this->tileMap;
}
