#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRLoRaJSONForm.h"
#include "Text/JSON.h"
#include "Text/TextBinEnc/Base64Enc.h"

void __stdcall SSWR::AVIRead::AVIRLoRaJSONForm::OnJSONParseClick(void *userObj)
{
	SSWR::AVIRead::AVIRLoRaJSONForm *me = (SSWR::AVIRead::AVIRLoRaJSONForm*)userObj;
	UInt8 buff[256];
	UOSInt buffSize;
	Text::StringBuilderUTF8 sb;
	me->txtJSON->GetText(&sb);
	Text::JSONBase *json = Text::JSONBase::ParseJSONStr(sb.ToCString());
	if (json)
	{
		Text::String *rxdata = json->GetValueString(CSTR("rxpk[0].data"));
		Text::JSONBase *rxstat = json->GetValue(CSTR("rxpk[0].stat"));
		Text::String *txdata = json->GetValueString(CSTR("txpk.data"));
		if (rxdata)
		{
			Text::TextBinEnc::Base64Enc b64;
			buffSize = b64.DecodeBin(rxdata->v, rxdata->leng, buff);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Received Packet:\r\n"));
			sb.AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::CRLF);
			sb.AppendC(UTF8STRC("\r\n\r\n"));
			if (rxstat == 0)
			{
				sb.AppendC(UTF8STRC("CRC state unknown"));
			}
			else if (rxstat->GetAsInt32() == 1)
			{
				sb.AppendC(UTF8STRC("CRC valid\r\n"));
				PHYPayloadDetail(&sb, buff, buffSize);
			}
			else
			{
				sb.AppendC(UTF8STRC("CRC invalid"));
			}
			me->txtInfo->SetText(sb.ToCString());
		}
		else if (txdata)
		{
			Text::TextBinEnc::Base64Enc b64;
			buffSize = b64.DecodeBin(txdata->v, txdata->leng, buff);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Transmitted Packet:\r\n"));
			sb.AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::CRLF);
			sb.AppendC(UTF8STRC("\r\n\r\n"));
			PHYPayloadDetail(&sb, buff, buffSize);
			me->txtInfo->SetText(sb.ToCString());
		}
		else
		{
			me->txtInfo->SetText(CSTR("Data not found"));
		}
		json->EndUse();
	}
	else
	{
		me->txtInfo->SetText(CSTR("Not JSON String"));
	}
}

void SSWR::AVIRead::AVIRLoRaJSONForm::PHYPayloadDetail(Text::StringBuilderUTF8 *sb, const UInt8 *buff, UOSInt buffSize)
{
	switch (buff[0] >> 5)
	{
	case 0:
		sb->AppendC(UTF8STRC("Message Type = Join Request\r\n"));
		break;
	case 1:
		sb->AppendC(UTF8STRC("Message Type = Join Accept\r\n"));
		break;
	case 2:
		sb->AppendC(UTF8STRC("Message Type = Unconfirmed Data Up\r\n"));
		MACPayloadDetail(sb, false, buff + 1, buffSize - 5);
		break;
	case 3:
		sb->AppendC(UTF8STRC("Message Type = Unconfirmed Data Down\r\n"));
		MACPayloadDetail(sb, true, buff + 1, buffSize - 5);
		break;
	case 4:
		sb->AppendC(UTF8STRC("Message Type = Confirmed Data Up\r\n"));
		MACPayloadDetail(sb, false, buff + 1, buffSize - 5);
		break;
	case 5:
		sb->AppendC(UTF8STRC("Message Type = Confirmed Data Down\r\n"));
		MACPayloadDetail(sb, true, buff + 1, buffSize - 5);
		break;
	case 6:
		sb->AppendC(UTF8STRC("Message Type = RFU\r\n"));
		break;
	case 7:
		sb->AppendC(UTF8STRC("Message Type = Propriety\r\n"));
		break;
	}
	sb->AppendC(UTF8STRC("MIC = 0x"));
	sb->AppendHexBuff(&buff[buffSize - 4], 4, 0, Text::LineBreakType::None);
	sb->AppendC(UTF8STRC("\r\n"));
}

void SSWR::AVIRead::AVIRLoRaJSONForm::MACPayloadDetail(Text::StringBuilderUTF8 *sb, Bool donwLink, const UInt8 *buff, UOSInt buffSize)
{
	if (buffSize < 7)
	{
		return;
	}
	sb->AppendC(UTF8STRC("DevAddr = 0x"));
	sb->AppendHex32(ReadUInt32(buff));
	sb->AppendC(UTF8STRC("\r\nADR = "));
	sb->AppendUOSInt(((UOSInt)buff[4] & 0x80) >> 7);
	sb->AppendC(UTF8STRC("\r\nACK = "));
	sb->AppendUOSInt(((UOSInt)buff[4] & 0x20) >> 5);
	if (donwLink)
	{
		sb->AppendC(UTF8STRC("\r\nRFU = "));
		sb->AppendUOSInt(((UOSInt)buff[4] & 0x40) >> 6);
		sb->AppendC(UTF8STRC("\r\nFPending = "));
		sb->AppendUOSInt(((UOSInt)buff[4] & 0x10) >> 4);
	}
	else
	{
		sb->AppendC(UTF8STRC("\r\nADRACKReq = "));
		sb->AppendUOSInt(((UOSInt)buff[4] & 0x40) >> 6);
		sb->AppendC(UTF8STRC("\r\nClassB = "));
		sb->AppendUOSInt(((UOSInt)buff[4] & 0x10) >> 4);
	}
	UOSInt fOptsLen = (UOSInt)buff[4] & 0xF;
	sb->AppendC(UTF8STRC("\r\nFOptsLen = "));
	sb->AppendUOSInt(fOptsLen);
	sb->AppendC(UTF8STRC("\r\nFCnt = "));
	sb->AppendU16(ReadUInt16(&buff[5]));
	if (fOptsLen + 7 > buffSize)
	{
		sb->AppendC(UTF8STRC("\r\n"));
		return;
	}
	if (fOptsLen > 0)
	{
		sb->AppendC(UTF8STRC("\r\nFOpts = "));
		sb->AppendHexBuff(buff + 7, fOptsLen, ' ', Text::LineBreakType::None);
	}
	sb->AppendC(UTF8STRC("\r\n"));
	buff += fOptsLen + 7;
	buffSize -= fOptsLen + 7;
	if (buffSize == 0)
	{
		return;
	}
	sb->AppendC(UTF8STRC("FPort = "));
	sb->AppendU16(buff[0]);
	if (buffSize > 1)
	{
		sb->AppendC(UTF8STRC("\r\nFRMPayload = "));
		sb->AppendHexBuff(buff + 1, buffSize - 1, ' ', Text::LineBreakType::None);
	}
	sb->AppendC(UTF8STRC("\r\n"));
}

SSWR::AVIRead::AVIRLoRaJSONForm::AVIRLoRaJSONForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("LoRa JSON Parser"));
	this->SetFont(0, 0, 8.25, false);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlJSON, UI::GUIPanel(ui, this));
	this->pnlJSON->SetRect(0, 0, 100, 103, false);
	this->pnlJSON->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->pnlJSONCtrl, UI::GUIPanel(ui, this->pnlJSON));
	this->pnlJSONCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlJSONCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lblJSON, UI::GUILabel(ui, this->pnlJSON, CSTR("JSON")));
	this->lblJSON->SetRect(0, 0, 100, 23, false);
	this->lblJSON->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->txtJSON, UI::GUITextBox(ui, this->pnlJSON, CSTR(""), true));
	this->txtJSON->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->btnJSONParse, UI::GUIButton(ui, this->pnlJSONCtrl, CSTR("Parse")));
	this->btnJSONParse->SetRect(104, 4, 75, 23, false);
	this->btnJSONParse->HandleButtonClick(OnJSONParseClick, this);
	NEW_CLASS(this->lblInfo, UI::GUILabel(ui, this, CSTR("Info")));
	this->lblInfo->SetRect(0, 0, 100, 23, false);
	this->lblInfo->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->txtInfo, UI::GUITextBox(ui, this, CSTR(""), true));
	this->txtInfo->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtInfo->SetReadOnly(true);
}

SSWR::AVIRead::AVIRLoRaJSONForm::~AVIRLoRaJSONForm()
{
}

void SSWR::AVIRead::AVIRLoRaJSONForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
