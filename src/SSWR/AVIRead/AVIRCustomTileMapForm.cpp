#include "Stdafx.h"
#include "Crypto/Hash/CRC32RC.h"
#include "IO/Path.h"
#include "Map/CustomTileMap.h"
#include "SSWR/AVIRead/AVIRCustomTileMapForm.h"

void __stdcall SSWR::AVIRead::AVIRCustomTileMapForm::OnOKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCustomTileMapForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCustomTileMapForm>();
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Int32 minLevel;
	Int32 maxLevel;
	Text::StringBuilderUTF8 sb;
	me->txtMinLevel->GetText(sb);
	if (!sb.ToInt32(minLevel) || (minLevel < 0) || (minLevel > 18))
	{
		me->ui->ShowMsgOK(CSTR("Min Level must be between 0 to 18"), CSTR("Custom Tile Map"), me);
		return;
	}
	sb.ClearStr();
	me->txtMaxLevel->GetText(sb);
	if (!sb.ToInt32(maxLevel) || (maxLevel < 0) || (maxLevel > 22))
	{
		me->ui->ShowMsgOK(CSTR("Max Level must be between 0 to 22"), CSTR("Custom Tile Map"), me);
		return;
	}
	if (maxLevel < minLevel)
	{
		me->ui->ShowMsgOK(CSTR("Max Level must be large than or equals to Min Level"), CSTR("Custom Tile Map"), me);
		return;
	}
	sb.ClearStr();
	me->txtURL->GetText(sb);
	if (!sb.StartsWith(UTF8STRC("http://")) && !sb.StartsWith(UTF8STRC("https://")) && !sb.StartsWith(UTF8STRC("file://")))
	{
		me->ui->ShowMsgOK(CSTR("URL is not valid"), CSTR("Custom Tile Map"), me);
		return;
	}
	if (sb.IndexOf(UTF8STRC("{x}")) == INVALID_INDEX)
	{
		me->ui->ShowMsgOK(CSTR("{x} not found in URL"), CSTR("Custom Tile Map"), me);
		return;
	}
	if (sb.IndexOf(UTF8STRC("{y}")) == INVALID_INDEX)
	{
		me->ui->ShowMsgOK(CSTR("{y} not found in URL"), CSTR("Custom Tile Map"), me);
		return;
	}
	if (sb.IndexOf(UTF8STRC("{z}")) == INVALID_INDEX)
	{
		me->ui->ShowMsgOK(CSTR("{z} not found in URL"), CSTR("Custom Tile Map"), me);
		return;
	}
	Crypto::Hash::CRC32RC crc;
	sbuff[0] = 0;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	UOSInt i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	sptr = Text::StrHexVal32(&sbuff[i + 1], crc.CalcDirect(sb.v, sb.leng));
	NEW_CLASSOPT(me->tileMap, Map::CustomTileMap(sb.ToCString(), CSTRP(sbuff, sptr), (UInt32)minLevel, (UInt32)maxLevel, me->core->GetTCPClientFactory(), me->ssl));
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

SSWR::AVIRead::AVIRCustomTileMapForm::AVIRCustomTileMapForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl) : UI::GUIForm(parent, 1024, 120, ui)
{
	this->SetText(CSTR("Custom Tile Map"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->ssl = ssl;
	this->tileMap = nullptr;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblURL = ui->NewLabel(*this, CSTR("URL"));
	this->lblURL->SetRect(4, 4, 100, 23, false);
	this->txtURL = ui->NewTextBox(*this, CSTR(""));
	this->txtURL->SetRect(104, 4, 800, 23, false);
	this->lblMinLevel = ui->NewLabel(*this, CSTR("Min Level"));
	this->lblMinLevel->SetRect(4, 28, 100, 23, false);
	this->txtMinLevel = ui->NewTextBox(*this, CSTR("0"));
	this->txtMinLevel->SetRect(104, 28, 100, 23, false);
	this->lblMaxLevel = ui->NewLabel(*this, CSTR("Max Level"));
	this->lblMaxLevel->SetRect(4, 52, 100, 23, false);
	this->txtMaxLevel = ui->NewTextBox(*this, CSTR("18"));
	this->txtMaxLevel->SetRect(104, 52, 100, 23, false);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(104, 76, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
}

SSWR::AVIRead::AVIRCustomTileMapForm::~AVIRCustomTileMapForm()
{
}

void SSWR::AVIRead::AVIRCustomTileMapForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Optional<Map::TileMap> SSWR::AVIRead::AVIRCustomTileMapForm::GetTileMap()
{
	return this->tileMap;
}
