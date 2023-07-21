#include "Stdafx.h"
#include "Net/LoRaGWUtil.h"
#include "SSWR/AVIRead/AVIRLoraGWSimForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "UI/MessageDialog.h"

#include <stdio.h>

SSWR::AVIRead::AVIRLoraGWSimForm::PredefData SSWR::AVIRead::AVIRLoraGWSimForm::pdata[] = {
	{UTF8STRC("Arwin LRS2060x Open"), 10, UTF8STRC("040064000001")},
	{UTF8STRC("Arwin LRS2060x Close"), 10, UTF8STRC("040064010001")},
};

void __stdcall SSWR::AVIRead::AVIRLoraGWSimForm::OnStartClick(void *userObj)
{
	SSWR::AVIRead::AVIRLoraGWSimForm *me = (SSWR::AVIRead::AVIRLoraGWSimForm*)userObj;
	if (me->lora)
	{
		DEL_CLASS(me->lora);
		me->lora = 0;
		me->txtGatewayEUI->SetReadOnly(false);
		me->txtServerIP->SetReadOnly(false);
		me->txtServerPort->SetReadOnly(false);
		me->btnStart->SetText(CSTR("Start"));
	}
	else
	{
		Net::SocketUtil::AddressInfo svrAddr;
		UInt16 svrPort;
		UInt8 gatewayEUI[8];
		Text::StringBuilderUTF8 sb;
		me->txtServerPort->GetText(&sb);
		if (!sb.ToUInt16(&svrPort))
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in parsing Server Port"), CSTR("LoRa Gateway Simulator"), me);
			return;
		}
		sb.ClearStr();
		me->txtServerIP->GetText(&sb);
		if (!me->sockf->DNSResolveIP(sb.ToCString(), &svrAddr))
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in parsing Server IP"), CSTR("LoRa Gateway Simulator"), me);
			return;
		}
		sb.ClearStr();
		me->txtGatewayEUI->GetText(&sb);
		if (sb.GetLength() != 16 || sb.Hex2Bytes(gatewayEUI) != 8)
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in parsing Gateway EUI"), CSTR("LoRa Gateway Simulator"), me);
			return;
		}
		NEW_CLASS(me->lora, Net::LoRaGateway(me->sockf, &svrAddr, svrPort, gatewayEUI, &me->log));
		if (me->lora->IsError())
		{
			DEL_CLASS(me->lora);
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
	if (me->lora == 0)
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

	UOSInt payloadLen = Net::LoRaGWUtil::GenUpPayload(payload, false, devAddr, fCnt, fPort, nwkSKey, appSKey, buff, buffLen);
	sb.ClearStr();
	Net::LoRaGWUtil::GenRxpkJSON(&sb, 923400000, 2, 0, 4, rssi, lsnr, payload, payloadLen);

	if (!me->lora->SendPushData(sb.ToString(), sb.GetLength()))
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

SSWR::AVIRead::AVIRLoraGWSimForm::AVIRLoraGWSimForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("LoRa Gateway Simulator"));
	this->SetFont(0, 0, 8.25, false);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->sockf = this->core->GetSocketFactory();
	this->lora = 0;

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
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Raw);

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
	SDEL_CLASS(this->lora);
	this->log.RemoveLogHandler(this->logger);
	DEL_CLASS(this->logger);
}

void SSWR::AVIRead::AVIRLoraGWSimForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
