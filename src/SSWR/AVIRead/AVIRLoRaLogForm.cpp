#include "Stdafx.h"
#include "DB/CSVFile.h"
#include "IO/FileStream.h"
#include "Net/LoRaGWUtil.h"
#include "SSWR/AVIRead/AVIRLoRaLogForm.h"
#include "Text/JSON.h"
#include "Text/UTF8Reader.h"
#include "Text/TextBinEnc/Base64Enc.h"

void __stdcall SSWR::AVIRead::AVIRLoRaLogForm::FreeLoRaLogEntry(NN<LoRaLogEntry> logEntry)
{
	logEntry->json->Release();
	MemFreeNN(logEntry);
}

void __stdcall SSWR::AVIRead::AVIRLoRaLogForm::OnCSVFile(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRLoRaLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLoRaLogForm>();
	NN<Text::String> fileName;
	UIntOS i = 0;
	UIntOS j = files.GetCount();
	while (i < j)
	{
		fileName = files.GetItem(i);
		if (fileName->EndsWith(CSTR(".csv")))
		{
			me->LoadCSV(fileName);
		}
		else if (fileName->EndsWith(CSTR(".log")))
		{
			me->LoadLog(fileName);
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRLoRaLogForm::OnGatewayChanged(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLoRaLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLoRaLogForm>();
	UIntOS selIndex = me->cboGateway->GetSelectedIndex();
	if (selIndex == 0)
	{
		me->ShowLog(nullptr);
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		me->cboGateway->GetText(sb);
		me->ShowLog(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRLoRaLogForm::OnLogSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLoRaLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLoRaLogForm>();
	UIntOS i = me->lvLog->GetSelectedIndex();
	NN<LoRaLogEntry> logEntry;
	if (i != INVALID_INDEX && me->lvLog->GetItem(i).GetOpt<LoRaLogEntry>().SetTo(logEntry))
	{
		me->ParseJSONText(logEntry->json);
	}
	else
	{
		me->txtDetail->SetText(CSTR("")); // Clear the detail text box
	}
}

void SSWR::AVIRead::AVIRLoRaLogForm::ParseJSONText(NN<Text::String> jsonText)
{
	NN<Text::JSONBase> json;
	UInt8 buff[256];
	UIntOS buffSize;
	if (Text::JSONBase::ParseJSONStr(jsonText->ToCString()).SetTo(json))
	{
		NN<Text::String> rxdata;
		NN<Text::JSONBase> rxstat;
		NN<Text::String> txdata;
		if (json->GetValueString(CSTR("rxpk[0].data")).SetTo(rxdata))
		{
			Text::StringBuilderUTF8 sb;
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
				this->PHYPayloadDetail(sb, buff, buffSize);
			}
			else
			{
				sb.AppendC(UTF8STRC("CRC invalid"));
			}
			this->txtDetail->SetText(sb.ToCString());
		}
		else if (json->GetValueString(CSTR("txpk.data")).SetTo(txdata))
		{
			Text::StringBuilderUTF8 sb;
			Text::TextBinEnc::Base64Enc b64;
			buffSize = b64.DecodeBin(txdata->ToCString(), buff);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Transmitted Packet:\r\n"));
			sb.AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::CRLF);
			sb.AppendC(UTF8STRC("\r\n\r\n"));
			this->PHYPayloadDetail(sb, buff, buffSize);
			this->txtDetail->SetText(sb.ToCString());
		}
		else
		{
			this->txtDetail->SetText(CSTR("Data not found"));
		}
		json->EndUse();
	}
	else
	{
		this->txtDetail->SetText(CSTR("Not JSON String"));
	}
}

void SSWR::AVIRead::AVIRLoRaLogForm::PHYPayloadDetail(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> buff, UIntOS buffSize)
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

UInt32 SSWR::AVIRead::AVIRLoRaLogForm::MACPayloadDetail(NN<Text::StringBuilderUTF8> sb, Bool downLink, UnsafeArray<const UInt8> buff, UIntOS buffSize, OutParam<UInt32> fCnt)
{
	if (buffSize < 7)
	{
		return 0;
	}
	UInt32 devAddr = ReadLUInt32(&buff[0]);
	sb->AppendC(UTF8STRC("DevAddr = 0x"));
	sb->AppendHex32(devAddr);
	sb->AppendC(UTF8STRC("\r\nADR = "));
	sb->AppendUIntOS(((UIntOS)buff[4] & 0x80) >> 7);
	sb->AppendC(UTF8STRC("\r\nACK = "));
	sb->AppendUIntOS(((UIntOS)buff[4] & 0x20) >> 5);
	if (downLink)
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
	sb->AppendU16(ReadLUInt16(&buff[5]));
	fCnt.Set(ReadLUInt16(&buff[5]));
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

Bool SSWR::AVIRead::AVIRLoRaLogForm::LoadCSV(NN<Text::String> fileName)
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

Bool SSWR::AVIRead::AVIRLoRaLogForm::LoadLog(NN<Text::String> fileName)
{
	this->logList.FreeAll(FreeLoRaLogEntry);
	this->cboGateway->ClearItems();
	this->cboGateway->AddItem(CSTR("All"), 0);
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	Text::UTF8Reader reader(fs);
	Text::PString sarr[7];
	NN<LoRaLogEntry> logEntry;
	Data::FastStringMapNative<UInt32> gatewayMap;
	Text::StringBuilderUTF8 sb;
	while (reader.ReadLine(sb, 4096))
	{
		if (sb.leng > 23 && sb.v[23] == '\t')
		{
			sb.v[23] = 0;
			Data::Timestamp ts = Data::Timestamp::FromStr(Text::CStringNN(sb.v, 23), Data::DateTimeUtil::GetLocalTzQhr());
			if (ts.NotNull() && Text::StrSplitTrimP(sarr, 7, sb.Substring(24), ',') == 7 && sarr[2].StartsWith(CSTR("token=")) && sarr[4].StartsWith(CSTR("type=")) && sarr[5].StartsWith(CSTR("GWEUI=")) && sarr[6].StartsWith(CSTR("Payload=")))
			{
				Bool frServer = sarr[0].Equals(CSTR("Fr Server"));
				UInt32 token = sarr[2].Substring(6).ToUInt32();
				Bool push = sarr[4].Equals(CSTR("type=PUSH_DATA"));
				Text::CStringNN payload = sarr[6].Substring(8).ToCString();
				NN<Text::JSONBase> json;
				NN<Text::String> data;
				if (Text::JSONBase::ParseJSONStr(payload).SetTo(json))
				{
					if (json->GetValueString(CSTR("rxpk[0].data")).SetTo(data) || json->GetValueString(CSTR("txpk[0].data")).SetTo(data))
					{
						logEntry = MemAllocNN(LoRaLogEntry);
						logEntry->frServer = frServer;
						logEntry->token = token;
						logEntry->push = push;
						logEntry->ts = ts;
						sarr[5].Substring(6).Hex2Bytes(logEntry->gwEUI);
						logEntry->json = Text::String::New(payload);
						this->logList.Add(logEntry);
						Text::CStringNN gwStr = sarr[5].Substring(6).ToCString();
						if (gatewayMap.GetC(gwStr) == 0)
						{
							gatewayMap.PutC(gwStr, 1);
							this->cboGateway->AddItem(gwStr, 0);
						}
					}
					json->EndUse();
				}
			}
		}
		sb.ClearStr();
	}
	this->txtLog->SetText(fileName->ToCString());
	this->cboGateway->SetSelectedIndex(0);
	this->ShowLog(nullptr);
	return true;
}

void SSWR::AVIRead::AVIRLoRaLogForm::ShowLog(Text::CString gateway)
{
	UInt8 gwEUI[8];
	Bool hasGateway = false;
	Text::CStringNN nncstr;
	if (gateway.SetTo(nncstr) && nncstr.leng == 16)
	{
		hasGateway = true;
		nncstr.Hex2Bytes(gwEUI);
	}

	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	UInt8 buff[256];
	UIntOS buffSize;
	Text::TextBinEnc::Base64Enc b64;
	NN<Text::JSONBase> json;
	NN<Text::String> data;
	NN<LoRaLogEntry> logEntry;
	this->lvLog->ClearItems();
	UIntOS i = 0;
	UIntOS j = this->logList.GetCount();
	while (i < j)
	{
		logEntry = this->logList.GetItemNoCheck(i);
		if (!hasGateway || Text::StrEqualsC(logEntry->gwEUI, 8, gwEUI, 8))
		{
			if (Text::JSONBase::ParseJSONStr(logEntry->json->ToCString()).SetTo(json))
			{
				if (json->GetValueString(CSTR("rxpk[0].data")).SetTo(data) || json->GetValueString(CSTR("txpk[0].data")).SetTo(data))
				{
					buffSize = b64.DecodeBin(data->ToCString(), buff);
					sptr = logEntry->ts.ToStringNoZone(sbuff);
					UIntOS k = this->lvLog->AddItem(CSTRP(sbuff, sptr), logEntry);
					this->lvLog->SetSubItem(k, 1, logEntry->frServer?CSTR("Yes"):CSTR("No"));
					sptr = Text::StrHexBytes(sbuff, logEntry->gwEUI, 8, 0);
					this->lvLog->SetSubItem(k, 2, CSTRP(sbuff, sptr));
					this->lvLog->SetSubItem(k, 3, logEntry->push?CSTR("Yes"):CSTR("No"));
					sptr = Text::StrUInt32(sbuff, logEntry->token);
					this->lvLog->SetSubItem(k, 4, CSTRP(sbuff, sptr));
					if (buffSize >= 5)
					{
						UInt8 msgType = buff[0] >> 5;
						if (msgType >= 2 && msgType <= 5)
						{
							UInt32 devAddr = ReadLUInt32(&buff[1]);
							sptr = Text::StrHexVal32(sbuff, devAddr);
							this->lvLog->SetSubItem(k, 5, CSTRP(sbuff, sptr));
						}
						this->lvLog->SetSubItem(k, 6, Net::LoRaGWUtil::MessageTypeGetName(msgType));
					}
				}
				json->EndUse();
			}
		}

		i++;
	}
	this->txtDetail->SetText(CSTR(""));
}

SSWR::AVIRead::AVIRLoRaLogForm::AVIRLoRaLogForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
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
	this->pnlLog = ui->NewPanel(*this);
	this->pnlLog->SetRect(0, 0, 100, 23, false);
	this->pnlLog->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblLog = ui->NewLabel(this->pnlLog, CSTR("Log"));
	this->lblLog->SetRect(0, 0, 100, 23, false);
	this->lblLog->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->txtLog = ui->NewTextBox(this->pnlLog, CSTR(""));
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pnlCtrl = ui->NewPanel(*this);
	this->pnlCtrl->SetRect(0, 0, 100, 23, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblGateway = ui->NewLabel(this->pnlCtrl, CSTR("Gateway"));
	this->lblGateway->SetRect(0, 0, 100, 23, false);
	this->cboGateway = ui->NewComboBox(this->pnlCtrl, false);
	this->cboGateway->SetRect(100, 0, 200, 23, false);
	this->cboGateway->HandleSelectionChange(OnGatewayChanged, this);
	this->txtDetail = ui->NewTextBox(*this, CSTR(""), true);
	this->txtDetail->SetReadOnly(true);
	this->txtDetail->SetRect(0, 0, 200, 300, false);
	this->txtDetail->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lvLog = ui->NewListView(*this, UI::ListViewStyle::Table, 8);
	this->lvLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvLog->AddColumn(CSTR("Timestamp"), 150);
	this->lvLog->AddColumn(CSTR("From Server"), 100);
	this->lvLog->AddColumn(CSTR("GW EUI"), 150);
	this->lvLog->AddColumn(CSTR("Push"), 100);
	this->lvLog->AddColumn(CSTR("Token"), 100);
	this->lvLog->AddColumn(CSTR("Dev Addr"), 150);
	this->lvLog->AddColumn(CSTR("Message Type"), 150);
	this->lvLog->HandleSelChg(OnLogSelChg, this);

	this->HandleDropFiles(OnCSVFile, this);
}

SSWR::AVIRead::AVIRLoRaLogForm::~AVIRLoRaLogForm()
{
	this->devMap.MemFreeAll();
	this->logList.FreeAll(FreeLoRaLogEntry);
}

void SSWR::AVIRead::AVIRLoRaLogForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
