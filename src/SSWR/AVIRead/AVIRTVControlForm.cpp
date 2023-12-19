#include "Stdafx.h"
#include "IO/SerialPort.h"
#include "IO/StreamLogger.h"
#include "SSWR/AVIRead/AVIRTVControlForm.h"

void __stdcall SSWR::AVIRead::AVIRTVControlForm::OnStartClick(void *userObj)
{
	SSWR::AVIRead::AVIRTVControlForm *me = (SSWR::AVIRead::AVIRTVControlForm*)userObj;
	if (me->port)
	{
		me->port->Close();
		DEL_CLASS(me->tvCtrl);
		DEL_CLASS(me->port);
		MemFree(me->cmdInfos);
		me->tvCtrl = 0;
		me->port = 0;

		me->cboPort->SetEnabled(true);
		me->cboTVType->SetEnabled(true);
		me->cboCommand->ClearItems();
		return;
	}
	IO::TVControl::TVType tvType = (IO::TVControl::TVType)(OSInt)me->cboTVType->GetSelectedItem();
	UOSInt i = me->cboPort->GetSelectedIndex();
	UInt32 portNum = (UInt32)(UOSInt)me->cboPort->GetItem(i);
	if (portNum == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please select a port"), CSTR("TV Control"), me);
		return;
	}

	IO::SerialPort *port;
	IO::TVControl::TVInfo info;
	if (!IO::TVControl::GetTVInfo(tvType, &info))
	{
		me->ui->ShowMsgOK(CSTR("Please select a valid TV Type"), CSTR("TV Control"), me);
		return;
	}
	NEW_CLASS(port, IO::SerialPort(portNum, info.defBaudRate, IO::SerialPort::PARITY_NONE, false));
	if (port->IsError())
	{
		DEL_CLASS(port);
		me->ui->ShowMsgOK(CSTR("Error in opening the port"), CSTR("TV Control"), me);
		return;
	}
	if (me->chkLogFile->IsChecked())
	{
		NEW_CLASS(me->port, IO::StreamLogger(port, true, CSTR("TVRecv.dat"), CSTR("TVSend.dat")));
	}
	else
	{
		me->port = port;
	}
	me->tvCtrl = IO::TVControl::CreateTVControl(me->port, tvType, 1);
	me->cboPort->SetEnabled(false);
	me->cboTVType->SetEnabled(false);
	me->cboCommand->ClearItems();

	Data::ArrayList<IO::TVControl::CommandType> cmdList;
	Data::ArrayList<IO::TVControl::CommandFormat> cmdFormats;
	UOSInt j;
	me->tvCtrl->GetSupportedCmd(&cmdList, &cmdFormats);
	i = 0;
	j = cmdList.GetCount();
	me->cmdInfos = MemAlloc(CommandInfo, j);
	while (i < j)
	{
		me->cmdInfos[i].cmdType = cmdList.GetItem(i);
		me->cmdInfos[i].cmdFmt = cmdFormats.GetItem(i);
		me->cboCommand->AddItem(IO::TVControl::GetCommandName(cmdList.GetItem(i)), &me->cmdInfos[i]);
		i++;
	}
	if (j > 0)
	{
		me->cboCommand->SetSelectedIndex(0);
	}

	
}

void __stdcall SSWR::AVIRead::AVIRTVControlForm::OnSendCommandClicked(void *userObj)
{
	SSWR::AVIRead::AVIRTVControlForm *me = (SSWR::AVIRead::AVIRTVControlForm*)userObj;
	if (me->tvCtrl)
	{
		Text::StringBuilderUTF8 sb;
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		Int32 cmdValue;
		CommandInfo *cmdInfo = (CommandInfo*)me->cboCommand->GetSelectedItem();
		sb.AppendC(UTF8STRC("Sending "));
		sb.Append(IO::TVControl::GetCommandName(cmdInfo->cmdType));
		me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
		if (cmdInfo->cmdFmt == IO::TVControl::CF_INSTRUCTION)
		{
			if (me->tvCtrl->SendInstruction(cmdInfo->cmdType))
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
			if (me->tvCtrl->SendGetCommand(cmdInfo->cmdType, &cmdValue, sbuff))
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
				if (me->tvCtrl->SendSetCommand(cmdInfo->cmdType, val))
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

void __stdcall SSWR::AVIRead::AVIRTVControlForm::OnCmdChanged(void *userObj)
{
	SSWR::AVIRead::AVIRTVControlForm *me = (SSWR::AVIRead::AVIRTVControlForm*)userObj;
	if (me->tvCtrl)
	{
		CommandInfo *cmdInfo = (CommandInfo*)me->cboCommand->GetSelectedItem();
		if (cmdInfo == 0)
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

SSWR::AVIRead::AVIRTVControlForm::AVIRTVControlForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 480, ui)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
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

	NEW_CLASSNN(this->pnlPort, UI::GUIPanel(ui, *this));
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
	NEW_CLASS(this->chkLogFile, UI::GUICheckBox(ui, this->pnlPort, CSTR("Log To File"), false));
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

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, *this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpControl = this->tcMain->AddTabPage(CSTR("Control"));
	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));

	this->txtLog = ui->NewTextBox(this->tpLog, CSTR(""));
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASSNN(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
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
	if (this->port)
	{
		this->port->Close();
		DEL_CLASS(this->tvCtrl);
		DEL_CLASS(this->port);
		MemFree(this->cmdInfos);
		this->tvCtrl = 0;
		this->port = 0;
	}
	this->log.RemoveLogHandler(this->logger);
	this->logger.Delete();
}

void SSWR::AVIRead::AVIRTVControlForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
