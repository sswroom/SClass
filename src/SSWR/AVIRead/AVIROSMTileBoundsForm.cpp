#include "Stdafx.h"
#include "Map/MercatorTileMap.h"
#include "SSWR/AVIRead/AVIROSMTileBoundsForm.h"

void __stdcall SSWR::AVIRead::AVIROSMTileBoundsForm::OnCalcClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIROSMTileBoundsForm> me = userObj.GetNN<SSWR::AVIRead::AVIROSMTileBoundsForm>();
	Text::StringBuilderUTF8 sb;
	UInt32 level;
	Int32 x;
	Int32 y;
	me->txtLevel->GetText(sb);
	if (!sb.ToUInt32(level))
	{
		return;
	}
	if (level > 22)
	{
		return;
	}
	sb.ClearStr();
	me->txtX->GetText(sb);
	if (!sb.ToInt32(x))
	{
		return;
	}
	sb.ClearStr();
	me->txtY->GetText(sb);
	if (!sb.ToInt32(y))
	{
		return;
	}
	Double minX = Map::MercatorTileMap::TileX2Lon(x, level);
	Double minY = Map::MercatorTileMap::TileY2Lat(y + 1, level);
	Double maxX = Map::MercatorTileMap::TileX2Lon(x + 1, level);
	Double maxY = Map::MercatorTileMap::TileY2Lat(y, level);
	sb.ClearStr();
	sb.AppendDouble(minX);
	me->txtMinX->SetText(sb.ToCString());
	sb.ClearStr();
	sb.AppendDouble(minY);
	me->txtMinY->SetText(sb.ToCString());
	sb.ClearStr();
	sb.AppendDouble(maxX);
	me->txtMaxX->SetText(sb.ToCString());
	sb.ClearStr();
	sb.AppendDouble(maxY);
	me->txtMaxY->SetText(sb.ToCString());
}

SSWR::AVIRead::AVIROSMTileBoundsForm::AVIROSMTileBoundsForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("OSM Tile Bounds"));
	this->SetFont(nullptr, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblLevel = ui->NewLabel(*this, CSTR("Level"));
	this->lblLevel->SetRect(4, 4, 100, 23, false);
	this->txtLevel = ui->NewTextBox(*this, CSTR("18"));
	this->txtLevel->SetRect(104, 4, 150, 23, false);
	this->lblX = ui->NewLabel(*this, CSTR("X"));
	this->lblX->SetRect(4, 28, 100, 23, false);
	this->txtX = ui->NewTextBox(*this, CSTR("0"));
	this->txtX->SetRect(104, 28, 150, 23, false);
	this->lblY = ui->NewLabel(*this, CSTR("Y"));
	this->lblY->SetRect(4, 52, 100, 23, false);
	this->txtY = ui->NewTextBox(*this, CSTR("0"));
	this->txtY->SetRect(104, 52, 150, 23, false);
	this->btnCalc = ui->NewButton(*this, CSTR("Calculate"));
	this->btnCalc->SetRect(104, 76, 100, 23, false);
	this->btnCalc->HandleButtonClick(OnCalcClicked, this);
	this->lblMinX = ui->NewLabel(*this, CSTR("Min X"));
	this->lblMinX->SetRect(4, 100, 100, 23, false);
	this->txtMinX = ui->NewTextBox(*this, CSTR(""));
	this->txtMinX->SetRect(104, 100, 150, 23, false);
	this->txtMinX->SetReadOnly(true);
	this->lblMinY = ui->NewLabel(*this, CSTR("Min Y"));
	this->lblMinY->SetRect(4, 124, 100, 23, false);
	this->txtMinY = ui->NewTextBox(*this, CSTR(""));
	this->txtMinY->SetRect(104, 124, 150, 23, false);
	this->txtMinY->SetReadOnly(true);
	this->lblMaxX = ui->NewLabel(*this, CSTR("Max X"));
	this->lblMaxX->SetRect(4, 148, 100, 23, false);
	this->txtMaxX = ui->NewTextBox(*this, CSTR(""));
	this->txtMaxX->SetRect(104, 148, 150, 23, false);
	this->txtMaxX->SetReadOnly(true);
	this->lblMaxY = ui->NewLabel(*this, CSTR("Max Y"));
	this->lblMaxY->SetRect(4, 172, 100, 23, false);
	this->txtMaxY = ui->NewTextBox(*this, CSTR(""));
	this->txtMaxY->SetRect(104, 172, 150, 23, false);
	this->txtMaxY->SetReadOnly(true);
}

SSWR::AVIRead::AVIROSMTileBoundsForm::~AVIROSMTileBoundsForm()
{
}

void SSWR::AVIRead::AVIROSMTileBoundsForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
