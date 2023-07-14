#include "Stdafx.h"
#include "IO/SerialPort.h"
#include "SSWR/AVIRead/AVIRADAMForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRADAMForm::OnStreamClicked(void *userObj)
{
	SSWR::AVIRead::AVIRADAMForm *me = (SSWR::AVIRead::AVIRADAMForm *)userObj;
	if (me->stm)
	{
		me->StopStream(true);
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		me->txtAddress->GetText(&sb);
		if (!sb.ToUInt8(&me->channelAddr))
		{
			UI::MessageDialog::ShowDialog(CSTR("Address out of range (0-255)"), CSTR("ADAM"), me);
			return;
		}
		IO::StreamType st;
		NotNullPtr<IO::Stream> stm;
		if (stm.Set(me->core->OpenStream(&st, me, 0, false)))
		{
			me->stm = stm.Ptr();
			UTF8Char sbuff[64];
			UTF8Char *sptr;
			NEW_CLASS(me->channel, IO::AdvantechASCIIChannel(stm, true));
			me->channelModule = 0;
			me->txtStream->SetText(IO::StreamTypeGetName(st));
			me->btnStream->SetText(CSTR("&Close"));
			me->txtAddress->SetReadOnly(true);

			if ((sptr = me->channel->GetModuleName(sbuff, me->channelAddr)) != 0)
			{
				me->txtModuleName->SetText(CSTRP(sbuff, sptr));
				if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("4050")))
				{
					me->channelModule = 4050;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("4051")))
				{
					me->channelModule = 4051;
				}
			}
			else
			{
				me->txtModuleName->SetText(CSTR("-"));
			}

			if ((sptr = me->channel->GetFirmwareVer(sbuff, me->channelAddr)) != 0)
			{
				me->txtFirmware->SetText(CSTRP(sbuff, sptr));
			}
			else
			{
				me->txtFirmware->SetText(CSTR("-"));
			}

			IO::AdvantechASCIIChannel::ADAMConfig cfg;
			if (me->channel->GetConfigStatus(me->channelAddr, &cfg))
			{
				sptr = Text::StrUInt16(sbuff, cfg.addr);
				me->txtDevAddress->SetText(CSTRP(sbuff, sptr));
				sptr = Text::StrUInt32(sbuff, IO::AdvantechASCIIChannel::BaudRateGetBps(cfg.baudRateCode));
				me->txtDevBaudRate->SetText(CSTRP(sbuff, sptr));
				me->txtInputType->SetText(IO::AdvantechASCIIChannel::TypeCodeGetName(cfg.typeCode));
				me->txtChecksum->SetText(cfg.checksumEnabled?CSTR("enabled"):CSTR("disabled"));
				me->txtModbusProtocol->SetText(cfg.modbusProtocol?CSTR("MODBUS"):CSTR("Advantech"));
				me->txtDataFormat->SetText(IO::AdvantechASCIIChannel::DataFormatGetName(cfg.dataFormat));
				me->txtIntegrationTime->SetText(IO::AdvantechASCIIChannel::IntegrationTimeGetName(cfg.intTime));
				me->txtSlewRate->SetText(IO::AdvantechASCIIChannel::SlewRateGetName(cfg.slewRate));
			}
			else
			{
				me->txtDevAddress->SetText(CSTR("?"));
				me->txtDevBaudRate->SetText(CSTR("?"));
				me->txtInputType->SetText(CSTR("?"));
				me->txtChecksum->SetText(CSTR("?"));
				me->txtModbusProtocol->SetText(CSTR("?"));
				me->txtDataFormat->SetText(CSTR("?"));
				me->txtIntegrationTime->SetText(CSTR("?"));
				me->txtSlewRate->SetText(CSTR("?"));
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRADAMForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRADAMForm *me = (SSWR::AVIRead::AVIRADAMForm *)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;		
	UOSInt i;
	me->lvData->ClearItems();
	if (me->channelModule == 4050)
	{
		UInt16 inputs;
		UInt16 outputs;
		if (me->channel->ADAM4050GetIOStatus(me->channelAddr, &outputs, &inputs))
		{
			i = me->lvData->AddItem(CSTR("Inputs"), 0);
			sptr = Text::StrHexVal16(sbuff, inputs);
			me->lvData->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			i = me->lvData->AddItem(CSTR("Outputs"), 0);
			sptr = Text::StrHexVal16(sbuff, outputs);
			me->lvData->SetSubItem(i, 1, CSTRP(sbuff, sptr));
		}
		else
		{
			i = me->lvData->AddItem(CSTR("Inputs"), 0);
			me->lvData->SetSubItem(i, 1, CSTR("-"));
			i = me->lvData->AddItem(CSTR("Outputs"), 0);
			me->lvData->SetSubItem(i, 1, CSTR("-"));
		}
	}
	else if (me->channelModule == 4051)
	{
		UInt16 inputs;
		UInt16 outputs;
		if (me->channel->ADAM4051GetIOStatus(me->channelAddr, &outputs, &inputs))
		{
			i = me->lvData->AddItem(CSTR("Inputs"), 0);
			sptr = Text::StrHexVal16(sbuff, inputs);
			me->lvData->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			i = me->lvData->AddItem(CSTR("Outputs"), 0);
			sptr = Text::StrHexVal16(sbuff, outputs);
			me->lvData->SetSubItem(i, 1, CSTRP(sbuff, sptr));
		}
		else
		{
			i = me->lvData->AddItem(CSTR("Inputs"), 0);
			me->lvData->SetSubItem(i, 1, CSTR("-"));
			i = me->lvData->AddItem(CSTR("Outputs"), 0);
			me->lvData->SetSubItem(i, 1, CSTR("-"));
		}
	}
}

void SSWR::AVIRead::AVIRADAMForm::StopStream(Bool clearUI)
{
	if (this->stm)
	{
		DEL_CLASS(this->channel);
		this->stm = 0;
		this->channel = 0;
		this->channelModule = 0;
		if (clearUI)
		{
			this->txtStream->SetText(CSTR("-"));
			this->btnStream->SetText(CSTR("&Open"));
			this->txtAddress->SetReadOnly(false);
		}
	}
}

SSWR::AVIRead::AVIRADAMForm::AVIRADAMForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 576, 480, ui)
{
	this->SetText(CSTR("ADAM device"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->stm = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlMain, UI::GUIPanel(ui, this));
	this->pnlMain->SetRect(0, 0, 100, 316, false);
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->grpStream, UI::GUIGroupBox(ui, this->pnlMain, CSTR("Stream")));
	this->grpStream->SetRect(0, 0, 100, 72, false);
	this->grpStream->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblAddress, UI::GUILabel(ui, this->grpStream, CSTR("Address")));
	this->lblAddress->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtAddress, UI::GUITextBox(ui, this->grpStream, CSTR("6")));
	this->txtAddress->SetRect(104, 4, 200, 23, false);
	NEW_CLASS(this->lblStream, UI::GUILabel(ui, this->grpStream, CSTR("Stream Type")));
	this->lblStream->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtStream, UI::GUITextBox(ui, this->grpStream, CSTR("-")));
	this->txtStream->SetRect(104, 28, 200, 23, false);
	this->txtStream->SetReadOnly(true);
	NEW_CLASS(this->btnStream, UI::GUIButton(ui, this->grpStream, CSTR("&Open")));
	this->btnStream->SetRect(304, 28, 75, 23, false);
	this->btnStream->HandleButtonClick(OnStreamClicked, this);
	NEW_CLASS(this->lblModuleName, UI::GUILabel(ui, this->pnlMain, CSTR("Module Name")));
	this->lblModuleName->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtModuleName, UI::GUITextBox(ui, this->pnlMain, CSTR("")));
	this->txtModuleName->SetRect(104, 76, 200, 23, false);
	this->txtModuleName->SetReadOnly(true);
	NEW_CLASS(this->lblFirmware, UI::GUILabel(ui, this->pnlMain, CSTR("Firmware")));
	this->lblFirmware->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtFirmware, UI::GUITextBox(ui, this->pnlMain, CSTR("")));
	this->txtFirmware->SetRect(104, 100, 200, 23, false);
	this->txtFirmware->SetReadOnly(true);
	NEW_CLASS(this->lblDevAddress, UI::GUILabel(ui, this->pnlMain, CSTR("Address")));
	this->lblDevAddress->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtDevAddress, UI::GUITextBox(ui, this->pnlMain, CSTR("")));
	this->txtDevAddress->SetRect(104, 124, 100, 23, false);
	this->txtDevAddress->SetReadOnly(true);
	NEW_CLASS(this->lblDevBaudRate, UI::GUILabel(ui, this->pnlMain, CSTR("BAud Rate")));
	this->lblDevBaudRate->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtDevBaudRate, UI::GUITextBox(ui, this->pnlMain, CSTR("")));
	this->txtDevBaudRate->SetRect(104, 148, 100, 23, false);
	this->txtDevBaudRate->SetReadOnly(true);
	NEW_CLASS(this->lblInputType, UI::GUILabel(ui, this->pnlMain, CSTR("Input Type")));
	this->lblInputType->SetRect(4, 172, 100, 23, false);
	NEW_CLASS(this->txtInputType, UI::GUITextBox(ui, this->pnlMain, CSTR("")));
	this->txtInputType->SetRect(104, 172, 200, 23, false);
	this->txtInputType->SetReadOnly(true);
	NEW_CLASS(this->lblChecksum, UI::GUILabel(ui, this->pnlMain, CSTR("Checksum")));
	this->lblChecksum->SetRect(4, 196, 100, 23, false);
	NEW_CLASS(this->txtChecksum, UI::GUITextBox(ui, this->pnlMain, CSTR("")));
	this->txtChecksum->SetRect(104, 196, 100, 23, false);
	this->txtChecksum->SetReadOnly(true);
	NEW_CLASS(this->lblModbusProtocol, UI::GUILabel(ui, this->pnlMain, CSTR("Protocol")));
	this->lblModbusProtocol->SetRect(4, 220, 100, 23, false);
	NEW_CLASS(this->txtModbusProtocol, UI::GUITextBox(ui, this->pnlMain, CSTR("")));
	this->txtModbusProtocol->SetRect(104, 220, 100, 23, false);
	this->txtModbusProtocol->SetReadOnly(true);
	NEW_CLASS(this->lblDataFormat, UI::GUILabel(ui, this->pnlMain, CSTR("Data Format")));
	this->lblDataFormat->SetRect(4, 244, 100, 23, false);
	NEW_CLASS(this->txtDataFormat, UI::GUITextBox(ui, this->pnlMain, CSTR("")));
	this->txtDataFormat->SetRect(104, 244, 100, 23, false);
	this->txtDataFormat->SetReadOnly(true);
	NEW_CLASS(this->lblIntegrationTime, UI::GUILabel(ui, this->pnlMain, CSTR("Integration Time")));
	this->lblIntegrationTime->SetRect(4, 268, 100, 23, false);
	NEW_CLASS(this->txtIntegrationTime, UI::GUITextBox(ui, this->pnlMain, CSTR("")));
	this->txtIntegrationTime->SetRect(104, 268, 100, 23, false);
	this->txtIntegrationTime->SetReadOnly(true);
	NEW_CLASS(this->lblSlewRate, UI::GUILabel(ui, this->pnlMain, CSTR("Slew Rate")));
	this->lblSlewRate->SetRect(4, 292, 100, 23, false);
	NEW_CLASS(this->txtSlewRate, UI::GUITextBox(ui, this->pnlMain, CSTR("")));
	this->txtSlewRate->SetRect(104, 292, 100, 23, false);
	this->txtSlewRate->SetReadOnly(true);

	NEW_CLASS(this->lvData, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvData->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvData->SetFullRowSelect(true);
	this->lvData->SetShowGrid(true);
	this->lvData->AddColumn(CSTR("Name"), 100);
	this->lvData->AddColumn(CSTR("Value"), 300);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRADAMForm::~AVIRADAMForm()
{
	this->StopStream(false);
}

void SSWR::AVIRead::AVIRADAMForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
