#include "Stdafx.h"
#include "Map/BingMapsTile.h"
#include "SSWR/AVIRead/AVIRBingMapsForm.h"

void __stdcall SSWR::AVIRead::AVIRBingMapsForm::OnOKClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRBingMapsForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBingMapsForm>();
	Text::StringBuilderUTF8 sbKey;
	Text::StringBuilderUTF8 sb;
	Map::BingMapsTile::ImagerySet is = (Map::BingMapsTile::ImagerySet)me->cboImagerySet->GetSelectedItem().GetOSInt();
	Map::BingMapsTile::GetDefaultCacheDir(is, sb);
	me->txtKey->GetText(sbKey);
	if (sbKey.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter the key"), CSTR("Bing Maps"), me);
		return;
	}
	Map::BingMapsTile *tileMap;
	NEW_CLASS(tileMap, Map::BingMapsTile(is, sbKey.ToCString(), sb.ToCString(), me->core->GetSocketFactory(), me->ssl));
	if (tileMap->IsError())
	{
		me->ui->ShowMsgOK(CSTR("Error in initializing Bing Maps"), CSTR("Bing Maps"), me);
		DEL_CLASS(tileMap);
		return;
	}
	me->tileMap = tileMap;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

SSWR::AVIRead::AVIRBingMapsForm::AVIRBingMapsForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl) : UI::GUIForm(parent, 1024, 120, ui)
{
	this->SetText(CSTR("Bing Maps"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->ssl = ssl;
	this->tileMap = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblKey = ui->NewLabel(*this, CSTR("Key"));
	this->lblKey->SetRect(4, 4, 100, 23, false);
	this->txtKey = ui->NewTextBox(*this, CSTR(""));
	this->txtKey->SetRect(104, 4, 800, 23, false);
	this->lblImagerySet = ui->NewLabel(*this, CSTR("Min Level"));
	this->lblImagerySet->SetRect(4, 28, 100, 23, false);
	this->cboImagerySet = ui->NewComboBox(*this, false);
	this->cboImagerySet->SetRect(104, 28, 200, 23, false);
	Map::BingMapsTile::ImagerySet is = (Map::BingMapsTile::ImagerySet)0;
	while (is <= Map::BingMapsTile::ImagerySet::Last)
	{
		this->cboImagerySet->AddItem(Map::BingMapsTile::ImagerySetGetName(is), (void*)(OSInt)is);
		is = (Map::BingMapsTile::ImagerySet)((OSInt)is + 1);
	}
	this->cboImagerySet->SetSelectedIndex(0);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
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
