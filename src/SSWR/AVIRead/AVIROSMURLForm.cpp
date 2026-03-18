#include "Stdafx.h"
#include "Crypto/Hash/CRC32R.h"
#include "IO/Path.h"
#include "Map/OSM/OSMTemplateTileMap.h"
#include "SSWR/AVIRead/AVIROSMURLForm.h"

void __stdcall SSWR::AVIRead::AVIROSMURLForm::OnOpenClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIROSMURLForm> me = userObj.GetNN<SSWR::AVIRead::AVIROSMURLForm>();
	UIntOS minZoom;
	UIntOS maxZoom;
	Double minLat;
	Double minLon;
	Double maxLat;
	Double maxLon;
	Text::StringBuilderUTF8 sb;
	me->txtMinZoom->GetText(sb);
	if (!sb.ToUIntOS(minZoom))
	{
		me->ui->ShowMsgOK(CSTR("Invalid min zoom"), CSTR("OSM URL"), me);
		return;
	}
	sb.ClearStr();
	me->txtMaxZoom->GetText(sb);
	if (!sb.ToUIntOS(maxZoom))
	{
		me->ui->ShowMsgOK(CSTR("Invalid max zoom"), CSTR("OSM URL"), me);
		return;
	}
	if (minZoom > maxZoom)
	{
		me->ui->ShowMsgOK(CSTR("Min zoom should be smaller than max zoom"), CSTR("OSM URL"), me);
		return;
	}
	sb.ClearStr();
	me->txtMinLat->GetText(sb);
	if (!sb.ToDouble(minLat))
	{
		me->ui->ShowMsgOK(CSTR("Invalid min lat"), CSTR("OSM URL"), me);
		return;
	}
	sb.ClearStr();
	me->txtMinLon->GetText(sb);
	if (!sb.ToDouble(minLon))
	{
		me->ui->ShowMsgOK(CSTR("Invalid min lon"), CSTR("OSM URL"), me);
		return;
	}
	sb.ClearStr();
	me->txtMaxLat->GetText(sb);
	if (!sb.ToDouble(maxLat))
	{
		me->ui->ShowMsgOK(CSTR("Invalid max lat"), CSTR("OSM URL"), me);
		return;
	}
	sb.ClearStr();
	me->txtMaxLon->GetText(sb);
	if (!sb.ToDouble(maxLon))
	{
		me->ui->ShowMsgOK(CSTR("Invalid max lon"), CSTR("OSM URL"), me);
		return;
	}
	if (minLat > maxLat)
	{
		me->ui->ShowMsgOK(CSTR("Min lat should be smaller than max lat"), CSTR("OSM URL"), me);
		return;
	}
	if (minLon > maxLon)
	{
		me->ui->ShowMsgOK(CSTR("Min lon should be smaller than max lon"), CSTR("OSM URL"), me);
		return;
	}
	sb.ClearStr();
	me->txtURL->GetText(sb);
	if (sb.leng == 0)
	{
		me->ui->ShowMsgOK(CSTR("URL cannot be empty"), CSTR("OSM URL"), me);
		return;
	}
	if (sb.IndexOf(CSTR("{z}")) == INVALID_INDEX || sb.IndexOf(CSTR("{x}")) == INVALID_INDEX || sb.IndexOf(CSTR("{y}")) == INVALID_INDEX)
	{
		me->ui->ShowMsgOK(CSTR("URL should contain {z}, {x} and {y}"), CSTR("OSM URL"), me);
		return;
	}
	me->tileMap.Delete();
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMTile_"));
	Crypto::Hash::CRC32R crc;
	sptr = Text::StrHexVal32(sptr, crc.CalcDirect(sb.v, sb.leng));
	NEW_CLASSOPT(me->tileMap, Map::OSM::OSMTemplateTileMap(sb.ToCString(), CSTRP(sbuff, sptr), minZoom, maxZoom, me->clif, me->ssl));
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

SSWR::AVIRead::AVIROSMURLForm::AVIROSMURLForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("OSM URL"));
	this->SetFont(nullptr, 8.25, false);

	this->core = core;
	this->tileMap = nullptr;
	this->clif = clif;
	this->ssl = ssl;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblURL = ui->NewLabel(*this, CSTR("URL"));
	this->lblURL->SetRect(4, 4, 100, 23, false);
	this->txtURL = ui->NewTextBox(*this, CSTR(""));
	this->txtURL->SetRect(104, 4, 400, 23, false);
	this->lblMinZoom = ui->NewLabel(*this, CSTR("Min Zoom"));
	this->lblMinZoom->SetRect(4, 28, 100, 23, false);
	this->txtMinZoom = ui->NewTextBox(*this, CSTR("0"));
	this->txtMinZoom->SetRect(104, 28, 150, 23, false);
	this->lblMaxZoom = ui->NewLabel(*this, CSTR("Max Zoom"));
	this->lblMaxZoom->SetRect(4, 52, 100, 23, false);
	this->txtMaxZoom = ui->NewTextBox(*this, CSTR("18"));
	this->txtMaxZoom->SetRect(104, 52, 150, 23, false);
	this->lblMinLon = ui->NewLabel(*this, CSTR("Min Lon"));
	this->lblMinLon->SetRect(4, 76, 100, 23, false);
	this->txtMinLon = ui->NewTextBox(*this, CSTR("-180"));
	this->txtMinLon->SetRect(104, 76, 150, 23, false);
	this->lblMinLat = ui->NewLabel(*this, CSTR("Min Lat"));
	this->lblMinLat->SetRect(4, 100, 100, 23, false);
	this->txtMinLat = ui->NewTextBox(*this, CSTR("-85.05112878"));
	this->txtMinLat->SetRect(104, 100, 150, 23, false);
	this->lblMaxLon = ui->NewLabel(*this, CSTR("Max Lon"));
	this->lblMaxLon->SetRect(4, 124, 100, 23, false);
	this->txtMaxLon = ui->NewTextBox(*this, CSTR("180"));
	this->txtMaxLon->SetRect(104, 124, 150, 23, false);
	this->lblMaxLat = ui->NewLabel(*this, CSTR("Max Lat"));
	this->lblMaxLat->SetRect(4, 148, 100, 23, false);
	this->txtMaxLat = ui->NewTextBox(*this, CSTR("85.05112878"));
	this->txtMaxLat->SetRect(104, 148, 150, 23, false);
	this->btnOpen = ui->NewButton(*this, CSTR("Open"));
	this->btnOpen->SetRect(104, 172, 100, 23, false);
	this->btnOpen->HandleButtonClick(OnOpenClicked, this);
}

SSWR::AVIRead::AVIROSMURLForm::~AVIROSMURLForm()
{
}

void SSWR::AVIRead::AVIROSMURLForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Optional<Map::TileMap> SSWR::AVIRead::AVIROSMURLForm::GetTileMap()
{
	return this->tileMap;
}
