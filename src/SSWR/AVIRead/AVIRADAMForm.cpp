#include "Stdafx.h"
#include "IO/SerialPort.h"
#include "SSWR/AVIRead/AVIRADAMForm.h"

void __stdcall SSWR::AVIRead::AVIRADAMForm::OnStreamClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRADAMForm> me = userObj.GetNN<SSWR::AVIRead::AVIRADAMForm>();
	if (me->stm.NotNull())
	{
		me->StopStream(true);
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		me->txtAddress->GetText(sb);
		if (!sb.ToUInt8(me->channelAddr))
		{
			me->ui->ShowMsgOK(CSTR("Address out of range (0-255)"), CSTR("ADAM"), me);
			return;
		}
		IO::StreamType st;
		NN<IO::Stream> stm;
		if (me->core->OpenStream(st, me, 0, false).SetTo(stm))
		{
			me->stm = stm;
			UTF8Char sbuff[64];
			UnsafeArray<UTF8Char> sptr;
			NN<IO::AdvantechASCIIChannel> channel;
			NEW_CLASSNN(channel, IO::AdvantechASCIIChannel(stm, true));
			me->channel = channel;
			me->channelModule = 0;
			me->txtStream->SetText(IO::StreamTypeGetName(st));
			me->btnStream->SetText(CSTR("&Close"));
			me->txtAddress->SetReadOnly(true);

			if (channel->GetModuleName(sbuff, me->channelAddr).SetTo(sptr))
			{
				me->txtModuleName->SetText(CSTRP(sbuff, sptr));
				if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("4050")))
				{
					me->channelModule = 4050;
				}
				else if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("4051")))
				{
					me->channelModule = 4051;
				}
			}
			else
			{
				me->txtModuleName->SetText(CSTR("-"));
			}

			if (channel->GetFirmwareVer(sbuff, me->channelAddr).SetTo(sptr))
			{
				me->txtFirmware->SetText(CSTRP(sbuff, sptr));
			}
			else
			{
				me->txtFirmware->SetText(CSTR("-"));
			}

			IO::AdvantechASCIIChannel::ADAMConfig cfg;
			if (channel->GetConfigStatus(me->channelAddr, cfg))
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

void __stdcall SSWR::AVIRead::AVIRADAMForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRADAMForm> me = userObj.GetNN<SSWR::AVIRead::AVIRADAMForm>();
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;		
	UIntOS i;
	NN<IO::AdvantechASCIIChannel> channel;
	me->lvData->ClearItems();
	if (me->channel.SetTo(channel))
	{
		if (me->channelModule == 4050)
		{
			UInt16 inputs;
			UInt16 outputs;
			if (channel->ADAM4050GetIOStatus(me->channelAddr, outputs, inputs))
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
			if (channel->ADAM4051GetIOStatus(me->channelAddr, outputs, inputs))
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
}

void SSWR::AVIRead::AVIRADAMForm::StopStream(Bool clearUI)
{
	if (this->stm.NotNull())
	{
		this->channel.Delete();
		this->stm = nullptr;
		this->channelModule = 0;
		if (clearUI)
		{
			this->txtStream->SetText(CSTR("-"));
			this->btnStream->SetText(CSTR("&Open"));
			this->txtAddress->SetReadOnly(false);
		}
	}
}

SSWR::AVIRead::AVIRADAMForm::AVIRADAMForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 576, 480, ui)
{
	this->SetText(CSTR("ADAM device"));
	this->SetFont(nullptr, 8.25, false);
	
	this->core = core;
	this->stm = nullptr;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlMain = ui->NewPanel(*this);
	this->pnlMain->SetRect(0, 0, 100, 316, false);
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_TOP);
	this->grpStream = ui->NewGroupBox(this->pnlMain, CSTR("Stream"));
	this->grpStream->SetRect(0, 0, 100, 72, false);
	this->grpStream->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblAddress = ui->NewLabel(this->grpStream, CSTR("Address"));
	this->lblAddress->SetRect(4, 4, 100, 23, false);
	this->txtAddress = ui->NewTextBox(this->grpStream, CSTR("6"));
	this->txtAddress->SetRect(104, 4, 200, 23, false);
	this->lblStream = ui->NewLabel(this->grpStream, CSTR("Stream Type"));
	this->lblStream->SetRect(4, 28, 100, 23, false);
	this->txtStream = ui->NewTextBox(this->grpStream, CSTR("-"));
	this->txtStream->SetRect(104, 28, 200, 23, false);
	this->txtStream->SetReadOnly(true);
	this->btnStream = ui->NewButton(this->grpStream, CSTR("&Open"));
	this->btnStream->SetRect(304, 28, 75, 23, false);
	this->btnStream->HandleButtonClick(OnStreamClicked, this);
	this->lblModuleName = ui->NewLabel(this->pnlMain, CSTR("Module Name"));
	this->lblModuleName->SetRect(4, 76, 100, 23, false);
	this->txtModuleName = ui->NewTextBox(this->pnlMain, CSTR(""));
	this->txtModuleName->SetRect(104, 76, 200, 23, false);
	this->txtModuleName->SetReadOnly(true);
	this->lblFirmware = ui->NewLabel(this->pnlMain, CSTR("Firmware"));
	this->lblFirmware->SetRect(4, 100, 100, 23, false);
	this->txtFirmware = ui->NewTextBox(this->pnlMain, CSTR(""));
	this->txtFirmware->SetRect(104, 100, 200, 23, false);
	this->txtFirmware->SetReadOnly(true);
	this->lblDevAddress = ui->NewLabel(this->pnlMain, CSTR("Address"));
	this->lblDevAddress->SetRect(4, 124, 100, 23, false);
	this->txtDevAddress = ui->NewTextBox(this->pnlMain, CSTR(""));
	this->txtDevAddress->SetRect(104, 124, 100, 23, false);
	this->txtDevAddress->SetReadOnly(true);
	this->lblDevBaudRate = ui->NewLabel(this->pnlMain, CSTR("BAud Rate"));
	this->lblDevBaudRate->SetRect(4, 148, 100, 23, false);
	this->txtDevBaudRate = ui->NewTextBox(this->pnlMain, CSTR(""));
	this->txtDevBaudRate->SetRect(104, 148, 100, 23, false);
	this->txtDevBaudRate->SetReadOnly(true);
	this->lblInputType = ui->NewLabel(this->pnlMain, CSTR("Input Type"));
	this->lblInputType->SetRect(4, 172, 100, 23, false);
	this->txtInputType = ui->NewTextBox(this->pnlMain, CSTR(""));
	this->txtInputType->SetRect(104, 172, 200, 23, false);
	this->txtInputType->SetReadOnly(true);
	this->lblChecksum = ui->NewLabel(this->pnlMain, CSTR("Checksum"));
	this->lblChecksum->SetRect(4, 196, 100, 23, false);
	this->txtChecksum = ui->NewTextBox(this->pnlMain, CSTR(""));
	this->txtChecksum->SetRect(104, 196, 100, 23, false);
	this->txtChecksum->SetReadOnly(true);
	this->lblModbusProtocol = ui->NewLabel(this->pnlMain, CSTR("Protocol"));
	this->lblModbusProtocol->SetRect(4, 220, 100, 23, false);
	this->txtModbusProtocol = ui->NewTextBox(this->pnlMain, CSTR(""));
	this->txtModbusProtocol->SetRect(104, 220, 100, 23, false);
	this->txtModbusProtocol->SetReadOnly(true);
	this->lblDataFormat = ui->NewLabel(this->pnlMain, CSTR("Data Format"));
	this->lblDataFormat->SetRect(4, 244, 100, 23, false);
	this->txtDataFormat = ui->NewTextBox(this->pnlMain, CSTR(""));
	this->txtDataFormat->SetRect(104, 244, 100, 23, false);
	this->txtDataFormat->SetReadOnly(true);
	this->lblIntegrationTime = ui->NewLabel(this->pnlMain, CSTR("Integration Time"));
	this->lblIntegrationTime->SetRect(4, 268, 100, 23, false);
	this->txtIntegrationTime = ui->NewTextBox(this->pnlMain, CSTR(""));
	this->txtIntegrationTime->SetRect(104, 268, 100, 23, false);
	this->txtIntegrationTime->SetReadOnly(true);
	this->lblSlewRate = ui->NewLabel(this->pnlMain, CSTR("Slew Rate"));
	this->lblSlewRate->SetRect(4, 292, 100, 23, false);
	this->txtSlewRate = ui->NewTextBox(this->pnlMain, CSTR(""));
	this->txtSlewRate->SetRect(104, 292, 100, 23, false);
	this->txtSlewRate->SetReadOnly(true);

	this->lvData = ui->NewListView(*this, UI::ListViewStyle::Table, 2);
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
