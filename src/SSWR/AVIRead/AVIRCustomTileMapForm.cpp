#include "Stdafx.h"
#include "Crypto/Hash/CRC32RC.h"
#include "IO/Path.h"
#include "Map/CustomTileMap.h"
#include "SSWR/AVIRead/AVIRCustomTileMapForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRCustomTileMapForm::OnOKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCustomTileMapForm *me = (SSWR::AVIRead::AVIRCustomTileMapForm*)userObj;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Int32 minLevel;
	Int32 maxLevel;
	Text::StringBuilderUTF8 sb;
	me->txtMinLevel->GetText(&sb);
	if (!sb.ToInt32(&minLevel) || (minLevel < 0) || (minLevel > 18))
	{
		UI::MessageDialog::ShowDialog(CSTR("Min Level must be between 0 to 18"), CSTR("Custom Tile Map"), me);
		return;
	}
	sb.ClearStr();
	me->txtMaxLevel->GetText(&sb);
	if (!sb.ToInt32(&maxLevel) || (maxLevel < 0) || (maxLevel > 22))
	{
		UI::MessageDialog::ShowDialog(CSTR("Max Level must be between 0 to 22"), CSTR("Custom Tile Map"), me);
		return;
	}
	if (maxLevel < minLevel)
	{
		UI::MessageDialog::ShowDialog(CSTR("Max Level must be large than or equals to Min Level"), CSTR("Custom Tile Map"), me);
		return;
	}
	sb.ClearStr();
	me->txtURL->GetText(&sb);
	if (!sb.StartsWith(UTF8STRC("http://")) && !sb.StartsWith(UTF8STRC("https://")) && !sb.StartsWith(UTF8STRC("file://")))
	{
		UI::MessageDialog::ShowDialog(CSTR("URL is not valid"), CSTR("Custom Tile Map"), me);
		return;
	}
	if (sb.IndexOf(UTF8STRC("{x}")) == INVALID_INDEX)
	{
		UI::MessageDialog::ShowDialog(CSTR("{x} not found in URL"), CSTR("Custom Tile Map"), me);
		return;
	}
	if (sb.IndexOf(UTF8STRC("{y}")) == INVALID_INDEX)
	{
		UI::MessageDialog::ShowDialog(CSTR("{y} not found in URL"), CSTR("Custom Tile Map"), me);
		return;
	}
	if (sb.IndexOf(UTF8STRC("{z}")) == INVALID_INDEX)
	{
		UI::MessageDialog::ShowDialog(CSTR("{z} not found in URL"), CSTR("Custom Tile Map"), me);
		return;
	}
	Crypto::Hash::CRC32RC crc;
	sptr = IO::Path::GetProcessFileName(sbuff);
	UOSInt i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	sptr = Text::StrHexVal32(&sbuff[i + 1], crc.CalcDirect(sb.v, sb.leng));
	NEW_CLASS(me->tileMap, Map::CustomTileMap(sb.ToCString(), CSTRP(sbuff, sptr), (UInt32)minLevel, (UInt32)maxLevel, me->core->GetSocketFactory(), me->ssl));
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

SSWR::AVIRead::AVIRCustomTileMapForm::AVIRCustomTileMapForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core, Net::SSLEngine *ssl) : UI::GUIForm(parent, 1024, 120, ui)
{
	this->SetText(CSTR("Custom Tile Map"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->ssl = ssl;
	this->tileMap = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblURL, UI::GUILabel(ui, this, CSTR("URL")));
	this->lblURL->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtURL, UI::GUITextBox(ui, this, CSTR("")));
	this->txtURL->SetRect(104, 4, 800, 23, false);
	NEW_CLASS(this->lblMinLevel, UI::GUILabel(ui, this, CSTR("Min Level")));
	this->lblMinLevel->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtMinLevel, UI::GUITextBox(ui, this, CSTR("0")));
	this->txtMinLevel->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->lblMaxLevel, UI::GUILabel(ui, this, CSTR("Max Level")));
	this->lblMaxLevel->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtMaxLevel, UI::GUITextBox(ui, this, CSTR("18")));
	this->txtMaxLevel->SetRect(104, 52, 100, 23, false);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, CSTR("OK")));
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

Map::TileMap *SSWR::AVIRead::AVIRCustomTileMapForm::GetTileMap()
{
	return this->tileMap;
}
