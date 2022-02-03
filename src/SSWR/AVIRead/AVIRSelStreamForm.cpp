#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/SerialPort.h"
#include "Net/TCPBoardcastStream.h"
#include "Net/TCPServerStream.h"
#include "SSWR/AVIRead/AVIRSelStreamForm.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRSelStreamForm::OnOKClick(void *userObj)
{
	SSWR::AVIRead::AVIRSelStreamForm *me = (SSWR::AVIRead::AVIRSelStreamForm*)userObj;
	SSWR::AVIRead::AVIRCore::StreamType st = (SSWR::AVIRead::AVIRCore::StreamType)(OSInt)me->cboStreamType->GetSelectedItem();
	UTF8Char sbuff[256];

	if (st == SSWR::AVIRead::AVIRCore::ST_SERIAL_PORT)
	{
		UOSInt i = me->cboSerialPort->GetSelectedIndex();
		UInt32 portNum = (UInt32)(UOSInt)me->cboSerialPort->GetItem(i);
		if (portNum == 0)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Please select a port", (const UTF8Char*)"Select Serial Port", me);
			return;
		}
		me->txtBaudRate->GetText(sbuff);
		UInt32 baudRate = Text::StrToUInt32(sbuff);
		if (baudRate == 0)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Please input a valid baud rate", (const UTF8Char*)"Select Serial Port", me);
			return;
		}
		IO::SerialPort::ParityType parity = (IO::SerialPort::ParityType)(OSInt)me->cboParity->GetSelectedItem();
		IO::SerialPort *port;
		NEW_CLASS(port, IO::SerialPort(portNum, baudRate, parity, false));
		if (port->IsError())
		{
			DEL_CLASS(port);
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in opening the port", (const UTF8Char*)"Select Serial Port", me);
			return;
		}
		me->stm = port;
		me->stmType = st;
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
	else if (st == SSWR::AVIRead::AVIRCore::ST_USBXPRESS)
	{
		UInt32 baudRate;
		Text::StringBuilderUTF8 sb;
		me->txtSLBaudRate->GetText(&sb);
		if (!sb.ToUInt32(&baudRate) || baudRate == 0)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Please input baud rate", (const UTF8Char*)"Error", me);
			return;
		}
		me->stm = me->siLabDriver->OpenPort((UInt32)(OSInt)me->lvSLPort->GetSelectedItem(), baudRate);
		if (me->stm)
		{
			me->stmType = st;
			me->SetDialogResult(UI::GUIForm::DR_OK);
		}
		else
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in opening the port", (const UTF8Char*)"Error", me);
		}
	}
	else if (st == SSWR::AVIRead::AVIRCore::ST_TCPSERVER)
	{
		Text::StringBuilderUTF8 sb;
		UInt16 port;
		me->txtTCPSvrPort->GetText(&sb);
		if (!sb.ToUInt16(&port))
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Port is not a number", (const UTF8Char*)"Error", me);
			return;
		}
		if (port <= 0 || port >= 65535)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Port is out of range", (const UTF8Char*)"Error", me);
			return;
		}
		if (me->chkBoardcast->IsChecked())
		{
			Net::TCPBoardcastStream *stm;
			NEW_CLASS(stm, Net::TCPBoardcastStream(me->core->GetSocketFactory(), port, 0));
			if (stm->IsError())
			{
				DEL_CLASS(stm);
				UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in listening to the port", (const UTF8Char*)"Error", me);
				return;
			}
			else
			{
				me->stm = stm;
				me->stmType = st;
				me->SetDialogResult(UI::GUIForm::DR_OK);
			}
		}
		else
		{
			Net::TCPServerStream *stm;
			NEW_CLASS(stm, Net::TCPServerStream(me->core->GetSocketFactory(), port, 0));
			if (stm->IsError())
			{
				DEL_CLASS(stm);
				UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in listening to the port", (const UTF8Char*)"Error", me);
				return;
			}
			else
			{
				me->stm = stm;
				me->stmType = st;
				me->SetDialogResult(UI::GUIForm::DR_OK);
			}
		}
	}
	else if (st == SSWR::AVIRead::AVIRCore::ST_TCPCLIENT)
	{
		Text::StringBuilderUTF8 sb;
		Net::SocketUtil::AddressInfo addr;
		UInt16 port;
		me->txtTCPCliHost->GetText(&sb);
		if (!me->core->GetSocketFactory()->DNSResolveIP(sb.ToString(), sb.GetLength(), &addr))
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Host is not valid", (const UTF8Char*)"Error", me);
			return;
		}
		sb.ClearStr();
		me->txtTCPCliPort->GetText(&sb);
		if (!sb.ToUInt16(&port))
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Port is not a number", (const UTF8Char*)"Error", me);
			return;
		}
		if (port <= 0 || port > 65535)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Port is out of range", (const UTF8Char*)"Error", me);
			return;
		}
		Net::TCPClient *cli;
		NEW_CLASS(cli, Net::TCPClient(me->core->GetSocketFactory(), &addr, port));
		if (cli->IsConnectError())
		{
			DEL_CLASS(cli);
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in connect to server", (const UTF8Char*)"Error", me);
			return;
		}
		else
		{
			me->stm = cli;
			me->stmType = st;
			me->SetDialogResult(UI::GUIForm::DR_OK);
		}
	}
	else if (st == SSWR::AVIRead::AVIRCore::ST_FILE)
	{
		Text::StringBuilderUTF8 sb;
		IO::FileStream *fs;
		me->txtFileName->GetText(&sb);
		if (sb.GetLength() > 0)
		{
			NEW_CLASS(fs, IO::FileStream(sb.ToString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
			if (fs->IsError())
			{
				DEL_CLASS(fs);
				UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in opening the file", (const UTF8Char*)"Open Stream", me);
			}
			else
			{
				me->stm = fs;
				me->stmType = st;
				me->SetDialogResult(UI::GUIForm::DR_OK);
			}
		}
	}
	else if (st == SSWR::AVIRead::AVIRCore::ST_HID)
	{
		IO::DeviceInfo *dev = (IO::DeviceInfo*)me->lbHIDDevice->GetSelectedItem();
		if (dev)
		{
			me->stm = dev->CreateStream();
			if (me->stm)
			{
				me->stmType = st;
				me->SetDialogResult(UI::GUIForm::DR_OK);
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRSelStreamForm::OnCancelClick(void *userObj)
{
	SSWR::AVIRead::AVIRSelStreamForm *me = (SSWR::AVIRead::AVIRSelStreamForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRSelStreamForm::OnFileBrowseClick(void *userObj)
{
	SSWR::AVIRead::AVIRSelStreamForm *me = (SSWR::AVIRead::AVIRSelStreamForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UI::FileDialog *ofd;
	me->txtFileName->GetText(&sb);
	NEW_CLASS(ofd, UI::FileDialog(L"SSWR", L"AVIRead", L"OpenStreamFile", false));
	ofd->SetAllowMultiSel(false);
	if (sb.GetLength() > 0)
	{
		ofd->SetFileName(sb.ToString());
	}
	if (ofd->ShowDialog(me->GetHandle()) == UI::GUIForm::DR_OK)
	{
		me->txtFileName->SetText(ofd->GetFileName()->v);
	}
	DEL_CLASS(ofd);
}

void __stdcall SSWR::AVIRead::AVIRSelStreamForm::OnStmTypeChg(void *userObj)
{
	SSWR::AVIRead::AVIRSelStreamForm *me = (SSWR::AVIRead::AVIRSelStreamForm*)userObj;
	UOSInt i = me->cboStreamType->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		SSWR::AVIRead::AVIRCore::StreamType st = (SSWR::AVIRead::AVIRCore::StreamType)(OSInt)me->cboStreamType->GetItem(i);
		if (st == SSWR::AVIRead::AVIRCore::ST_SERIAL_PORT)
		{
			me->tcConfig->SetSelectedPage(me->tpSerialPort);
		}
		else if (st == SSWR::AVIRead::AVIRCore::ST_USBXPRESS)
		{
			me->tcConfig->SetSelectedPage(me->tpSiLabPort);
		}
		else if (st == SSWR::AVIRead::AVIRCore::ST_TCPSERVER)
		{
			me->tcConfig->SetSelectedPage(me->tpTCPSvr);
		}
		else if (st == SSWR::AVIRead::AVIRCore::ST_TCPCLIENT)
		{
			me->tcConfig->SetSelectedPage(me->tpTCPCli);
		}
		else if (st == SSWR::AVIRead::AVIRCore::ST_FILE)
		{
			me->tcConfig->SetSelectedPage(me->tpFile);
		}
		else if (st == SSWR::AVIRead::AVIRCore::ST_HID)
		{
			me->tcConfig->SetSelectedPage(me->tpHID);
		}
	}
}

SSWR::AVIRead::AVIRSelStreamForm::AVIRSelStreamForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Bool allowReadOnly) : UI::GUIForm(parent, 640, 300, ui)
{
	UTF8Char sbuff[32];
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UTF8Char *sptr;

	this->SetText((const UTF8Char*)"Select Stream");
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->siLabDriver = this->core->GetSiLabDriver();
	NEW_CLASS(this->devMgr, IO::DeviceManager());
	NEW_CLASS(this->devList, Data::ArrayList<IO::DeviceInfo*>());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlStreamType, UI::GUIPanel(ui, this));
	this->pnlStreamType->SetRect(0, 0, 100, 31, false);
	this->pnlStreamType->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblStreamType, UI::GUILabel(ui, this->pnlStreamType, (const UTF8Char*)"Stream Type"));
	this->lblStreamType->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboStreamType, UI::GUIComboBox(ui, this->pnlStreamType, false));
	this->cboStreamType->SetRect(104, 4, 200, 23, false);
	this->cboStreamType->AddItem(SSWR::AVIRead::AVIRCore::GetStreamTypeName(SSWR::AVIRead::AVIRCore::ST_SERIAL_PORT).v, (void*)SSWR::AVIRead::AVIRCore::ST_SERIAL_PORT);
	if (this->siLabDriver)
	{
		this->cboStreamType->AddItem(SSWR::AVIRead::AVIRCore::GetStreamTypeName(SSWR::AVIRead::AVIRCore::ST_USBXPRESS).v, (void*)SSWR::AVIRead::AVIRCore::ST_USBXPRESS);
	}
	this->cboStreamType->AddItem(SSWR::AVIRead::AVIRCore::GetStreamTypeName(SSWR::AVIRead::AVIRCore::ST_TCPSERVER).v, (void*)SSWR::AVIRead::AVIRCore::ST_TCPSERVER);
	this->cboStreamType->AddItem(SSWR::AVIRead::AVIRCore::GetStreamTypeName(SSWR::AVIRead::AVIRCore::ST_TCPCLIENT).v, (void*)SSWR::AVIRead::AVIRCore::ST_TCPCLIENT);
	if (allowReadOnly)
	{
		this->cboStreamType->AddItem(SSWR::AVIRead::AVIRCore::GetStreamTypeName(SSWR::AVIRead::AVIRCore::ST_FILE).v, (void*)SSWR::AVIRead::AVIRCore::ST_FILE);
	}
	this->cboStreamType->AddItem(SSWR::AVIRead::AVIRCore::GetStreamTypeName(SSWR::AVIRead::AVIRCore::ST_HID).v, (void*)SSWR::AVIRead::AVIRCore::ST_HID);
	this->cboStreamType->HandleSelectionChange(OnStmTypeChg, this);

	NEW_CLASS(this->pnlButtons, UI::GUIPanel(ui, this));
	this->pnlButtons->SetRect(0, 0, 100, 31, false);
	this->pnlButtons->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this->pnlButtons, (const UTF8Char*)"&OK"));
	this->btnOK->SetRect(49, 4, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClick, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this->pnlButtons, (const UTF8Char*)"&Cancel"));
	this->btnCancel->SetRect(132, 4, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClick, this);

	NEW_CLASS(this->tcConfig, UI::GUITabControl(ui, this));
	this->tcConfig->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpSerialPort = this->tcConfig->AddTabPage((const UTF8Char*)"Serial Port");
	NEW_CLASS(this->lblSerialPort, UI::GUILabel(ui, this->tpSerialPort, (const UTF8Char*)"Port"));
	this->lblSerialPort->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->cboSerialPort, UI::GUIComboBox(ui, this->tpSerialPort, false));
	this->cboSerialPort->SetRect(108, 8, 100, 23, false);

	Data::ArrayList<UOSInt> *ports;
	NEW_CLASS(ports, Data::ArrayList<UOSInt>());
	Data::ArrayList<IO::SerialPort::SerialPortType> portTypeList;
	IO::SerialPort::GetAvailablePorts(ports, &portTypeList);
	UOSInt currPort;
	i = 0;
	j = ports->GetCount();
	while (i < j)
	{
		currPort = ports->GetItem(i);
		sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("COM")), currPort);
		sptr = Text::StrConcatC(sptr, UTF8STRC(" ("));
		sptr = IO::SerialPort::GetPortTypeName(portTypeList.GetItem(i)).ConcatTo(sptr);
		sptr = Text::StrConcatC(sptr, UTF8STRC(")"));
		this->cboSerialPort->AddItem(sbuff, (void*)(OSInt)currPort);
		i++;
	}
	if (j > 0)
	{
		this->cboSerialPort->SetSelectedIndex(0);
	}
	DEL_CLASS(ports);
	NEW_CLASS(this->lblBaudRate, UI::GUILabel(ui, this->tpSerialPort, (const UTF8Char*)"Baud Rate"));
	this->lblBaudRate->SetRect(8, 32, 100, 23, false);
	NEW_CLASS(this->txtBaudRate, UI::GUITextBox(ui, this->tpSerialPort, (const UTF8Char*)"115200"));
	this->txtBaudRate->SetRect(108, 32, 100, 23, false);
	NEW_CLASS(this->lblParity, UI::GUILabel(ui, this->tpSerialPort, (const UTF8Char*)"Parity"));
	this->lblParity->SetRect(8, 56, 100, 23, false);
	NEW_CLASS(this->cboParity, UI::GUIComboBox(ui, this->tpSerialPort, false));
	this->cboParity->SetRect(108, 56, 100, 23, false);
	this->cboParity->AddItem((const UTF8Char*)"None", (void*)(OSInt)IO::SerialPort::PARITY_NONE);
	this->cboParity->AddItem((const UTF8Char*)"Odd", (void*)(OSInt)IO::SerialPort::PARITY_ODD);
	this->cboParity->AddItem((const UTF8Char*)"Even", (void*)(OSInt)IO::SerialPort::PARITY_EVEN);
	this->cboParity->SetSelectedIndex(0);

	if (this->siLabDriver)
	{
		this->tpSiLabPort = this->tcConfig->AddTabPage((const UTF8Char*)"SiLab");
		NEW_CLASS(this->pnlSLInfo, UI::GUIPanel(ui, this->tpSiLabPort));
		this->pnlSLInfo->SetRect(0, 0, 100, 55, false);
		this->pnlSLInfo->SetDockType(UI::GUIControl::DOCK_TOP);
		NEW_CLASS(this->lblDLLVer, UI::GUILabel(ui, this->pnlSLInfo, (const UTF8Char*)"DLL Version"));
		this->lblDLLVer->SetRect(4, 4, 100, 23, false);
		NEW_CLASS(this->txtDLLVer, UI::GUITextBox(ui, this->pnlSLInfo, (const UTF8Char*)""));
		this->txtDLLVer->SetRect(104, 4, 100, 23, false);
		this->txtDLLVer->SetReadOnly(true);
		NEW_CLASS(this->lblDriverVer, UI::GUILabel(ui, this->pnlSLInfo, (const UTF8Char*)"Driver Version"));
		this->lblDriverVer->SetRect(4, 28, 100, 23, false);
		NEW_CLASS(this->txtDriverVer, UI::GUITextBox(ui, this->pnlSLInfo, (const UTF8Char*)""));
		this->txtDriverVer->SetRect(104, 28, 100, 23, false);
		this->txtDriverVer->SetReadOnly(true);

		NEW_CLASS(this->pnlSLControl, UI::GUIPanel(ui, this->tpSiLabPort));
		this->pnlSLControl->SetRect(0, 0, 100, 67, false);
		this->pnlSLControl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
		NEW_CLASS(this->lblSLBaudRate, UI::GUILabel(ui, this->pnlSLControl, (const UTF8Char*)"Baud Rate"));
		this->lblSLBaudRate->SetRect(4, 4, 100, 23, false);
		NEW_CLASS(this->txtSLBaudRate, UI::GUITextBox(ui, this->pnlSLControl, (const UTF8Char*)"115200"));
		this->txtSLBaudRate->SetRect(104, 4, 100, 23, false);

		NEW_CLASS(this->lvSLPort, UI::GUIListView(ui, this->tpSiLabPort, UI::GUIListView::LVSTYLE_TABLE, 5));
		this->lvSLPort->SetDockType(UI::GUIControl::DOCK_FILL);
		this->lvSLPort->AddColumn((const UTF8Char*)"Num", 40);
		this->lvSLPort->AddColumn((const UTF8Char*)"VID", 60);
		this->lvSLPort->AddColumn((const UTF8Char*)"PID", 60);
		this->lvSLPort->AddColumn((const UTF8Char*)"SN", 100);
		this->lvSLPort->AddColumn((const UTF8Char*)"Description", 400);
		this->lvSLPort->SetFullRowSelect(true);

		Text::StringBuilderUTF8 sb;
		UInt16 ver1;
		UInt16 ver2;
		UInt16 ver3;
		UInt16 ver4;

		if (this->siLabDriver->GetDLLVersion(&ver1, &ver2, &ver3, &ver4))
		{
			sb.ClearStr();
			sb.AppendU32(ver1);
			sb.AppendC(UTF8STRC("."));
			sb.AppendU32(ver2);
			sb.AppendC(UTF8STRC("."));
			sb.AppendU32(ver3);
			sb.AppendC(UTF8STRC("."));
			sb.AppendU32(ver4);
			this->txtDLLVer->SetText(sb.ToString());
		}
		if (this->siLabDriver->GetDriverVersion(&ver1, &ver2, &ver3, &ver4))
		{
			sb.ClearStr();
			sb.AppendU32(ver1);
			sb.AppendC(UTF8STRC("."));
			sb.AppendU32(ver2);
			sb.AppendC(UTF8STRC("."));
			sb.AppendU32(ver3);
			sb.AppendC(UTF8STRC("."));
			sb.AppendU32(ver4);
			this->txtDriverVer->SetText(sb.ToString());
		}

		j = 0;
		j = this->siLabDriver->GetNumDevices();
		i = 0;
		while (i < j)
		{
			UInt32 v;
			Text::StrUOSInt(sbuff, i);
			k = this->lvSLPort->AddItem(sbuff, (void*)(OSInt)i);
			v = 0;
			if (this->siLabDriver->GetDeviceVID((UInt32)i, &v))
			{
				Text::StrHexVal16(sbuff, (UInt16)v);
				this->lvSLPort->SetSubItem(k, 1, sbuff);
			}
			if (this->siLabDriver->GetDevicePID((UInt32)i, &v))
			{
				Text::StrHexVal16(sbuff, (UInt16)v);
				this->lvSLPort->SetSubItem(k, 2, sbuff);
			}
			if (this->siLabDriver->GetDeviceSN((UInt32)i, sbuff))
			{
				this->lvSLPort->SetSubItem(k, 3, sbuff);
			}
			if (this->siLabDriver->GetDeviceDesc((UInt32)i, sbuff))
			{
				this->lvSLPort->SetSubItem(k, 4, sbuff);
			}
			i++;
		}
		if (j > 0)
		{
			this->lvSLPort->SetSelectedIndex(0);
		}
	}

	this->tpTCPSvr = this->tcConfig->AddTabPage((const UTF8Char*)"TCP Server");
	NEW_CLASS(this->lblTCPSvrPort, UI::GUILabel(ui, this->tpTCPSvr, (const UTF8Char*)"Port"));
	this->lblTCPSvrPort->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtTCPSvrPort, UI::GUITextBox(ui, this->tpTCPSvr, (const UTF8Char*)""));
	this->txtTCPSvrPort->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->chkBoardcast, UI::GUICheckBox(ui, this->tpTCPSvr, (const UTF8Char*)"Boardcast", true));
	this->chkBoardcast->SetRect(104, 28, 100, 23, false);

	this->tpTCPCli = this->tcConfig->AddTabPage((const UTF8Char*)"TCP Client");
	NEW_CLASS(this->lblTCPCliHost, UI::GUILabel(ui, this->tpTCPCli, (const UTF8Char*)"Host"));
	this->lblTCPCliHost->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtTCPCliHost, UI::GUITextBox(ui, this->tpTCPCli, (const UTF8Char*)""));
	this->txtTCPCliHost->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->lblTCPCliPort, UI::GUILabel(ui, this->tpTCPCli, (const UTF8Char*)"Port"));
	this->lblTCPCliPort->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtTCPCliPort, UI::GUITextBox(ui, this->tpTCPCli, (const UTF8Char*)""));
	this->txtTCPCliPort->SetRect(104, 28, 100, 23, false);

	if (allowReadOnly)
	{
		this->tpFile = this->tcConfig->AddTabPage((const UTF8Char*)"File");
		NEW_CLASS(this->lblFileName, UI::GUILabel(ui, this->tpFile, (const UTF8Char*)"File Name"));
		this->lblFileName->SetRect(4, 4, 100, 23, false);
		NEW_CLASS(this->txtFileName, UI::GUITextBox(ui, this->tpFile, (const UTF8Char*)""));
		this->txtFileName->SetRect(104, 4, 400, 23, false);
		this->txtFileName->SetReadOnly(true);
		NEW_CLASS(this->btnFileBrowse, UI::GUIButton(ui, this->tpFile, (const UTF8Char*)"Browse"));
		this->btnFileBrowse->SetRect(504, 4, 75, 23, false);
		this->btnFileBrowse->HandleButtonClick(OnFileBrowseClick, this);
	}

	this->tpHID = tcConfig->AddTabPage((const UTF8Char*)"HID");
	NEW_CLASS(this->lbHIDDevice, UI::GUIListBox(ui, this->tpHID, false));
	this->lbHIDDevice->SetDockType(UI::GUIControl::DOCK_FILL);
	IO::DeviceInfo *dev;
	j = this->devMgr->QueryHIDDevices(this->devList);
	i = 0;
	while (i < j)
	{
		dev = this->devList->GetItem(i);
		const UTF8Char *name = dev->GetName();
		this->lbHIDDevice->AddItem({name, Text::StrCharCnt(name)}, dev);
		i++;
	}

	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
	SetInitStreamType(SSWR::AVIRead::AVIRCore::ST_SERIAL_PORT);
}

SSWR::AVIRead::AVIRSelStreamForm::~AVIRSelStreamForm()
{
	this->devMgr->FreeDevices(this->devList);
	DEL_CLASS(this->devList);
	DEL_CLASS(this->devMgr);
}

void SSWR::AVIRead::AVIRSelStreamForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRSelStreamForm::SetInitStreamType(SSWR::AVIRead::AVIRCore::StreamType stype)
{
	SSWR::AVIRead::AVIRCore::StreamType st;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->cboStreamType->GetCount();
	while (i < j)
	{
		st = (SSWR::AVIRead::AVIRCore::StreamType)(OSInt)this->cboStreamType->GetItem(i);
		if (st == stype)
		{
			this->cboStreamType->SetSelectedIndex(i);
			break;
		}
		i++;
	}
}

void SSWR::AVIRead::AVIRSelStreamForm::SetInitSerialPort(UOSInt port)
{
	UOSInt p;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->cboSerialPort->GetCount();
	while (i < j)
	{
		p = (UOSInt)(OSInt)this->cboSerialPort->GetItem(i);
		if (p == port)
		{
			this->cboSerialPort->SetSelectedIndex(i);
			this->SetInitStreamType(SSWR::AVIRead::AVIRCore::ST_SERIAL_PORT);
			break;
		}
		i++;
	}
}

void SSWR::AVIRead::AVIRSelStreamForm::SetInitBaudRate(Int32 baudRate)
{
	UTF8Char sbuff[32];
	Text::StrInt32(sbuff, baudRate);
	this->txtBaudRate->SetText(sbuff);
}
