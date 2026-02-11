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
				me->PHYPayloadDetail(sb, buff, buffSize);
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
			me->PHYPayloadDetail(sb, buff, buffSize);
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

void SSWR::AVIRead::AVIRLoRaJSONForm::PHYPayloadDetail(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> buff, UIntOS buffSize)
{
	UInt32 devAddr = 0;
	UInt32 fCnt = 0;
	Bool downLink = false;
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
		devAddr = MACPayloadDetail(sb, downLink = false, buff + 1, buffSize - 5, fCnt);
		break;
	case 3:
		sb->AppendC(UTF8STRC("Message Type = Unconfirmed Data Down\r\n"));
		devAddr = MACPayloadDetail(sb, downLink = true, buff + 1, buffSize - 5, fCnt);
		break;
	case 4:
		sb->AppendC(UTF8STRC("Message Type = Confirmed Data Up\r\n"));
		devAddr = MACPayloadDetail(sb, downLink = false, buff + 1, buffSize - 5, fCnt);
		break;
	case 5:
		sb->AppendC(UTF8STRC("Message Type = Confirmed Data Down\r\n"));
		devAddr = MACPayloadDetail(sb, downLink = true, buff + 1, buffSize - 5, fCnt);
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
	UInt8 calcMIC[4];
	NN<LoRaDevInfo> dev;
	if (this->devMap.Get(devAddr).SetTo(dev))
	{
		Net::LoRaGWUtil::CalcMIC(calcMIC, devAddr, fCnt, downLink, dev->nwkSKey, buff, buffSize - 4);
		sb->AppendC(UTF8STRC("Calculated MIC = 0x"));
		sb->AppendHexBuff(calcMIC, 4, 0, Text::LineBreakType::None);
		sb->AppendC(UTF8STRC("\r\n"));
	}
}

UInt32 SSWR::AVIRead::AVIRLoRaJSONForm::MACPayloadDetail(NN<Text::StringBuilderUTF8> sb, Bool donwLink, UnsafeArray<const UInt8> buff, UIntOS buffSize, OutParam<UInt32> fCnt)
{
	if (buffSize < 7)
	{
		return 0;
	}
	UInt32 devAddr = ReadUInt32(&buff[0]);
	sb->AppendC(UTF8STRC("DevAddr = 0x"));
	sb->AppendHex32(devAddr);
	sb->AppendC(UTF8STRC("\r\nADR = "));
	sb->AppendUIntOS(((UIntOS)buff[4] & 0x80) >> 7);
	sb->AppendC(UTF8STRC("\r\nACK = "));
	sb->AppendUIntOS(((UIntOS)buff[4] & 0x20) >> 5);
	if (donwLink)
	{
		sb->AppendC(UTF8STRC("\r\nRFU = "));
		sb->AppendUIntOS(((UIntOS)buff[4] & 0x40) >> 6);
		sb->AppendC(UTF8STRC("\r\nFPending = "));
		sb->AppendUIntOS(((UIntOS)buff[4] & 0x10) >> 4);
	}
	else
	{
		sb->AppendC(UTF8STRC("\r\nADRACKReq = "));
		sb->AppendUIntOS(((UIntOS)buff[4] & 0x40) >> 6);
		sb->AppendC(UTF8STRC("\r\nClassB = "));
		sb->AppendUIntOS(((UIntOS)buff[4] & 0x10) >> 4);
	}
	UIntOS fOptsLen = (UIntOS)buff[4] & 0xF;
	sb->AppendC(UTF8STRC("\r\nFOptsLen = "));
	sb->AppendUIntOS(fOptsLen);
	sb->AppendC(UTF8STRC("\r\nFCnt = "));
	sb->AppendU16(ReadUInt16(&buff[5]));
	fCnt.Set(ReadUInt16(&buff[5]));
	if (fOptsLen + 7 > buffSize)
	{
		sb->AppendC(UTF8STRC("\r\n"));
		return devAddr;
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
		return devAddr;
	}
	sb->AppendC(UTF8STRC("FPort = "));
	sb->AppendU16(buff[0]);
	if (buffSize > 1)
	{
		sb->AppendC(UTF8STRC("\r\nFRMPayload = "));
		sb->AppendHexBuff(buff + 1, buffSize - 1, ' ', Text::LineBreakType::None);
	}
	sb->AppendC(UTF8STRC("\r\n"));
	NN<LoRaDevInfo> dev;
	if (this->devMap.Get(devAddr).SetTo(dev))
	{
		sb->AppendC(UTF8STRC("DevEUI = 0x"));
		sb->AppendHexBuff(dev->devEUI, 8, 0, Text::LineBreakType::None);
		sb->AppendC(UTF8STRC("\r\n"));
		sb->AppendC(UTF8STRC("NwkSKey = 0x"));
		sb->AppendHexBuff(dev->nwkSKey, 16, 0, Text::LineBreakType::None);
		sb->AppendC(UTF8STRC("\r\n"));
		sb->AppendC(UTF8STRC("AppSKey = 0x"));
		sb->AppendHexBuff(dev->appSKey, 16, 0, Text::LineBreakType::None);
		sb->AppendC(UTF8STRC("\r\n"));
	}
	return devAddr;
}

Bool SSWR::AVIRead::AVIRLoRaJSONForm::LoadCSV(NN<Text::String> fileName)
{
	UIntOS devEUICol = INVALID_INDEX;
	UIntOS nwkSKeyCol = INVALID_INDEX;
	UIntOS appSKeyCol = INVALID_INDEX;
	DB::CSVFile csv(fileName, 65001);
	NN<DB::DBReader> r;
	if (!csv.QueryTableData(nullptr, CSTR(""), nullptr, 0, 0, nullptr, nullptr).SetTo(r))
	{
		return false;
	}
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UIntOS j = r->ColCount();
	while (j-- > 0)
	{
		if (r->GetName(j, sbuff).SetTo(sptr))
		{
			Text::CStringNN colName = CSTRP(sbuff, sptr);
			if (colName.EqualsICase(UTF8STRC("DevEUI")))
			{
				devEUICol = j;
			}
			else if (colName.EqualsICase(UTF8STRC("NwkSKey")))
			{
				nwkSKeyCol = j;
			}
			else if (colName.EqualsICase(UTF8STRC("AppSKey")))
			{
				appSKeyCol = j;
			}
		}
	}

	if (devEUICol == INVALID_INDEX || nwkSKeyCol == INVALID_INDEX || appSKeyCol == INVALID_INDEX)
	{
		csv.CloseReader(r);
		return false;
	}
	this->devMap.MemFreeAll();
	while (r->ReadNext())
	{
		NN<LoRaDevInfo> dev;
		NN<Text::String> devEUI = r->GetNewStrNN(devEUICol);
		NN<Text::String> nwkSKey = r->GetNewStrNN(nwkSKeyCol);
		NN<Text::String> appSKey = r->GetNewStrNN(appSKeyCol);
		if (devEUI->leng == 16 && nwkSKey->leng == 32 && appSKey->leng == 32)
		{
			dev = MemAllocNN(LoRaDevInfo);
			if (devEUI->Hex2Bytes(dev->devEUI) == 8 && nwkSKey->Hex2Bytes(dev->nwkSKey) == 16 && appSKey->Hex2Bytes(dev->appSKey) == 16)
			{
				this->devMap.Put(ReadMUInt32(&dev->devEUI[4]), dev);
			}
			else
			{
				MemFreeNN(dev);
			}
		}
		devEUI->Release();
		nwkSKey->Release();
		appSKey->Release();
	}
	csv.CloseReader(r);
	Text::StringBuilderUTF8 sb;
	sb.AppendUIntOS(this->devMap.GetCount());
	sb.AppendC(UTF8STRC(" devices: "));
	sb.Append(fileName);
	this->txtDevice->SetText(sb.ToCString());
	return true;
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
	this->lblDevice = ui->NewLabel(this->pnlDevice, CSTR("Devices"));
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
