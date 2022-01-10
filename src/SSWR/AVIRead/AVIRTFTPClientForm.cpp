#include "Stdafx.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "Net/TFTPClient.h"
#include "SSWR/AVIRead/AVIRTFTPClientForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRTFTPClientForm::OnRecvClick(void *userObj)
{
	SSWR::AVIRead::AVIRTFTPClientForm *me = (SSWR::AVIRead::AVIRTFTPClientForm*)userObj;
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	Text::StringBuilderUTF8 sb;
	me->txtHost->GetText(&sb);
	if (!me->core->GetSocketFactory()->DNSResolveIP(sb.ToString(), &addr))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Unknown Host", (const UTF8Char*)"TFTP Client", me);
		me->txtHost->Focus();
		return;
	}
	sb.ClearStr();
	me->txtPort->GetText(&sb);
	if (!sb.ToUInt16(&port))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Port is not valid", (const UTF8Char*)"TFTP Client", me);
		me->txtPort->Focus();
		return;
	}
	sb.ClearStr();
	me->txtFileName->GetText(&sb);
	if (sb.GetLength() <= 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter file name", (const UTF8Char*)"TFTP Client", me);
		me->txtFileName->Focus();
		return;
	}
	if (sb.IndexOf(IO::Path::PATH_SEPERATOR) != INVALID_INDEX)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"File Name is not valid", (const UTF8Char*)"TFTP Client", me);
		me->txtFileName->Focus();
		return;
	}
	UTF8Char sbuff[512];
	IO::Path::GetProcessFileName(sbuff);
	IO::Path::AppendPath(sbuff, sb.ToString());
	if (IO::Path::GetPathType(sbuff) != IO::Path::PathType::Unknown)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"File is already exist", (const UTF8Char*)"TFTP Client", me);
		me->txtFileName->Focus();
		return;
	}
	Net::TFTPClient *cli;
	NEW_CLASS(cli, Net::TFTPClient(me->core->GetSocketFactory(), &addr, port));
	if (cli->IsError())
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in starting client", (const UTF8Char*)"TFTP Client", me);
	}
	else
	{
		IO::MemoryStream *mstm;
		NEW_CLASS(mstm, IO::MemoryStream(UTF8STRC("SSWR.AVIRead.AVIRTFTPClientForm.mstm")));
		if (cli->RecvFile(sb.ToString(), mstm))
		{
			UInt8 *buff;
			UOSInt buffSize;
			buff = mstm->GetBuff(&buffSize);
			IO::FileStream *fs;
			NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			if (fs->IsError())
			{
				UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in creating file", (const UTF8Char*)"TFTP Client", me);
			}
			else if (fs->Write(buff, buffSize) == buffSize)
			{
				UI::MessageDialog::ShowDialog((const UTF8Char*)"File received successfully", (const UTF8Char*)"TFTP Client", me);
			}
			else
			{
				fs->Close();
				IO::Path::DeleteFile(sbuff);
				UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in saving file", (const UTF8Char*)"TFTP Client", me);
			}
			DEL_CLASS(fs);
		}
		else
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"File receive failed", (const UTF8Char*)"TFTP Client", me);
		}
		DEL_CLASS(mstm);
	}
	DEL_CLASS(cli);
}

void __stdcall SSWR::AVIRead::AVIRTFTPClientForm::OnSendClick(void *userObj)
{
	SSWR::AVIRead::AVIRTFTPClientForm *me = (SSWR::AVIRead::AVIRTFTPClientForm*)userObj;
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	Text::StringBuilderUTF8 sb;
	me->txtHost->GetText(&sb);
	if (!me->core->GetSocketFactory()->DNSResolveIP(sb.ToString(), &addr))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Unknown Host", (const UTF8Char*)"TFTP Client", me);
		me->txtHost->Focus();
		return;
	}
	sb.ClearStr();
	me->txtPort->GetText(&sb);
	if (!sb.ToUInt16(&port))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Port is not valid", (const UTF8Char*)"TFTP Client", me);
		me->txtPort->Focus();
		return;
	}
	UI::FileDialog *dlg;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"TFTPClient", false));
	dlg->SetAllowMultiSel(false);
	if (dlg->ShowDialog(me->GetHandle()))
	{
		const UTF8Char *fileName = dlg->GetFileName();
		UOSInt i = Text::StrLastIndexOf(fileName, IO::Path::PATH_SEPERATOR);
		Net::TFTPClient *cli;
		NEW_CLASS(cli, Net::TFTPClient(me->core->GetSocketFactory(), &addr, port));
		if (cli->IsError())
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in starting client", (const UTF8Char*)"TFTP Client", me);
		}
		else
		{
			IO::FileStream *fs;
			NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			if (fs->IsError())
			{
				UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in opening file", (const UTF8Char*)"TFTP Client", me);
			}
			else if (cli->SendFile(&fileName[i + 1], fs))
			{
				UI::MessageDialog::ShowDialog((const UTF8Char*)"File sent successfully", (const UTF8Char*)"TFTP Client", me);
			}
			else
			{
				UI::MessageDialog::ShowDialog((const UTF8Char*)"File sent failed", (const UTF8Char*)"TFTP Client", me);
			}
			DEL_CLASS(fs);
		}
		DEL_CLASS(cli);
	}
	DEL_CLASS(dlg);
}

SSWR::AVIRead::AVIRTFTPClientForm::AVIRTFTPClientForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 240, ui)
{
	this->core = core;
	this->SetText((const UTF8Char*)"TFTP Client");
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblHost, UI::GUILabel(ui, this, (const UTF8Char*)"Host"));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtHost, UI::GUITextBox(ui, this, (const UTF8Char*)"127.0.0.1"));
	this->txtHost->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this, (const UTF8Char*)"Port"));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this, (const UTF8Char*)"69"));
	this->txtPort->SetRect(104, 28, 50, 23, false);
	NEW_CLASS(this->lblFileName, UI::GUILabel(ui, this, (const UTF8Char*)"FileName"));
	this->lblFileName->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtFileName, UI::GUITextBox(ui, this, (const UTF8Char*)"File.dat"));
	this->txtFileName->SetRect(104, 52, 200, 23, false);
	NEW_CLASS(this->btnRecv, UI::GUIButton(ui, this, (const UTF8Char*)"Receive"));
	this->btnRecv->SetRect(104, 76, 75, 23, false);
	this->btnRecv->HandleButtonClick(OnRecvClick, this);
	NEW_CLASS(this->btnSend, UI::GUIButton(ui, this, (const UTF8Char*)"Send"));
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
