#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/SerialPort.h"
#include "Net/TCPBoardcastStream.h"
#include "Net/TCPServerStream.h"
#include "Net/UDPServerStream.h"
#include "SSWR/AVIRead/AVIRSelStreamForm.h"
#include "UI/GUIFileDialog.h"

#define NETTIMEOUT 30000

void __stdcall SSWR::AVIRead::AVIRSelStreamForm::OnOKClick(void *userObj)
{
	SSWR::AVIRead::AVIRSelStreamForm *me = (SSWR::AVIRead::AVIRSelStreamForm*)userObj;
	IO::StreamType st = (IO::StreamType)(OSInt)me->cboStreamType->GetSelectedItem();
	UTF8Char sbuff[256];

	switch (st)
	{
	case IO::StreamType::SerialPort:
		{
			UOSInt i = me->cboSerialPort->GetSelectedIndex();
			UInt32 portNum = (UInt32)(UOSInt)me->cboSerialPort->GetItem(i);
			if (portNum == 0)
			{
				me->ui->ShowMsgOK(CSTR("Please select a port"), CSTR("Select Serial Port"), me);
				return;
			}
			me->txtBaudRate->GetText(sbuff);
			UInt32 baudRate = Text::StrToUInt32(sbuff);
			if (baudRate == 0)
			{
				me->ui->ShowMsgOK(CSTR("Please input a valid baud rate"), CSTR("Select Serial Port"), me);
				return;
			}
			IO::SerialPort::ParityType parity = (IO::SerialPort::ParityType)(OSInt)me->cboParity->GetSelectedItem();
			IO::SerialPort *port;
			NEW_CLASS(port, IO::SerialPort(portNum, baudRate, parity, false));
			if (port->IsError())
			{
				DEL_CLASS(port);
				me->ui->ShowMsgOK(CSTR("Error in opening the port"), CSTR("Select Serial Port"), me);
				return;
			}
			me->stm = port;
			me->stmType = st;
			me->SetDialogResult(UI::GUIForm::DR_OK);
		}
		break;
	case IO::StreamType::USBxpress:
		{
			UInt32 baudRate;
			Text::StringBuilderUTF8 sb;
			me->txtSLBaudRate->GetText(sb);
			if (!sb.ToUInt32(baudRate) || baudRate == 0)
			{
				me->ui->ShowMsgOK(CSTR("Please input baud rate"), CSTR("Error"), me);
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
				me->ui->ShowMsgOK(CSTR("Error in opening the port"), CSTR("Error"), me);
			}
		}
		break;
	case IO::StreamType::TCPServer:
		{
			Text::StringBuilderUTF8 sb;
			UInt16 port;
			me->txtTCPSvrPort->GetText(sb);
			if (!sb.ToUInt16(port))
			{
				me->ui->ShowMsgOK(CSTR("Port is not a number"), CSTR("Error"), me);
				return;
			}
			if (port <= 0 || port >= 65535)
			{
				me->ui->ShowMsgOK(CSTR("Port is out of range"), CSTR("Error"), me);
				return;
			}
			if (me->chkBoardcast->IsChecked())
			{
				Net::TCPBoardcastStream *stm;
				NEW_CLASS(stm, Net::TCPBoardcastStream(me->core->GetSocketFactory(), port, me->log));
				if (stm->IsError())
				{
					DEL_CLASS(stm);
					me->ui->ShowMsgOK(CSTR("Error in listening to the port"), CSTR("Error"), me);
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
				NEW_CLASS(stm, Net::TCPServerStream(me->core->GetSocketFactory(), port, me->log));
				if (stm->IsError())
				{
					DEL_CLASS(stm);
					me->ui->ShowMsgOK(CSTR("Error in listening to the port"), CSTR("Error"), me);
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
		break;
	case IO::StreamType::TCPClient:
		{
			Text::StringBuilderUTF8 sb;
			Net::SocketUtil::AddressInfo addr;
			UInt16 port;
			me->txtTCPCliHost->GetText(sb);
			if (!me->core->GetSocketFactory()->DNSResolveIP(sb.ToCString(), addr))
			{
				me->ui->ShowMsgOK(CSTR("Host is not valid"), CSTR("Error"), me);
				return;
			}
			sb.ClearStr();
			me->txtTCPCliPort->GetText(sb);
			if (!sb.ToUInt16(port))
			{
				me->ui->ShowMsgOK(CSTR("Port is not a number"), CSTR("Error"), me);
				return;
			}
			if (port <= 0 || port > 65535)
			{
				me->ui->ShowMsgOK(CSTR("Port is out of range"), CSTR("Error"), me);
				return;
			}
			Net::TCPClient *cli;
			NEW_CLASS(cli, Net::TCPClient(me->core->GetSocketFactory(), addr, port, NETTIMEOUT));
			if (cli->IsConnectError())
			{
				DEL_CLASS(cli);
				me->ui->ShowMsgOK(CSTR("Error in connect to server"), CSTR("Error"), me);
				return;
			}
			else
			{
				me->stm = cli;
				me->stmType = st;
				me->SetDialogResult(UI::GUIForm::DR_OK);
			}
		}
		break;
	case IO::StreamType::SSLClient:
		{
			Text::StringBuilderUTF8 sb;
			Net::SocketUtil::AddressInfo addr;
			NotNullPtr<Net::SSLEngine> ssl;
			if (!me->ssl.SetTo(ssl))
			{
				me->ui->ShowMsgOK(CSTR("SSL Engine is not initiated"), CSTR("Error"), me);
				return;
			}
			UInt16 port;
			me->txtSSLCliPort->GetText(sb);
			if (!sb.ToUInt16(port))
			{
				me->ui->ShowMsgOK(CSTR("Port is not a number"), CSTR("Error"), me);
				return;
			}
			if (port <= 0 || port > 65535)
			{
				me->ui->ShowMsgOK(CSTR("Port is out of range"), CSTR("Error"), me);
				return;
			}
			sb.ClearStr();
			me->txtSSLCliHost->GetText(sb);
			if (!me->core->GetSocketFactory()->DNSResolveIP(sb.ToCString(), addr))
			{
				me->ui->ShowMsgOK(CSTR("Host is not valid"), CSTR("Error"), me);
				return;
			}
			Net::SSLEngine::ErrorType err;
			Net::SSLClient *cli;
			cli = ssl->ClientConnect(sb.ToCString(), port, err, NETTIMEOUT);
			if (cli == 0)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Error in connect to server: "));
				sb.Append(Net::SSLEngine::ErrorTypeGetName(err));
				me->ui->ShowMsgOK(sb.ToCString(), CSTR("Error"), me);
				return;
			}
			else
			{
				me->stm = cli;
				me->stmType = st;
				me->SetDialogResult(UI::GUIForm::DR_OK);
			}
		}
		break;
	case IO::StreamType::File:
		{
			Text::StringBuilderUTF8 sb;
			IO::FileStream *fs;
			me->txtFileName->GetText(sb);
			if (sb.GetLength() > 0)
			{
				NEW_CLASS(fs, IO::FileStream(sb.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
				if (fs->IsError())
				{
					DEL_CLASS(fs);
					me->ui->ShowMsgOK(CSTR("Error in opening the file"), CSTR("Open Stream"), me);
				}
				else
				{
					me->stm = fs;
					me->stmType = st;
					me->SetDialogResult(UI::GUIForm::DR_OK);
				}
			}
		}
		break;
	case IO::StreamType::HID:
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
		break;
	case IO::StreamType::UDPServer:
		{
			Text::StringBuilderUTF8 sb;
			UInt16 port;
			me->txtUDPSvrPort->GetText(sb);
			if (!sb.ToUInt16(port))
			{
				me->ui->ShowMsgOK(CSTR("Port is not a number"), CSTR("Error"), me);
				return;
			}
			if (port <= 0 || port >= 65535)
			{
				me->ui->ShowMsgOK(CSTR("Port is out of range"), CSTR("Error"), me);
				return;
			}

			Net::UDPServerStream *stm;
			NEW_CLASS(stm, Net::UDPServerStream(me->core->GetSocketFactory(), port, me->core->GetLog()));
			if (stm->IsError())
			{
				DEL_CLASS(stm);
				me->ui->ShowMsgOK(CSTR("Error in listening to the port"), CSTR("Error"), me);
				return;
			}
			else
			{
				me->stm = stm;
				me->stmType = st;
				me->SetDialogResult(UI::GUIForm::DR_OK);
			}
		}
		break;
	case IO::StreamType::UDPClient:
		{
			Text::StringBuilderUTF8 sb;
			Net::SocketUtil::AddressInfo addr;
			UInt16 port;
			me->txtUDPCliHost->GetText(sb);
			if (!me->core->GetSocketFactory()->DNSResolveIP(sb.ToCString(), addr))
			{
				me->ui->ShowMsgOK(CSTR("Error in resolving host"), CSTR("Error"), me);
				return;
			}
			sb.ClearStr();
			me->txtUDPCliPort->GetText(sb);
			if (!sb.ToUInt16(port))
			{
				me->ui->ShowMsgOK(CSTR("Port is not a number"), CSTR("Error"), me);
				return;
			}
			if (port <= 0 || port >= 65535)
			{
				me->ui->ShowMsgOK(CSTR("Port is out of range"), CSTR("Error"), me);
				return;
			}

			Net::UDPServerStream *stm;
			NEW_CLASS(stm, Net::UDPServerStream(me->core->GetSocketFactory(), 0, me->core->GetLog()));
			if (stm->IsError())
			{
				DEL_CLASS(stm);
				me->ui->ShowMsgOK(CSTR("Error in listening to the port"), CSTR("Error"), me);
				return;
			}
			else
			{
				stm->SetClientAddr(&addr, port);
				me->stm = stm;
				me->stmType = st;
				me->SetDialogResult(UI::GUIForm::DR_OK);
			}
		}
		break;
	case IO::StreamType::WebSocket:
	case IO::StreamType::Unknown:
	case IO::StreamType::Memory:
	case IO::StreamType::BufferedOutput:
	case IO::StreamType::FTPClient:
	case IO::StreamType::StreamData:
	case IO::StreamType::Deflate:
	case IO::StreamType::HTTPClient:
	case IO::StreamType::TCPBoardcast:
	case IO::StreamType::BufferedInput:
	case IO::StreamType::StreamLogger:
	case IO::StreamType::ProcessExecution:
	case IO::StreamType::LZWDec:
	case IO::StreamType::WebConnection:
	case IO::StreamType::Hash:
	case IO::StreamType::Inflate:
	case IO::StreamType::WriteCache:
	case IO::StreamType::LZWEnc:
	case IO::StreamType::RS232GPIO:
	case IO::StreamType::WindowsCOM:
	case IO::StreamType::MemoryReading:
	case IO::StreamType::DataCapture:
	case IO::StreamType::Null:
	case IO::StreamType::FileView:
		break;
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
	me->txtFileName->GetText(sb);
	NotNullPtr<UI::GUIFileDialog> ofd = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"OpenStreamFile", false);
	ofd->SetAllowMultiSel(false);
	if (sb.GetLength() > 0)
	{
		ofd->SetFileName(sb.ToCString());
	}
	if (ofd->ShowDialog(me->GetHandle()) == UI::GUIForm::DR_OK)
	{
		me->txtFileName->SetText(ofd->GetFileName()->ToCString());
	}
	ofd.Delete();
}

void __stdcall SSWR::AVIRead::AVIRSelStreamForm::OnStmTypeChg(void *userObj)
{
	SSWR::AVIRead::AVIRSelStreamForm *me = (SSWR::AVIRead::AVIRSelStreamForm*)userObj;
	UOSInt i = me->cboStreamType->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		IO::StreamType st = (IO::StreamType)(OSInt)me->cboStreamType->GetItem(i);
		if (st == IO::StreamType::SerialPort)
		{
			me->tcConfig->SetSelectedPage(me->tpSerialPort);
		}
		else if (st == IO::StreamType::USBxpress)
		{
			me->tcConfig->SetSelectedPage(me->tpSiLabPort);
		}
		else if (st == IO::StreamType::TCPServer)
		{
			me->tcConfig->SetSelectedPage(me->tpTCPSvr);
		}
		else if (st == IO::StreamType::TCPClient)
		{
			me->tcConfig->SetSelectedPage(me->tpTCPCli);
		}
		else if (st == IO::StreamType::File)
		{
			me->tcConfig->SetSelectedPage(me->tpFile);
		}
		else if (st == IO::StreamType::HID)
		{
			me->tcConfig->SetSelectedPage(me->tpHID);
		}
	}
}

SSWR::AVIRead::AVIRSelStreamForm::AVIRSelStreamForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Bool allowReadOnly, Optional<Net::SSLEngine> ssl) : UI::GUIForm(parent, 640, 300, ui)
{
	UTF8Char sbuff[32];
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UTF8Char *sptr;

	this->SetText(CSTR("Select Stream"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->siLabDriver = this->core->GetSiLabDriver();
	this->ssl = ssl;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlStreamType = ui->NewPanel(*this);
	this->pnlStreamType->SetRect(0, 0, 100, 31, false);
	this->pnlStreamType->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblStreamType = ui->NewLabel(this->pnlStreamType, CSTR("Stream Type"));
	this->lblStreamType->SetRect(4, 4, 100, 23, false);
	this->cboStreamType = ui->NewComboBox(this->pnlStreamType, false);
	this->cboStreamType->SetRect(104, 4, 200, 23, false);
	this->cboStreamType->AddItem(IO::StreamTypeGetName(IO::StreamType::SerialPort), (void*)IO::StreamType::SerialPort);
	if (this->siLabDriver)
	{
		this->cboStreamType->AddItem(IO::StreamTypeGetName(IO::StreamType::USBxpress), (void*)IO::StreamType::USBxpress);
	}
	this->cboStreamType->AddItem(IO::StreamTypeGetName(IO::StreamType::TCPServer), (void*)IO::StreamType::TCPServer);
	this->cboStreamType->AddItem(IO::StreamTypeGetName(IO::StreamType::TCPClient), (void*)IO::StreamType::TCPClient);
	if (!this->ssl.IsNull())
	{
		this->cboStreamType->AddItem(IO::StreamTypeGetName(IO::StreamType::SSLClient), (void*)IO::StreamType::SSLClient);
	}
	this->cboStreamType->AddItem(IO::StreamTypeGetName(IO::StreamType::UDPServer), (void*)IO::StreamType::UDPServer);
	this->cboStreamType->AddItem(IO::StreamTypeGetName(IO::StreamType::UDPClient), (void*)IO::StreamType::UDPClient);
	if (allowReadOnly)
	{
		this->cboStreamType->AddItem(IO::StreamTypeGetName(IO::StreamType::File), (void*)IO::StreamType::File);
	}
	this->cboStreamType->AddItem(IO::StreamTypeGetName(IO::StreamType::HID), (void*)IO::StreamType::HID);
	this->cboStreamType->HandleSelectionChange(OnStmTypeChg, this);

	this->pnlButtons = ui->NewPanel(*this);
	this->pnlButtons->SetRect(0, 0, 100, 31, false);
	this->pnlButtons->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnOK = ui->NewButton(this->pnlButtons, CSTR("&OK"));
	this->btnOK->SetRect(49, 4, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClick, this);
	this->btnCancel = ui->NewButton(this->pnlButtons, CSTR("&Cancel"));
	this->btnCancel->SetRect(132, 4, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClick, this);

	this->tcConfig = ui->NewTabControl(*this);
	this->tcConfig->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpSerialPort = this->tcConfig->AddTabPage(CSTR("Serial Port"));
	this->lblSerialPort = ui->NewLabel(this->tpSerialPort, CSTR("Port"));
	this->lblSerialPort->SetRect(8, 8, 100, 23, false);
	this->cboSerialPort = ui->NewComboBox(this->tpSerialPort, false);
	this->cboSerialPort->SetRect(108, 8, 100, 23, false);

	{
		Data::ArrayList<UOSInt> ports;
		Data::ArrayList<IO::SerialPort::SerialPortType> portTypeList;
		IO::SerialPort::GetAvailablePorts(ports, &portTypeList);
		UOSInt currPort;
		i = 0;
		j = ports.GetCount();
		while (i < j)
		{
			currPort = ports.GetItem(i);
			sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("COM")), currPort);
			sptr = Text::StrConcatC(sptr, UTF8STRC(" ("));
			sptr = IO::SerialPort::GetPortTypeName(portTypeList.GetItem(i)).ConcatTo(sptr);
			sptr = Text::StrConcatC(sptr, UTF8STRC(")"));
			this->cboSerialPort->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)currPort);
			i++;
		}
		if (j > 0)
		{
			this->cboSerialPort->SetSelectedIndex(0);
		}
	}
	this->lblBaudRate = ui->NewLabel(this->tpSerialPort, CSTR("Baud Rate"));
	this->lblBaudRate->SetRect(8, 32, 100, 23, false);
	this->txtBaudRate = ui->NewTextBox(this->tpSerialPort, CSTR("115200"));
	this->txtBaudRate->SetRect(108, 32, 100, 23, false);
	this->lblParity = ui->NewLabel(this->tpSerialPort, CSTR("Parity"));
	this->lblParity->SetRect(8, 56, 100, 23, false);
	this->cboParity = ui->NewComboBox(this->tpSerialPort, false);
	this->cboParity->SetRect(108, 56, 100, 23, false);
	this->cboParity->AddItem(CSTR("None"), (void*)(OSInt)IO::SerialPort::PARITY_NONE);
	this->cboParity->AddItem(CSTR("Odd"), (void*)(OSInt)IO::SerialPort::PARITY_ODD);
	this->cboParity->AddItem(CSTR("Even"), (void*)(OSInt)IO::SerialPort::PARITY_EVEN);
	this->cboParity->SetSelectedIndex(0);

	if (this->siLabDriver)
	{
		this->tpSiLabPort = this->tcConfig->AddTabPage(CSTR("SiLab"));
		this->pnlSLInfo = ui->NewPanel(this->tpSiLabPort);
		this->pnlSLInfo->SetRect(0, 0, 100, 55, false);
		this->pnlSLInfo->SetDockType(UI::GUIControl::DOCK_TOP);
		this->lblDLLVer = ui->NewLabel(this->pnlSLInfo, CSTR("DLL Version"));
		this->lblDLLVer->SetRect(4, 4, 100, 23, false);
		this->txtDLLVer = ui->NewTextBox(this->pnlSLInfo, CSTR(""));
		this->txtDLLVer->SetRect(104, 4, 100, 23, false);
		this->txtDLLVer->SetReadOnly(true);
		this->lblDriverVer = ui->NewLabel(this->pnlSLInfo, CSTR("Driver Version"));
		this->lblDriverVer->SetRect(4, 28, 100, 23, false);
		this->txtDriverVer = ui->NewTextBox(this->pnlSLInfo, CSTR(""));
		this->txtDriverVer->SetRect(104, 28, 100, 23, false);
		this->txtDriverVer->SetReadOnly(true);

		this->pnlSLControl = ui->NewPanel(this->tpSiLabPort);
		this->pnlSLControl->SetRect(0, 0, 100, 67, false);
		this->pnlSLControl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
		this->lblSLBaudRate = ui->NewLabel(this->pnlSLControl, CSTR("Baud Rate"));
		this->lblSLBaudRate->SetRect(4, 4, 100, 23, false);
		this->txtSLBaudRate = ui->NewTextBox(this->pnlSLControl, CSTR("115200"));
		this->txtSLBaudRate->SetRect(104, 4, 100, 23, false);

		this->lvSLPort = ui->NewListView(this->tpSiLabPort, UI::ListViewStyle::Table, 5);
		this->lvSLPort->SetDockType(UI::GUIControl::DOCK_FILL);
		this->lvSLPort->AddColumn(CSTR("Num"), 40);
		this->lvSLPort->AddColumn(CSTR("VID"), 60);
		this->lvSLPort->AddColumn(CSTR("PID"), 60);
		this->lvSLPort->AddColumn(CSTR("SN"), 100);
		this->lvSLPort->AddColumn(CSTR("Description"), 400);
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
			this->txtDLLVer->SetText(sb.ToCString());
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
			this->txtDriverVer->SetText(sb.ToCString());
		}

		j = 0;
		j = this->siLabDriver->GetNumDevices();
		i = 0;
		while (i < j)
		{
			UInt32 v;
			sptr = Text::StrUOSInt(sbuff, i);
			k = this->lvSLPort->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)i);
			v = 0;
			if (this->siLabDriver->GetDeviceVID((UInt32)i, &v))
			{
				sptr = Text::StrHexVal16(sbuff, (UInt16)v);
				this->lvSLPort->SetSubItem(k, 1, CSTRP(sbuff, sptr));
			}
			if (this->siLabDriver->GetDevicePID((UInt32)i, &v))
			{
				sptr = Text::StrHexVal16(sbuff, (UInt16)v);
				this->lvSLPort->SetSubItem(k, 2, CSTRP(sbuff, sptr));
			}
			if ((sptr = this->siLabDriver->GetDeviceSN((UInt32)i, sbuff)) != 0)
			{
				this->lvSLPort->SetSubItem(k, 3, CSTRP(sbuff, sptr));
			}
			if ((sptr = this->siLabDriver->GetDeviceDesc((UInt32)i, sbuff)) != 0)
			{
				this->lvSLPort->SetSubItem(k, 4, CSTRP(sbuff, sptr));
			}
			i++;
		}
		if (j > 0)
		{
			this->lvSLPort->SetSelectedIndex(0);
		}
	}

	this->tpTCPSvr = this->tcConfig->AddTabPage(CSTR("TCP Server"));
	this->lblTCPSvrPort = ui->NewLabel(this->tpTCPSvr, CSTR("Port"));
	this->lblTCPSvrPort->SetRect(4, 4, 100, 23, false);
	this->txtTCPSvrPort = ui->NewTextBox(this->tpTCPSvr, CSTR(""));
	this->txtTCPSvrPort->SetRect(104, 4, 100, 23, false);
	this->chkBoardcast = ui->NewCheckBox(this->tpTCPSvr, CSTR("Boardcast"), true);
	this->chkBoardcast->SetRect(104, 28, 100, 23, false);

	this->tpTCPCli = this->tcConfig->AddTabPage(CSTR("TCP Client"));
	this->lblTCPCliHost = ui->NewLabel(this->tpTCPCli, CSTR("Host"));
	this->lblTCPCliHost->SetRect(4, 4, 100, 23, false);
	this->txtTCPCliHost = ui->NewTextBox(this->tpTCPCli, CSTR(""));
	this->txtTCPCliHost->SetRect(104, 4, 100, 23, false);
	this->lblTCPCliPort = ui->NewLabel(this->tpTCPCli, CSTR("Port"));
	this->lblTCPCliPort->SetRect(4, 28, 100, 23, false);
	this->txtTCPCliPort = ui->NewTextBox(this->tpTCPCli, CSTR(""));
	this->txtTCPCliPort->SetRect(104, 28, 100, 23, false);

	if (!this->ssl.IsNull())
	{
		this->tpSSLCli = this->tcConfig->AddTabPage(CSTR("SSL Client"));
		this->lblSSLCliHost = ui->NewLabel(this->tpSSLCli, CSTR("Host"));
		this->lblSSLCliHost->SetRect(4, 4, 100, 23, false);
		this->txtSSLCliHost = ui->NewTextBox(this->tpSSLCli, CSTR(""));
		this->txtSSLCliHost->SetRect(104, 4, 100, 23, false);
		this->lblSSLCliPort = ui->NewLabel(this->tpSSLCli, CSTR("Port"));
		this->lblSSLCliPort->SetRect(4, 28, 100, 23, false);
		this->txtSSLCliPort = ui->NewTextBox(this->tpSSLCli, CSTR(""));
		this->txtSSLCliPort->SetRect(104, 28, 100, 23, false);
	}

	this->tpUDPSvr = this->tcConfig->AddTabPage(CSTR("UDP Server"));
	this->lblUDPSvrPort = ui->NewLabel(this->tpUDPSvr, CSTR("Port"));
	this->lblUDPSvrPort->SetRect(4, 4, 100, 23, false);
	this->txtUDPSvrPort = ui->NewTextBox(this->tpUDPSvr, CSTR(""));
	this->txtUDPSvrPort->SetRect(104, 4, 100, 23, false);

	this->tpUDPCli = this->tcConfig->AddTabPage(CSTR("UDP Client"));
	this->lblUDPCliHost = ui->NewLabel(this->tpUDPCli, CSTR("Host"));
	this->lblUDPCliHost->SetRect(4, 4, 100, 23, false);
	this->txtUDPCliHost = ui->NewTextBox(this->tpUDPCli, CSTR(""));
	this->txtUDPCliHost->SetRect(104, 4, 100, 23, false);
	this->lblUDPCliPort = ui->NewLabel(this->tpUDPCli, CSTR("Port"));
	this->lblUDPCliPort->SetRect(4, 28, 100, 23, false);
	this->txtUDPCliPort = ui->NewTextBox(this->tpUDPCli, CSTR(""));
	this->txtUDPCliPort->SetRect(104, 28, 100, 23, false);

	if (allowReadOnly)
	{
		this->tpFile = this->tcConfig->AddTabPage(CSTR("File"));
		this->lblFileName = ui->NewLabel(this->tpFile, CSTR("File Name"));
		this->lblFileName->SetRect(4, 4, 100, 23, false);
		this->txtFileName = ui->NewTextBox(this->tpFile, CSTR(""));
		this->txtFileName->SetRect(104, 4, 400, 23, false);
		this->txtFileName->SetReadOnly(true);
		this->btnFileBrowse = ui->NewButton(this->tpFile, CSTR("Browse"));
		this->btnFileBrowse->SetRect(504, 4, 75, 23, false);
		this->btnFileBrowse->HandleButtonClick(OnFileBrowseClick, this);
	}

	this->tpHID = tcConfig->AddTabPage(CSTR("HID"));
	this->lbHIDDevice = ui->NewListBox(this->tpHID, false);
	this->lbHIDDevice->SetDockType(UI::GUIControl::DOCK_FILL);
	IO::DeviceInfo *dev;
	j = this->devMgr.QueryHIDDevices(&this->devList);
	i = 0;
	while (i < j)
	{
		dev = this->devList.GetItem(i);
		Text::String *name = dev->GetName();
		this->lbHIDDevice->AddItem(name->ToCString(), dev);
		i++;
	}

	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
	SetInitStreamType(IO::StreamType::SerialPort);
}

SSWR::AVIRead::AVIRSelStreamForm::~AVIRSelStreamForm()
{
	this->devMgr.FreeDevices(&this->devList);
}

void SSWR::AVIRead::AVIRSelStreamForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRSelStreamForm::SetInitStreamType(IO::StreamType stype)
{
	IO::StreamType st;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->cboStreamType->GetCount();
	while (i < j)
	{
		st = (IO::StreamType)(OSInt)this->cboStreamType->GetItem(i);
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
			this->SetInitStreamType(IO::StreamType::SerialPort);
			break;
		}
		i++;
	}
}

void SSWR::AVIRead::AVIRSelStreamForm::SetInitBaudRate(Int32 baudRate)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	sptr = Text::StrInt32(sbuff, baudRate);
	this->txtBaudRate->SetText(CSTRP(sbuff, sptr));
}

NotNullPtr<IO::Stream> SSWR::AVIRead::AVIRSelStreamForm::GetStream() const
{
	return NotNullPtr<IO::Stream>::FromPtr(this->stm);
}

IO::StreamType SSWR::AVIRead::AVIRSelStreamForm::GetStreamType() const
{
	return this->stmType;
}
