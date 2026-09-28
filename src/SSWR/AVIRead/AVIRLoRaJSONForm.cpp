#include "Stdafx.h"
#include "DB/CSVFile.h"
#include "Net/LoRaGWUtil.h"
#include "SSWR/AVIRead/AVIRLoRaJSONForm.h"
#include "Text/JSON.h"
#include "Text/TextBinEnc/Base64Enc.h"

void __stdcall SSWR::AVIRead::AVIRLoRaJSONForm::OnJSONParseClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLoRaJSONForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLoRaJSONForm>();
	UInt8 buff[256];
	UIntOS buffSize;
	Text::StringBuilderUTF8 sb;
	me->txtJSON->GetText(sb);
	NN<Text::JSONBase> json;
	if (Text::JSONBase::ParseJSONStr(sb.ToCString()).SetTo(json))
	{
		NN<Text::String> rxdata;
		NN<Text::JSONBase> rxstat;
		NN<Text::String> txdata;
		if (json->GetValueString(CSTR("rxpk[0].data")).SetTo(rxdata))
		{
			Text::TextBinEnc::Base64Enc b64;
			buffSize = b64.DecodeBin(rxdata->ToCString(), buff);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Received Packet:\r\n"));
			sb.AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::CRLF);
			sb.AppendC(UTF8STRC("\r\n\r\n"));
			if (!json->GetValue(CSTR("rxpk[0].stat")).SetTo(rxstat))
			{
				sb.AppendC(UTF8STRC("CRC state unknown"));
			}
			else if (rxstat->GetAsInt32() == 1)
			{
				sb.AppendC(UTF8STRC("CRC valid\r\n"));
				Net::LoRaGWUtil::PHYPayloadDetail(sb, buff, buffSize, me->devMap);
			}
			else
			{
				sb.AppendC(UTF8STRC("CRC invalid"));
			}
			me->txtInfo->SetText(sb.ToCString());
		}
		else if (json->GetValueString(CSTR("txpk.data")).SetTo(txdata))
		{
			Text::TextBinEnc::Base64Enc b64;
			buffSize = b64.DecodeBin(txdata->ToCString(), buff);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Transmitted Packet:\r\n"));
			sb.AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::CRLF);
			sb.AppendC(UTF8STRC("\r\n\r\n"));
			Net::LoRaGWUtil::PHYPayloadDetail(sb, buff, buffSize, me->devMap);
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

void __stdcall SSWR::AVIRead::AVIRLoRaJSONForm::OnCSVFile(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRLoRaJSONForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLoRaJSONForm>();
	UIntOS i = 0;
	UIntOS j = files.GetCount();
	while (i < j)
	{
		if (me->LoadCSV(files.GetItem(i)))
		{
			break;
		}
		i++;
	}
}

Bool SSWR::AVIRead::AVIRLoRaJSONForm::LoadCSV(NN<Text::String> fileName)
{
	if (Net::LoRaGWUtil::LoadCSV(fileName->ToCString(), this->devMap))
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendUIntOS(this->devMap.GetCount());
		sb.AppendC(UTF8STRC(" devices: "));
		sb.Append(fileName);
		this->txtDevice->SetText(sb.ToCString());
		return true;
	}
	return false;
}

SSWR::AVIRead::AVIRLoRaJSONForm::AVIRLoRaJSONForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("LoRa JSON Parser"));
	this->SetFont(nullptr, 8.25, false);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlDevice = ui->NewPanel(*this);
	this->pnlDevice->SetRect(0, 0, 100, 23, false);
	this->pnlDevice->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblDevice = ui->NewLabel(this->pnlDevice, CSTR("Devices (CSV)"));
	this->lblDevice->SetRect(0, 0, 100, 23, false);
	this->lblDevice->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->txtDevice = ui->NewTextBox(this->pnlDevice, CSTR(""));
	this->txtDevice->SetReadOnly(true);
	this->txtDevice->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pnlJSON = ui->NewPanel(*this);
	this->pnlJSON->SetRect(0, 0, 100, 103, false);
	this->pnlJSON->SetDockType(UI::GUIControl::DOCK_TOP);
	this->pnlJSONCtrl = ui->NewPanel(this->pnlJSON);
	this->pnlJSONCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlJSONCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lblJSON = ui->NewLabel(this->pnlJSON, CSTR("JSON"));
	this->lblJSON->SetRect(0, 0, 100, 23, false);
	this->lblJSON->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->txtJSON = ui->NewTextBox(this->pnlJSON, CSTR(""), true);
	this->txtJSON->SetDockType(UI::GUIControl::DOCK_FILL);
	this->btnJSONParse = ui->NewButton(this->pnlJSONCtrl, CSTR("Parse"));
	this->btnJSONParse->SetRect(104, 4, 75, 23, false);
	this->btnJSONParse->HandleButtonClick(OnJSONParseClick, this);
	this->lblInfo = ui->NewLabel(*this, CSTR("Info"));
	this->lblInfo->SetRect(0, 0, 100, 23, false);
	this->lblInfo->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->txtInfo = ui->NewTextBox(*this, CSTR(""), true);
	this->txtInfo->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtInfo->SetReadOnly(true);

	this->HandleDropFiles(OnCSVFile, this);
}

SSWR::AVIRead::AVIRLoRaJSONForm::~AVIRLoRaJSONForm()
{
	this->devMap.MemFreeAll();
}

void SSWR::AVIRead::AVIRLoRaJSONForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
