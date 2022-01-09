#include "Stdafx.h"
#include "IO/SerialPort.h"
#include "IO/StreamLogger.h"
#include "SSWR/AVIRead/AVIRTVControlForm.h"
#include "UI/MessageDialog.h"

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
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please select a port", (const UTF8Char*)"TV Control", me);
		return;
	}

	IO::SerialPort *port;
	IO::TVControl::TVInfo info;
	if (!IO::TVControl::GetTVInfo(tvType, &info))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please select a valid TV Type", (const UTF8Char*)"TV Control", me);
		return;
	}
	NEW_CLASS(port, IO::SerialPort(portNum, info.defBaudRate, IO::SerialPort::PARITY_NONE, false));
	if (port->IsError())
	{
		DEL_CLASS(port);
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in opening the port", (const UTF8Char*)"TV Control", me);
		return;
	}
	if (me->chkLogFile->IsChecked())
	{
		NEW_CLASS(me->port, IO::StreamLogger(port, true, (const UTF8Char*)"TVRecv.dat", (const UTF8Char*)"TVSend.dat"));
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
		Int32 cmdValue;
		CommandInfo *cmdInfo = (CommandInfo*)me->cboCommand->GetSelectedItem();
		sb.AppendC(UTF8STRC("Sending "));
		sb.Append(IO::TVControl::GetCommandName(cmdInfo->cmdType));
		me->log->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_RAW);
		if (cmdInfo->cmdFmt == IO::TVControl::CF_INSTRUCTION)
		{
			if (me->tvCtrl->SendInstruction(cmdInfo->cmdType))
			{
				sb.ClearStr();
				sb.Append(IO::TVControl::GetCommandName(cmdInfo->cmdType));
				sb.AppendC(UTF8STRC(" success"));
				me->log->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_ACTION);
				me->txtCommand->SetText((const UTF8Char*)"Success");
			}
			else
			{
				sb.ClearStr();
				sb.Append(IO::TVControl::GetCommandName(cmdInfo->cmdType));
				sb.AppendC(UTF8STRC(" failed"));
				me->log->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_ERROR);
				me->txtCommand->SetText((const UTF8Char*)"Failed");
			}
		}
		else if (cmdInfo->cmdFmt == IO::TVControl::CF_GETCOMMAND)
		{
			if (me->tvCtrl->SendGetCommand(cmdInfo->cmdType, &cmdValue, sbuff))
			{
				sb.ClearStr();
				sb.Append(IO::TVControl::GetCommandName(cmdInfo->cmdType));
				sb.AppendC(UTF8STRC(" success, reply = "));
				sb.Append(sbuff);
				me->log->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_ACTION);
				me->txtCommand->SetText(sbuff);
			}
			else
			{
				sb.ClearStr();
				sb.Append(IO::TVControl::GetCommandName(cmdInfo->cmdType));
				sb.AppendC(UTF8STRC(" failed"));
				me->log->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_ERROR);
				me->txtCommand->SetText((const UTF8Char*)"Failed");
			}
		}
		else if (cmdInfo->cmdFmt == IO::TVControl::CF_SETCOMMAND)
		{
			Int32 val;
			sb.ClearStr();
			me->txtCommand->GetText(&sb);
			if (sb.ToInt32(&val))
			{
				if (me->tvCtrl->SendSetCommand(cmdInfo->cmdType, val))
				{
					sb.ClearStr();
					sb.Append(IO::TVControl::GetCommandName(cmdInfo->cmdType));
					sb.AppendC(UTF8STRC(" success, value = "));
					sb.AppendI32(val);
					me->log->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_ACTION);
					me->txtCommand->SetText(sb.ToString());
				}
				else
				{
					sb.ClearStr();
					sb.Append(IO::TVControl::GetCommandName(cmdInfo->cmdType));
					sb.AppendC(UTF8STRC(" failed"));
					me->log->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_ERROR);
					me->txtCommand->SetText((const UTF8Char*)"Failed");
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

SSWR::AVIRead::AVIRTVControlForm::AVIRTVControlForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 640, 480, ui)
{
	UTF8Char sbuff[32];
	UOSInt i;
	UOSInt j;
	Data::ArrayList<IO::TVControl::TVType> tvTypes;
	IO::TVControl::TVInfo tvInfo;

	this->SetText((const UTF8Char*)"TV Control");
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->port = 0;
	this->tvCtrl = 0;
	this->cmdInfos = 0;
	NEW_CLASS(this->log, IO::LogTool());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlPort, UI::GUIPanel(ui, this));
	this->pnlPort->SetRect(0, 0, 100, 64, false);
	this->pnlPort->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblTVType, UI::GUILabel(ui, this->pnlPort, (const UTF8Char*)"TV Type"));
	this->lblTVType->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->cboTVType, UI::GUIComboBox(ui, this->pnlPort, false));
	this->cboTVType->SetRect(108, 8,  300, 23, false);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->pnlPort, (const UTF8Char*)"Port"));
	this->lblPort->SetRect(8, 32, 100, 23, false);
	NEW_CLASS(this->cboPort, UI::GUIComboBox(ui, this->pnlPort, false));
	this->cboPort->SetRect(108, 32, 100, 23, false);
	NEW_CLASS(this->chkLogFile, UI::GUICheckBox(ui, this->pnlPort, (const UTF8Char*)"Log To File", false));
	this->chkLogFile->SetRect(208, 32, 100, 23, false);

	IO::TVControl::GetTVList(&tvTypes);
	i = 0;
	j = tvTypes.GetCount();
	while (i < j)
	{
		if (IO::TVControl::GetTVInfo(tvTypes.GetItem(i), &tvInfo))
		{
			this->cboTVType->AddItem(tvInfo.name, (void*)(OSInt)tvInfo.tvType);
		}
		i++;
	}
	if (j > 0)
	{
		this->cboTVType->SetSelectedIndex(0);
	}
	
	Data::ArrayList<UOSInt> *ports;
	NEW_CLASS(ports, Data::ArrayList<UOSInt>());
	IO::SerialPort::GetAvailablePorts(ports, 0);
	i = 0;
	j = ports->GetCount();
	while (i < j)
	{
		Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("COM")), ports->GetItem(i));
		this->cboPort->AddItem(sbuff, (void*)ports->GetItem(i));
		i++;
	}
	if (j > 0)
	{
		this->cboPort->SetSelectedIndex(0);
	}
	DEL_CLASS(ports);

	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->pnlPort, (const UTF8Char*)"&Start"));
	this->btnStart->SetRect(308, 32, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClick, this);

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpControl = this->tcMain->AddTabPage((const UTF8Char*)"Control");
	this->tpLog = this->tcMain->AddTabPage((const UTF8Char*)"Log");

	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this->tpLog, (const UTF8Char*)""));
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);

	NEW_CLASS(this->lblCommand, UI::GUILabel(ui, this->tpControl, (const UTF8Char*)"Command"));
	this->lblCommand->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->cboCommand, UI::GUIComboBox(ui, this->tpControl, false));
	this->cboCommand->SetRect(108, 8, 150, 23, false);
	this->cboCommand->HandleSelectionChange(OnCmdChanged, this);
	NEW_CLASS(this->btnCommand, UI::GUIButton(ui, this->tpControl, (const UTF8Char*)"Send"));
	this->btnCommand->SetRect(258, 8, 75, 23, false);
	this->btnCommand->HandleButtonClick(OnSendCommandClicked, this);
	NEW_CLASS(this->txtCommand, UI::GUITextBox(ui, this->tpControl, (const UTF8Char*)""));
	this->txtCommand->SetRect(338, 8, 120, 23, false);
	this->txtCommand->SetReadOnly(true);

	NEW_CLASS(this->logger, UI::ListBoxLogger(this, this->lbLog, 300, true));
	this->log->AddLogHandler(this->logger, IO::ILogHandler::LOG_LEVEL_RAW);
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
	DEL_CLASS(this->log);
	DEL_CLASS(this->logger);
}

void SSWR::AVIRead::AVIRTVControlForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
