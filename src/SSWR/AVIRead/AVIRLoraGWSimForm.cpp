#include "Stdafx.h"
#include "Crypto/Encrypt/AES128.h"
#include "Crypto/Hash/AESCMAC.h"
#include "SSWR/AVIRead/AVIRLoraGWSimForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/TextBinEnc/Base64Enc.h"
#include "UI/MessageDialog.h"

SSWR::AVIRead::AVIRLoraGWSimForm::PredefData SSWR::AVIRead::AVIRLoraGWSimForm::pdata[] = {
	{UTF8STRC("Arwin LRS2060x Open"), 10, UTF8STRC("040064000001")},
	{UTF8STRC("Arwin LRS2060x Close"), 10, UTF8STRC("040064010001")},
};

void __stdcall SSWR::AVIRead::AVIRLoraGWSimForm::OnStartClick(void *userObj)
{
	SSWR::AVIRead::AVIRLoraGWSimForm *me = (SSWR::AVIRead::AVIRLoraGWSimForm*)userObj;
	if (me->udp)
	{
		DEL_CLASS(me->udp);
		me->udp = 0;
		me->txtGatewayEUI->SetReadOnly(false);
		me->txtServerIP->SetReadOnly(false);
		me->txtServerPort->SetReadOnly(false);
		me->btnStart->SetText(CSTR("Start"));
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		me->txtServerPort->GetText(&sb);
		if (!sb.ToUInt16(&me->svrPort))
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in parsing Server Port"), CSTR("LoRa Gateway Simulator"), me);
			return;
		}
		sb.ClearStr();
		me->txtServerIP->GetText(&sb);
		if (!me->sockf->DNSResolveIP(sb.ToCString(), &me->svrAddr))
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in parsing Server IP"), CSTR("LoRa Gateway Simulator"), me);
			return;
		}
		sb.ClearStr();
		me->txtGatewayEUI->GetText(&sb);
		if (sb.GetLength() != 16 || sb.Hex2Bytes(me->gatewayEUI) != 8)
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in parsing Gateway EUI"), CSTR("LoRa Gateway Simulator"), me);
			return;
		}
		me->tokenNext = (UInt16)(Data::DateTimeUtil::GetCurrTimeMillis() & 0xffff);
		NEW_CLASS(me->udp, Net::UDPServer(me->sockf, 0, 0, CSTR_NULL, OnUDPPacket, me, &me->log, CSTR("LoRa: "), 4, false));
		if (me->udp->IsError())
		{
			DEL_CLASS(me->udp);
			UI::MessageDialog::ShowDialog(CSTR("Error in Listening to UDP"), CSTR("LoRa Gateway Simulator"), me);
			return;
		}
		me->txtGatewayEUI->SetReadOnly(true);
		me->txtServerIP->SetReadOnly(true);
		me->txtServerPort->SetReadOnly(true);
		me->btnStart->SetText(CSTR("Stop"));
	}
}

void __stdcall SSWR::AVIRead::AVIRLoraGWSimForm::OnPredefClicked(void *userObj)
{
	SSWR::AVIRead::AVIRLoraGWSimForm *me = (SSWR::AVIRead::AVIRLoraGWSimForm*)userObj;
	PredefData *data = (PredefData*)me->cboPredef->GetSelectedItem();
	if (data)
	{
		UTF8Char sbuff[16];
		UTF8Char *sptr;
		sptr = Text::StrUInt16(sbuff, data->fPort);
		me->txtFPort->SetText(CSTRP(sbuff, sptr));
		me->txtData->SetText(Text::CString(data->data, data->dataLen));
	}
}

void __stdcall SSWR::AVIRead::AVIRLoraGWSimForm::OnSendULDataClick(void *userObj)
{
	SSWR::AVIRead::AVIRLoraGWSimForm *me = (SSWR::AVIRead::AVIRLoraGWSimForm*)userObj;
	if (me->udp == 0)
	{
		return;
	}
	UInt8 payload[64];
	Text::StringBuilderUTF8 sb;
	UInt8 nwkSKey[16];
	UInt8 appSKey[16];
	UInt32 devAddr;
	UInt32 fCnt;
	Int32 rssi;
	Int32 lsnr;
	UInt8 fPort;
	UInt8 buff[32];
	UOSInt buffLen;

	sb.ClearStr();
	me->txtDevAddr->GetText(&sb);
	if (sb.GetLength() != 8 || sb.Hex2Bytes(payload) != 4)
	{
		UI::MessageDialog::ShowDialog(CSTR("DevAddr invalid"), CSTR("LoRa Gateway Simulator"), me);
		return;
	}
	devAddr = ReadMUInt32(payload);
	sb.ClearStr();
	me->txtNwkSKey->GetText(&sb);
	if (sb.GetLength() != 32 || sb.Hex2Bytes(nwkSKey) != 16)
	{
		UI::MessageDialog::ShowDialog(CSTR("NwkSKey invalid"), CSTR("LoRa Gateway Simulator"), me);
		return;
	}
	sb.ClearStr();
	me->txtAppSKey->GetText(&sb);
	if (sb.GetLength() != 32 || sb.Hex2Bytes(appSKey) != 16)
	{
		UI::MessageDialog::ShowDialog(CSTR("AppSKey invalid"), CSTR("LoRa Gateway Simulator"), me);
		return;
	}
	sb.ClearStr();
	me->txtFCnt->GetText(&sb);
	if (!sb.ToUInt32(&fCnt))
	{
		UI::MessageDialog::ShowDialog(CSTR("FCnt invalid"), CSTR("LoRa Gateway Simulator"), me);
		return;
	}
	sb.ClearStr();
	me->txtRSSI->GetText(&sb);
	if (!sb.ToInt32(&rssi))
	{
		UI::MessageDialog::ShowDialog(CSTR("RSSI invalid"), CSTR("LoRa Gateway Simulator"), me);
		return;
	}
	Double dlsnr;
	sb.ClearStr();
	me->txtLSNR->GetText(&sb);
	if (!sb.ToDouble(&dlsnr))
	{
		UI::MessageDialog::ShowDialog(CSTR("LSNR invalid"), CSTR("LoRa Gateway Simulator"), me);
		return;
	}
	lsnr = Double2Int32(dlsnr * 10);
	sb.ClearStr();
	me->txtFPort->GetText(&sb);
	if (!sb.ToUInt8(&fPort))
	{
		UI::MessageDialog::ShowDialog(CSTR("FPort invalid"), CSTR("LoRa Gateway Simulator"), me);
		return;
	}
	sb.ClearStr();
	me->txtData->GetText(&sb);
	if (sb.GetLength() > 64 || (buffLen = sb.Hex2Bytes(buff)) != (sb.GetLength() >> 1))
	{
		UI::MessageDialog::ShowDialog(CSTR("Data invalid"), CSTR("LoRa Gateway Simulator"), me);
		return;
	}

	UOSInt payloadLen = GenUpPayload(payload, false, devAddr, fCnt, fPort, nwkSKey, appSKey, buff, buffLen);
	sb.ClearStr();
	GenRxpkJSON(&sb, 923400000, 2, 0, 4, rssi, lsnr, payload, payloadLen);

	if (!me->SendPushData(sb.ToString(), sb.GetLength()))
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in sending UL data"), CSTR("LoRa Gateway Simulator"), me);
	}
	else
	{
		sb.ClearStr();
		sb.AppendU32(fCnt + 1);
		me->txtFCnt->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRLoraGWSimForm::OnLogSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRLoraGWSimForm *me = (SSWR::AVIRead::AVIRLoraGWSimForm*)userObj;
	Text::String *s = me->lbLog->GetSelectedItemTextNew();
	me->lbLog->SetText(Text::String::OrEmpty(s)->ToCString());
	SDEL_STRING(s);
}

void __stdcall SSWR::AVIRead::AVIRLoraGWSimForm::OnUDPPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{

}

void __stdcall SSWR::AVIRead::AVIRLoraGWSimForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRLoraGWSimForm *me = (SSWR::AVIRead::AVIRLoraGWSimForm*)userObj;
	if (me->udp)
	{
		me->SendPullData();
	}
}

UOSInt SSWR::AVIRead::AVIRLoraGWSimForm::GenUpPayload(UInt8 *buff, Bool needConfirm, UInt32 devAddr, UInt32 fCnt, UInt8 fPort, const UInt8 *nwkSKey, const UInt8 *appSKey, const UInt8 *payload, UOSInt payloadLen)
{
	UOSInt index;
	// MHDR
	if (needConfirm)
	{
		buff[0] = 0x80;
	}
	else
	{
		buff[0] = 0x40;
	}
	UInt8 ablock[16];
	UInt8 sblock[16];
	// FHDR
	WriteUInt32(&buff[1], devAddr);
	buff[5] = 0x80; //FCtrl = ADR
	WriteUInt16(&buff[6], (UInt16)fCnt);
	buff[8] = fPort;
	index = 9;
	if (payloadLen > 0)
	{
		Crypto::Encrypt::AES128 aes((fPort == 0)?nwkSKey:appSKey);
		ablock[0] = 1;
		ablock[1] = 0;
		ablock[2] = 0;
		ablock[3] = 0;
		ablock[4] = 0;
		ablock[5] = 0; //dir
		WriteUInt32(&ablock[6], devAddr);
		WriteUInt32(&ablock[10], fCnt);
		ablock[14] = 0;
		ablock[15] = 0;
		while (payloadLen > 0)
		{
			ablock[15]++;
			aes.EncryptBlock(ablock, sblock, 0);
			if (payloadLen >= 16)
			{
				WriteNUInt64(&buff[index], ReadNUInt64(sblock) ^ ReadNUInt64(payload));
				WriteNUInt64(&buff[index + 8], ReadNUInt64(&sblock[8]) ^ ReadNUInt64(&payload[8]));
				index += 16;
				payloadLen -= 16;
				payload += 16;
			}
			else
			{
				UOSInt i = 0;
				while (i < payloadLen)
				{
					buff[index + i] = sblock[i] ^ payload[i];
					i++;
				}
				index += payloadLen;
				payloadLen = 0;
				break;
			}
		}
	}
	
	ablock[0] = 0x49;
	ablock[1] = 0;
	ablock[2] = 0;
	ablock[3] = 0;
	ablock[4] = 0;
	ablock[5] = 0; //dir
	WriteUInt32(&ablock[6], devAddr);
	WriteUInt32(&ablock[10], fCnt);
	ablock[14] = 0;
	ablock[15] = (UInt8)index;
	UInt8 cmac[16];
	Crypto::Hash::AESCMAC aescmac(nwkSKey);
	aescmac.Calc(ablock, 16);
	aescmac.Calc(buff, index);
	aescmac.GetValue(cmac);
	WriteNUInt32(&buff[index], ReadNUInt32(cmac));
	return index + 4;
}

void SSWR::AVIRead::AVIRLoraGWSimForm::GenRxpkJSON(Text::StringBuilderUTF8 *sb, UInt32 freq, UInt32 chan, UInt32 rfch, UInt32 codrk, Int32 rssi, Int32 lsnr, const UInt8 *data, UOSInt dataSize)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Data::Timestamp ts = Data::Timestamp::UtcNow();
	sb->AppendC(UTF8STRC("{\"rxpk\":[{\"tmst\":"));
	sb->AppendU32((UInt32)((ts.ticks / 1000) * 1000000) + ts.nanosec / 1000);
	sb->AppendC(UTF8STRC(",\"time\":\""));
	sptr = ts.ToString(sbuff, "yyyy-MM-dd\\THH:mm:ss.ffffff\\Z");
	sb->AppendP(sbuff, sptr);
	sb->AppendC(UTF8STRC("\",\"chan\":"));
	sb->AppendU32(chan);
	sb->AppendC(UTF8STRC(",\"rfch\":"));
	sb->AppendU32(rfch);
	sb->AppendC(UTF8STRC(",\"freq\":"));
	sptr = Text::StrUInt32(sbuff, freq);
	sb->AppendP(sbuff, sptr - 6);
	sb->AppendUTF8Char('.');
	sb->AppendC(sptr - 6, 6);
	sb->AppendC(UTF8STRC(",\"stat\":1,\"modu\":\"LORA\",\"datr\":\"SF7BW125\",\"codr\":\""));
	sb->AppendU32(codrk);
	sb->AppendC(UTF8STRC("/5\",\"lsnr\":"));
	sptr = Text::StrInt32(sbuff, lsnr);
	sb->AppendP(sbuff, sptr - 1);
	sb->AppendUTF8Char('.');
	sb->AppendUTF8Char(sptr[-1]);
	sb->AppendC(UTF8STRC(",\"rssi\":"));
	sb->AppendI32(rssi);
	sb->AppendC(UTF8STRC(",\"size\":"));
	sb->AppendUOSInt(dataSize);
	sb->AppendC(UTF8STRC(",\"data\":\""));
	Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::Normal, false);
	b64.EncodeBin(sb, data, dataSize);
	sb->AppendC(UTF8STRC("\"}]}"));
}

UInt16 SSWR::AVIRead::AVIRLoraGWSimForm::NextToken()
{
	Sync::MutexUsage mutUsage(&this->tokenMut);
	return this->tokenNext++;
}

Bool SSWR::AVIRead::AVIRLoraGWSimForm::SendPushData(const UInt8 *data, UOSInt dataLeng)
{
	UInt8 *buff = MemAlloc(UInt8, 12 + dataLeng);
	UInt16 token = this->NextToken();
	buff[0] = 1; //Version
	WriteMUInt16(&buff[1], token);
	buff[3] = 0; //PUSH_DATA
	MemCopyNO(&buff[4], this->gatewayEUI, 8);
	MemCopyNO(&buff[12], data, dataLeng);
	Bool ret = this->udp->SendTo(&this->svrAddr, this->svrPort, buff, 12 + dataLeng);
	MemFree(buff);
	return ret;
}

Bool SSWR::AVIRead::AVIRLoraGWSimForm::SendPullData()
{
	UInt8 buff[12];
	UInt16 token = this->NextToken();
	buff[0] = 1; //Version
	WriteMUInt16(&buff[1], token);
	buff[3] = 2; //PULL_DATA
	MemCopyNO(&buff[4], this->gatewayEUI, 8);
	return this->udp->SendTo(&this->svrAddr, this->svrPort, buff, 12);
}

SSWR::AVIRead::AVIRLoraGWSimForm::AVIRLoraGWSimForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("LoRa Gateway Simulator"));
	this->SetFont(0, 0, 8.25, false);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->sockf = this->core->GetSocketFactory();
	this->udp = 0;

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpControl = this->tcMain->AddTabPage(CSTR("Control"));
	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this->tpControl));
	this->pnlControl->SetRect(0, 0, 100, 103, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblServerIP, UI::GUILabel(ui, this->pnlControl, CSTR("Server IP")));
	this->lblServerIP->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtServerIP, UI::GUITextBox(ui, this->pnlControl, CSTR("127.0.0.1")));
	this->txtServerIP->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->lblServerPort, UI::GUILabel(ui, this->pnlControl, CSTR("Server Port")));
	this->lblServerPort->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtServerPort, UI::GUITextBox(ui, this->pnlControl, CSTR("1700")));
	this->txtServerPort->SetRect(104, 28, 50, 23, false);
	NEW_CLASS(this->lblGatewayEUI, UI::GUILabel(ui, this->pnlControl, CSTR("GatewayEUI")));
	this->lblGatewayEUI->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtGatewayEUI, UI::GUITextBox(ui, this->pnlControl, CSTR("1122334455667788")));
	this->txtGatewayEUI->SetRect(104, 52, 150, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->pnlControl, CSTR("Start")));
	this->btnStart->SetRect(104, 76, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClick, this);
	NEW_CLASS(this->pnlDevice, UI::GUIPanel(ui, this->tpControl));
	this->pnlDevice->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblDevAddr, UI::GUILabel(ui, this->pnlDevice, CSTR("DevAddr")));
	this->lblDevAddr->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtDevAddr, UI::GUITextBox(ui, this->pnlDevice, CSTR("00e94cc2")));
	this->txtDevAddr->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->lblNwkSKey, UI::GUILabel(ui, this->pnlDevice, CSTR("NwkSKey")));
	this->lblNwkSKey->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtNwkSKey, UI::GUITextBox(ui, this->pnlDevice, CSTR("484e854a06c6794f93976fa1f8677eac")));
	this->txtNwkSKey->SetRect(104, 28, 200, 23, false);
	NEW_CLASS(this->lblAppSKey, UI::GUILabel(ui, this->pnlDevice, CSTR("AppSKey")));
	this->lblAppSKey->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtAppSKey, UI::GUITextBox(ui, this->pnlDevice, CSTR("6ba625cc40fc701ae67528a052907a9e")));
	this->txtAppSKey->SetRect(104, 52, 200, 23, false);
	NEW_CLASS(this->lblFCnt, UI::GUILabel(ui, this->pnlDevice, CSTR("FCnt")));
	this->lblFCnt->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtFCnt, UI::GUITextBox(ui, this->pnlDevice, CSTR("1")));
	this->txtFCnt->SetRect(104, 76, 100, 23, false);
	NEW_CLASS(this->lblRSSI, UI::GUILabel(ui, this->pnlDevice, CSTR("RSSI")));
	this->lblRSSI->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtRSSI, UI::GUITextBox(ui, this->pnlDevice, CSTR("-100")));
	this->txtRSSI->SetRect(104, 100, 100, 23, false);
	NEW_CLASS(this->lblLSNR, UI::GUILabel(ui, this->pnlDevice, CSTR("LSNR")));
	this->lblLSNR->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtLSNR, UI::GUITextBox(ui, this->pnlDevice, CSTR("-12.0")));
	this->txtLSNR->SetRect(104, 124, 100, 23, false);
	NEW_CLASS(this->lblPredef, UI::GUILabel(ui, this->pnlDevice, CSTR("Predef Data")));
	this->lblPredef->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->cboPredef, UI::GUIComboBox(ui, this->pnlDevice, false));
	this->cboPredef->SetRect(104, 148, 250, 23, false);
	NEW_CLASS(this->btnPredef, UI::GUIButton(ui, this->pnlDevice, CSTR("Set")));
	this->btnPredef->SetRect(354, 148, 75, 23, false);
	this->btnPredef->HandleButtonClick(OnPredefClicked, this);
	NEW_CLASS(this->lblFPort, UI::GUILabel(ui, this->pnlDevice, CSTR("FPort")));
	this->lblFPort->SetRect(4, 172, 100, 23, false);
	NEW_CLASS(this->txtFPort, UI::GUITextBox(ui, this->pnlDevice, CSTR("1")));
	this->txtFPort->SetRect(104, 172, 100, 23, false);
	NEW_CLASS(this->lblData, UI::GUILabel(ui, this->pnlDevice, CSTR("Data(Hex)")));
	this->lblData->SetRect(4, 196, 100, 23, false);
	NEW_CLASS(this->txtData, UI::GUITextBox(ui, this->pnlDevice, CSTR("00000000")));
	this->txtData->SetRect(104, 196, 300, 23, false);
	NEW_CLASS(this->btnSendULData, UI::GUIButton(ui, this->pnlDevice, CSTR("Send UL Data")));
	this->btnSendULData->SetRect(104, 220, 75, 23, false);
	this->btnSendULData->HandleButtonClick(OnSendULDataClick, this);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this->tpLog, CSTR("")));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelChg, this);

	NEW_CLASS(this->logger, UI::ListBoxLogger(this, this->lbLog, 100, true));
	this->log.AddLogHandler(this->logger, IO::ILogHandler::LOG_LEVEL_RAW);

	this->AddTimer(10000, OnTimerTick, this);

	UOSInt i = 0;
	UOSInt j = sizeof(pdata) / sizeof(pdata[0]);
	while (i < j)
	{
		this->cboPredef->AddItem(Text::CString(pdata[i].name, pdata[i].nameLen), &pdata[i]);
		i++;
	}
	if (j > 0)
	{
		this->cboPredef->SetSelectedIndex(0);
	}
}

SSWR::AVIRead::AVIRLoraGWSimForm::~AVIRLoraGWSimForm()
{
	SDEL_CLASS(this->udp);
	this->log.RemoveLogHandler(this->logger);
	DEL_CLASS(this->logger);
}

void SSWR::AVIRead::AVIRLoraGWSimForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
