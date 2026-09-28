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
				Net::LoRaGWUtil::PHYPayloadDetail(sb, buff, buffSize, this->devMap);
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
			Net::LoRaGWUtil::PHYPayloadDetail(sb, buff, buffSize, this->devMap);
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

Bool SSWR::AVIRead::AVIRLoRaLogForm::LoadCSV(NN<Text::String> fileName)
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
	else
	{
		return false;
	}
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
	Text::StringBuilderUTF8 sb;
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
						UInt32 mic = ReadMUInt32(&buff[buffSize - 4]);
						sptr = Text::StrHexVal32(sbuff, mic);
						this->lvLog->SetSubItem(k, 7, CSTRP(sbuff, sptr));
						if (msgType >= 2 && msgType <= 5)
						{
							UInt32 devAddr = ReadLUInt32(&buff[1]);
							sptr = Text::StrHexVal32(sbuff, devAddr);

							this->lvLog->SetSubItem(k, 5, CSTRP(sbuff, sptr));

							NN<Net::LoRaGWUtil::LoRaDevInfo> dev;
							if (this->devMap.Get(devAddr).SetTo(dev))
							{
								UInt8 calcMIC[4];
								UInt32 fCnt = 0;
								UIntOS l = 0;
								switch (msgType)
								{
								case 2:
								case 4:
									devAddr = Net::LoRaGWUtil::MACPayloadDetail(sb, this->devMap, false, buff + 1, buffSize - 5, fCnt);
									Net::LoRaGWUtil::CalcMIC(calcMIC, devAddr, fCnt, false, dev->nwkSKey, buff, buffSize - 4);
									sptr = Text::StrHexBytes(sbuff, calcMIC, 4, 0);
									this->lvLog->SetSubItem(k, 8, CSTRP(sbuff, sptr));
									sptr = Text::StrUInt32(sbuff, fCnt);
									this->lvLog->SetSubItem(k, 9, CSTRP(sbuff, sptr));
									if (ReadMUInt32(calcMIC) != mic)
									{
										while (l < 8)
										{
											fCnt += 65536;
											Net::LoRaGWUtil::CalcMIC(calcMIC, devAddr, fCnt, false, dev->nwkSKey, buff, buffSize - 4);
											if (ReadMUInt32(calcMIC) == mic)
											{
												sptr = Text::StrHexBytes(sbuff, calcMIC, 4, 0);
												this->lvLog->SetSubItem(k, 8, CSTRP(sbuff, sptr));
												sptr = Text::StrUInt32(sbuff, fCnt);
												this->lvLog->SetSubItem(k, 9, CSTRP(sbuff, sptr));
												break;
											}
											l++;
										}
									}
									break;
								case 3:
								case 5:
									devAddr = Net::LoRaGWUtil::MACPayloadDetail(sb, this->devMap, true, buff + 1, buffSize - 5, fCnt);
									Net::LoRaGWUtil::CalcMIC(calcMIC, devAddr, fCnt, true, dev->nwkSKey, buff, buffSize - 4);
									sptr = Text::StrHexBytes(sbuff, calcMIC, 4, 0);
									this->lvLog->SetSubItem(k, 8, CSTRP(sbuff, sptr));
									sptr = Text::StrUInt32(sbuff, fCnt);
									this->lvLog->SetSubItem(k, 9, CSTRP(sbuff, sptr));
									if (ReadMUInt32(calcMIC) != mic)
									{
										while (l < 8)
										{
											fCnt += 65536;
											Net::LoRaGWUtil::CalcMIC(calcMIC, devAddr, fCnt, true, dev->nwkSKey, buff, buffSize - 4);
											if (ReadMUInt32(calcMIC) == mic)
											{
												sptr = Text::StrHexBytes(sbuff, calcMIC, 4, 0);
												this->lvLog->SetSubItem(k, 8, CSTRP(sbuff, sptr));
												sptr = Text::StrUInt32(sbuff, fCnt);
												this->lvLog->SetSubItem(k, 9, CSTRP(sbuff, sptr));
												break;
											}
											l++;
										}
									}
									break;
								}
							}
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
	this->lvLog = ui->NewListView(*this, UI::ListViewStyle::Table, 10);
	this->lvLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvLog->AddColumn(CSTR("Timestamp"), 130);
	this->lvLog->AddColumn(CSTR("From Server"), 60);
	this->lvLog->AddColumn(CSTR("GW EUI"), 120);
	this->lvLog->AddColumn(CSTR("Push"), 30);
	this->lvLog->AddColumn(CSTR("Token"), 40);
	this->lvLog->AddColumn(CSTR("Dev Addr"), 80);
	this->lvLog->AddColumn(CSTR("Message Type"), 150);
	this->lvLog->AddColumn(CSTR("MIC"), 100);
	this->lvLog->AddColumn(CSTR("Calc MIC"), 100);
	this->lvLog->AddColumn(CSTR("FCnt"), 60);
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
