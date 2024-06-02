#include "Stdafx.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "Net/TFTPClient.h"
#include "SSWR/AVIRead/AVIRTFTPClientForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRTFTPClientForm::OnRecvClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTFTPClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTFTPClientForm>();
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	Text::StringBuilderUTF8 sb;
	me->txtHost->GetText(sb);
	if (!me->core->GetSocketFactory()->DNSResolveIP(sb.ToCString(), addr))
	{
		me->ui->ShowMsgOK(CSTR("Unknown Host"), CSTR("TFTP Client"), me);
		me->txtHost->Focus();
		return;
	}
	sb.ClearStr();
	me->txtPort->GetText(sb);
	if (!sb.ToUInt16(port))
	{
		me->ui->ShowMsgOK(CSTR("Port is not valid"), CSTR("TFTP Client"), me);
		me->txtPort->Focus();
		return;
	}
	sb.ClearStr();
	me->txtFileName->GetText(sb);
	if (sb.GetLength() <= 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter file name"), CSTR("TFTP Client"), me);
		me->txtFileName->Focus();
		return;
	}
	if (sb.IndexOf(IO::Path::PATH_SEPERATOR) != INVALID_INDEX)
	{
		me->ui->ShowMsgOK(CSTR("File Name is not valid"), CSTR("TFTP Client"), me);
		me->txtFileName->Focus();
		return;
	}
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, sb.ToCString());
	if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) != IO::Path::PathType::Unknown)
	{
		me->ui->ShowMsgOK(CSTR("File is already exist"), CSTR("TFTP Client"), me);
		me->txtFileName->Focus();
		return;
	}
	Net::TFTPClient cli(me->core->GetSocketFactory(), addr, port, me->core->GetLog());
	if (cli.IsError())
	{
		me->ui->ShowMsgOK(CSTR("Error in starting client"), CSTR("TFTP Client"), me);
	}
	else
	{
		IO::MemoryStream mstm;
		if (cli.RecvFile(sb.ToString(), &mstm))
		{
			UInt8 *buff;
			UOSInt buffSize;
			buff = mstm.GetBuff(buffSize);
			IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (fs.IsError())
			{
				me->ui->ShowMsgOK(CSTR("Error in creating file"), CSTR("TFTP Client"), me);
			}
			else if (fs.Write(buff, buffSize) == buffSize)
			{
				me->ui->ShowMsgOK(CSTR("File received successfully"), CSTR("TFTP Client"), me);
			}
			else
			{
				fs.Close();
				IO::Path::DeleteFile(sbuff);
				me->ui->ShowMsgOK(CSTR("Error in saving file"), CSTR("TFTP Client"), me);
			}
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("File receive failed"), CSTR("TFTP Client"), me);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRTFTPClientForm::OnSendClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTFTPClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTFTPClientForm>();
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	Text::StringBuilderUTF8 sb;
	me->txtHost->GetText(sb);
	if (!me->core->GetSocketFactory()->DNSResolveIP(sb.ToCString(), addr))
	{
		me->ui->ShowMsgOK(CSTR("Unknown Host"), CSTR("TFTP Client"), me);
		me->txtHost->Focus();
		return;
	}
	sb.ClearStr();
	me->txtPort->GetText(sb);
	if (!sb.ToUInt16(port))
	{
		me->ui->ShowMsgOK(CSTR("Port is not valid"), CSTR("TFTP Client"), me);
		me->txtPort->Focus();
		return;
	}
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"TFTPClient", false);
	dlg->SetAllowMultiSel(false);
	if (dlg->ShowDialog(me->GetHandle()))
	{
		NN<Text::String> fileName = dlg->GetFileName();
		UOSInt i = fileName->LastIndexOf(IO::Path::PATH_SEPERATOR);
		Net::TFTPClient cli(me->core->GetSocketFactory(), addr, port, me->core->GetLog());
		if (cli.IsError())
		{
			me->ui->ShowMsgOK(CSTR("Error in starting client"), CSTR("TFTP Client"), me);
		}
		else
		{
			IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (fs.IsError())
			{
				me->ui->ShowMsgOK(CSTR("Error in opening file"), CSTR("TFTP Client"), me);
			}
			else if (cli.SendFile(&fileName->v[i + 1], &fs))
			{
				me->ui->ShowMsgOK(CSTR("File sent successfully"), CSTR("TFTP Client"), me);
			}
			else
			{
				me->ui->ShowMsgOK(CSTR("File sent failed"), CSTR("TFTP Client"), me);
			}
		}
	}
	dlg.Delete();
}

SSWR::AVIRead::AVIRTFTPClientForm::AVIRTFTPClientForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 240, ui)
{
	this->core = core;
	this->SetText(CSTR("TFTP Client"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblHost = ui->NewLabel(*this, CSTR("Host"));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	this->txtHost = ui->NewTextBox(*this, CSTR("127.0.0.1"));
	this->txtHost->SetRect(104, 4, 150, 23, false);
	this->lblPort = ui->NewLabel(*this, CSTR("Port"));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	this->txtPort = ui->NewTextBox(*this, CSTR("69"));
	this->txtPort->SetRect(104, 28, 50, 23, false);
	this->lblFileName = ui->NewLabel(*this, CSTR("FileName"));
	this->lblFileName->SetRect(4, 52, 100, 23, false);
	this->txtFileName = ui->NewTextBox(*this, CSTR("File.dat"));
	this->txtFileName->SetRect(104, 52, 200, 23, false);
	this->btnRecv = ui->NewButton(*this, CSTR("Receive"));
	this->btnRecv->SetRect(104, 76, 75, 23, false);
	this->btnRecv->HandleButtonClick(OnRecvClick, this);
	this->btnSend = ui->NewButton(*this, CSTR("Send"));
	this->btnSend->SetRect(184, 76, 75, 23, false);
	this->btnSend->HandleButtonClick(OnSendClick, this);
}

SSWR::AVIRead::AVIRTFTPClientForm::~AVIRTFTPClientForm()
{
}

void SSWR::AVIRead::AVIRTFTPClientForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
