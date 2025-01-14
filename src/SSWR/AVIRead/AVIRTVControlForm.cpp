#include "Stdafx.h"
#include "IO/SerialPort.h"
#include "IO/StreamLogger.h"
#include "SSWR/AVIRead/AVIRTVControlForm.h"

void __stdcall SSWR::AVIRead::AVIRTVControlForm::OnStartClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTVControlForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTVControlForm>();
	NN<IO::Stream> port;
	UnsafeArray<CommandInfo> cmdInfos;
	if (me->port.SetTo(port) && me->cmdInfos.SetTo(cmdInfos))
	{
		port->Close();
		me->tvCtrl.Delete();
		me->port.Delete();
		MemFreeNN(cmdInfos);
		me->cmdInfos = 0;

		me->cboPort->SetEnabled(true);
		me->cboTVType->SetEnabled(true);
		me->cboCommand->ClearItems();
		return;
	}
	IO::TVControl::TVType tvType = (IO::TVControl::TVType)me->cboTVType->GetSelectedItem().GetOSInt();
	UOSInt i = me->cboPort->GetSelectedIndex();
	UInt32 portNum = (UInt32)me->cboPort->GetItem(i).GetUOSInt();
	if (portNum == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please select a port"), CSTR("TV Control"), me);
		return;
	}

	IO::TVControl::TVInfo info;
	if (!IO::TVControl::GetTVInfo(tvType, &info))
	{
		me->ui->ShowMsgOK(CSTR("Please select a valid TV Type"), CSTR("TV Control"), me);
		return;
	}
	NEW_CLASSNN(port, IO::SerialPort(portNum, info.defBaudRate, IO::SerialPort::PARITY_NONE, false));
	if (NN<IO::SerialPort>::ConvertFrom(port)->IsError())
	{
		port.Delete();
		me->ui->ShowMsgOK(CSTR("Error in opening the port"), CSTR("TV Control"), me);
		return;
	}
	if (me->chkLogFile->IsChecked())
	{
		NEW_CLASSNN(port, IO::StreamLogger(port, true, CSTR("TVRecv.dat"), CSTR("TVSend.dat")));
	}
	me->port = port;
	me->tvCtrl = IO::TVControl::CreateTVControl(port, tvType, 1);
	NN<IO::TVControl> tvCtrl;
	if (!me->tvCtrl.SetTo(tvCtrl))
	{
		port.Delete();
		me->ui->ShowMsgOK(CSTR("Error in creating TV Control"), CSTR("TV Control"), me);
		return;
	}

	me->cboPort->SetEnabled(false);
	me->cboTVType->SetEnabled(false);
	me->cboCommand->ClearItems();

	Data::ArrayList<IO::TVControl::CommandType> cmdList;
	Data::ArrayList<IO::TVControl::CommandFormat> cmdFormats;
	UOSInt j;
	tvCtrl->GetSupportedCmd(&cmdList, &cmdFormats);
	i = 0;
	j = cmdList.GetCount();
	me->cmdInfos = cmdInfos = MemAllocArr(CommandInfo, j);
	while (i < j)
	{
		cmdInfos[i].cmdType = cmdList.GetItem(i);
		cmdInfos[i].cmdFmt = cmdFormats.GetItem(i);
		me->cboCommand->AddItem(IO::TVControl::GetCommandName(cmdList.GetItem(i)), &cmdInfos[i]);
		i++;
	}
	if (j > 0)
	{
		me->cboCommand->SetSelectedIndex(0);
	}

	
}

void __stdcall SSWR::AVIRead::AVIRTVControlForm::OnSendCommandClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTVControlForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTVControlForm>();
	NN<IO::TVControl> tvCtrl;
	if (me->tvCtrl.SetTo(tvCtrl))
	{
		Text::StringBuilderUTF8 sb;
		UTF8Char sbuff[32];
		UnsafeArray<UTF8Char> sptr;
		Int32 cmdValue;
		NN<CommandInfo> cmdInfo;
		if (!me->cboCommand->GetSelectedItem().GetOpt<CommandInfo>().SetTo(cmdInfo))
			return;
		sb.AppendC(UTF8STRC("Sending "));
		sb.Append(IO::TVControl::GetCommandName(cmdInfo->cmdType));
		me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
		if (cmdInfo->cmdFmt == IO::TVControl::CF_INSTRUCTION)
		{
			if (tvCtrl->SendInstruction(cmdInfo->cmdType))
			{
				sb.ClearStr();
				sb.Append(IO::TVControl::GetCommandName(cmdInfo->cmdType));
				sb.AppendC(UTF8STRC(" success"));
				me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
				me->txtCommand->SetText(CSTR("Success"));
			}
			else
			{
				sb.ClearStr();
				sb.Append(IO::TVControl::GetCommandName(cmdInfo->cmdType));
				sb.AppendC(UTF8STRC(" failed"));
				me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
				me->txtCommand->SetText(CSTR("Failed"));
			}
		}
		else if (cmdInfo->cmdFmt == IO::TVControl::CF_GETCOMMAND)
		{
			if (tvCtrl->SendGetCommand(cmdInfo->cmdType, &cmdValue, sbuff))
			{
				sptr = &sbuff[Text::StrCharCnt(sbuff)];
				sb.ClearStr();
				sb.Append(IO::TVControl::GetCommandName(cmdInfo->cmdType));
				sb.AppendC(UTF8STRC(" success, reply = "));
				sb.AppendP(sbuff, sptr);
				me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
				me->txtCommand->SetText(CSTRP(sbuff, sptr));
			}
			else
			{
				sb.ClearStr();
				sb.Append(IO::TVControl::GetCommandName(cmdInfo->cmdType));
				sb.AppendC(UTF8STRC(" failed"));
				me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
				me->txtCommand->SetText(CSTR("Failed"));
			}
		}
		else if (cmdInfo->cmdFmt == IO::TVControl::CF_SETCOMMAND)
		{
			Int32 val;
			sb.ClearStr();
			me->txtCommand->GetText(sb);
			if (sb.ToInt32(val))
			{
				if (tvCtrl->SendSetCommand(cmdInfo->cmdType, val))
				{
					sb.ClearStr();
					sb.Append(IO::TVControl::GetCommandName(cmdInfo->cmdType));
					sb.AppendC(UTF8STRC(" success, value = "));
					sb.AppendI32(val);
					me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
					me->txtCommand->SetText(sb.ToCString());
				}
				else
				{
					sb.ClearStr();
					sb.Append(IO::TVControl::GetCommandName(cmdInfo->cmdType));
					sb.AppendC(UTF8STRC(" failed"));
					me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
					me->txtCommand->SetText(CSTR("Failed"));
				}
				me->txtCommand->SetReadOnly(true);
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRTVControlForm::OnCmdChanged(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTVControlForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTVControlForm>();
	NN<IO::TVControl> tvCtrl;
	if (me->tvCtrl.SetTo(tvCtrl))
	{
		NN<CommandInfo> cmdInfo;
		if (!me->cboCommand->GetSelectedItem().GetOpt<CommandInfo>().SetTo(cmdInfo))
		{
			me->txtCommand->SetReadOnly(true);
		}
		else if (cmdInfo->cmdFmt == IO::TVControl::CF_SETCOMMAND)
		{
			me->txtCommand->SetReadOnly(false);
		}
		else
		{
			me->txtCommand->SetReadOnly(true);
		}
	}
}

SSWR::AVIRead::AVIRTVControlForm::AVIRTVControlForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 480, ui)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	Data::ArrayList<IO::TVControl::TVType> tvTypes;
	IO::TVControl::TVInfo tvInfo;

	this->SetText(CSTR("TV Control"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->port = 0;
	this->tvCtrl = 0;
	this->cmdInfos = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlPort = ui->NewPanel(*this);
	this->pnlPort->SetRect(0, 0, 100, 64, false);
	this->pnlPort->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblTVType = ui->NewLabel(this->pnlPort, CSTR("TV Type"));
	this->lblTVType->SetRect(8, 8, 100, 23, false);
	this->cboTVType = ui->NewComboBox(this->pnlPort, false);
	this->cboTVType->SetRect(108, 8,  300, 23, false);
	this->lblPort = ui->NewLabel(this->pnlPort, CSTR("Port"));
	this->lblPort->SetRect(8, 32, 100, 23, false);
	this->cboPort = ui->NewComboBox(this->pnlPort, false);
	this->cboPort->SetRect(108, 32, 100, 23, false);
	this->chkLogFile = ui->NewCheckBox(this->pnlPort, CSTR("Log To File"), false);
	this->chkLogFile->SetRect(208, 32, 100, 23, false);

	IO::TVControl::GetTVList(&tvTypes);
	i = 0;
	j = tvTypes.GetCount();
	while (i < j)
	{
		if (IO::TVControl::GetTVInfo(tvTypes.GetItem(i), &tvInfo))
		{
			this->cboTVType->AddItem({tvInfo.name, tvInfo.nameLen}, (void*)(OSInt)tvInfo.tvType);
		}
		i++;
	}
	if (j > 0)
	{
		this->cboTVType->SetSelectedIndex(0);
	}
	
	{
		Data::ArrayList<UOSInt> ports;
		IO::SerialPort::GetAvailablePorts(ports, 0);
		i = 0;
		j = ports.GetCount();
		while (i < j)
		{
			sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("COM")), ports.GetItem(i));
			this->cboPort->AddItem(CSTRP(sbuff, sptr), (void*)ports.GetItem(i));
			i++;
		}
		if (j > 0)
		{
			this->cboPort->SetSelectedIndex(0);
		}
	}

	this->btnStart = ui->NewButton(this->pnlPort, CSTR("&Start"));
	this->btnStart->SetRect(308, 32, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClick, this);

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpControl = this->tcMain->AddTabPage(CSTR("Control"));
	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));

	this->txtLog = ui->NewTextBox(this->tpLog, CSTR(""));
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lbLog = ui->NewListBox(this->tpLog, false);
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);

	this->lblCommand = ui->NewLabel(this->tpControl, CSTR("Command"));
	this->lblCommand->SetRect(8, 8, 100, 23, false);
	this->cboCommand = ui->NewComboBox(this->tpControl, false);
	this->cboCommand->SetRect(108, 8, 150, 23, false);
	this->cboCommand->HandleSelectionChange(OnCmdChanged, this);
	this->btnCommand = ui->NewButton(this->tpControl, CSTR("Send"));
	this->btnCommand->SetRect(258, 8, 75, 23, false);
	this->btnCommand->HandleButtonClick(OnSendCommandClicked, this);
	this->txtCommand = ui->NewTextBox(this->tpControl, CSTR(""));
	this->txtCommand->SetRect(338, 8, 120, 23, false);
	this->txtCommand->SetReadOnly(true);

	NEW_CLASSNN(this->logger, UI::ListBoxLogger(*this, this->lbLog, 300, true));
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Raw);
}

SSWR::AVIRead::AVIRTVControlForm::~AVIRTVControlForm()
{
	NN<IO::Stream> port;
	UnsafeArray<CommandInfo> cmdInfos;
	if (this->port.SetTo(port) && this->cmdInfos.SetTo(cmdInfos))
	{
		port->Close();
		this->tvCtrl.Delete();
		this->port.Delete();
		MemFreeArr(cmdInfos);
		this->cmdInfos = 0;
	}
	this->log.RemoveLogHandler(this->logger);
	this->logger.Delete();
}

void SSWR::AVIRead::AVIRTVControlForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
