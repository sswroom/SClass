#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "Net/Email/SMTPClient.h"
#include "SSWR/AVIRead/AVIRSMTPClientForm.h"
#include "Text/StringBuilderWriter.h"

void __stdcall SSWR::AVIRead::AVIRSMTPClientForm::OnSendClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRSMTPClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSMTPClientForm>();
	Text::StringBuilderUTF8 sb1;
	Text::StringBuilderUTF8 sb2;
	UInt16 port;
	Net::SocketUtil::AddressInfo addr;
	me->txtHost->GetText(sb1);
	me->txtPort->GetText(sb2);
	if (!me->core->GetSocketFactory()->DNSResolveIP(sb1.ToCString(), addr))
	{
		me->ui->ShowMsgOK(CSTR("Error in resolve host name"), CSTR("SMTP Client"), me);
		me->txtHost->Focus();
		return;
	}
	if (!sb2.ToUInt16(port))
	{
		me->ui->ShowMsgOK(CSTR("Error in port number"), CSTR("SMTP Client"), me);
		me->txtPort->Focus();
		return;
	}
	Net::Email::SMTPClient *cli;
	Text::StringBuilderUTF8 sbLog;
	Text::StringBuilderWriter writer(sbLog);
	NEW_CLASS(cli, Net::Email::SMTPClient(me->core->GetSocketFactory(), me->ssl, sb1.ToCString(), port, (Net::Email::SMTPConn::ConnType)me->cboSSLType->GetSelectedItem().GetOSInt(), &writer, 60000));
	sb1.ClearStr();
	sb2.ClearStr();
	me->txtUsername->GetText(sb1);
	me->txtPassword->GetText(sb2);
	if (sb1.GetCharCnt() != 0 && sb2.GetCharCnt() != 0)
	{
		//cli->SetPlainAuth(sb1.ToString(), sb2.ToString());
	}
	Net::Email::EmailMessage msg;
	sb1.ClearStr();
	sb2.ClearStr();
	me->txtFromAddr->GetText(sb1);
	me->txtToAddr->GetText(sb2);
	if (sb1.GetCharCnt() == 0 || !msg.SetFrom(CSTR_NULL, sb1.ToCString()))
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid From Address"), CSTR("SMTP Client"), me);
		me->txtFromAddr->Focus();
		DEL_CLASS(cli);
		return;
	}
	if (sb2.GetCharCnt() == 0 || !msg.AddTo(CSTR_NULL, sb2.ToCString()))
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid To Address"), CSTR("SMTP Client"), me);
		me->txtToAddr->Focus();
		DEL_CLASS(cli);
		return;
	}
	sb1.ClearStr();
	me->txtCcAddr->GetText(sb1);
	if (sb1.GetCharCnt() > 0 && !msg.AddCc(CSTR_NULL, sb1.ToCString()))
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid Cc Address"), CSTR("SMTP Client"), me);
		me->txtCcAddr->Focus();
		DEL_CLASS(cli);
		return;
	}
	sb1.ClearStr();
	me->txtBccAddr->GetText(sb1);
	if (sb1.GetCharCnt() > 0 && !msg.AddBcc(sb1.ToCString()))
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid Cc Address"), CSTR("SMTP Client"), me);
		me->txtBccAddr->Focus();
		DEL_CLASS(cli);
		return;
	}
	sb1.ClearStr();
	me->txtSubject->GetText(sb1);
	if (sb1.GetCharCnt() == 0 || !msg.SetSubject(sb1.ToCString()))
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid Subject"), CSTR("SMTP Client"), me);
		me->txtSubject->Focus();
		DEL_CLASS(cli);
		return;
	}
	sb1.ClearStr();
	me->txtContent->GetText(sb1);
	if (sb1.GetCharCnt() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter content"), CSTR("SMTP Client"), me);
		me->txtSubject->Focus();
		DEL_CLASS(cli);
		return;
	}
	msg.SetContent(sb1.ToCString(), CSTR("text/plain; charset=UTF-8"));
	cli->Send(msg);
	DEL_CLASS(cli);
	me->txtLog->SetText(sbLog.ToCString());
}

SSWR::AVIRead::AVIRSMTPClientForm::AVIRSMTPClientForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("SMTP Client"));

	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 300, 23, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lblHost = ui->NewLabel(this->pnlControl, CSTR("Host"));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	this->txtHost = ui->NewTextBox(this->pnlControl, CSTR("127.0.0.1"));
	this->txtHost->SetRect(104, 4, 200, 23, false);
	this->lblPort = ui->NewLabel(this->pnlControl, CSTR("Port"));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->pnlControl, CSTR("25"));
	this->txtPort->SetRect(104, 28, 200, 23, false);
	this->lblSSLType = ui->NewLabel(this->pnlControl, CSTR("SSL Type"));
	this->lblSSLType->SetRect(4, 52, 100, 23, false);
	this->cboSSLType = ui->NewComboBox(this->pnlControl, false);
	this->cboSSLType->SetRect(104, 52, 200, 23, false);
	this->cboSSLType->AddItem(CSTR("Plain"), (void*)(OSInt)Net::Email::SMTPConn::ConnType::Plain);
	this->cboSSLType->AddItem(CSTR("STARTTLS"), (void*)(OSInt)Net::Email::SMTPConn::ConnType::STARTTLS);
	this->cboSSLType->AddItem(CSTR("SSL"), (void*)(OSInt)Net::Email::SMTPConn::ConnType::SSL);
	this->cboSSLType->SetSelectedIndex(0);
	this->lblUsername = ui->NewLabel(this->pnlControl, CSTR("Username"));
	this->lblUsername->SetRect(4, 76, 100, 23, false);
	this->txtUsername = ui->NewTextBox(this->pnlControl, CSTR(""));
	this->txtUsername->SetRect(104, 76, 200, 23, false);
	this->lblPassword = ui->NewLabel(this->pnlControl, CSTR("Password"));
	this->lblPassword->SetRect(4, 100, 100, 23, false);
	this->txtPassword = ui->NewTextBox(this->pnlControl, CSTR(""));
	this->txtPassword->SetRect(104, 100, 200, 23, false);
	this->txtPassword->SetPasswordChar('*');
	this->lblFromAddr = ui->NewLabel(this->pnlControl, CSTR("From"));
	this->lblFromAddr->SetRect(4, 124, 100, 23, false);
	this->txtFromAddr = ui->NewTextBox(this->pnlControl, CSTR("sswroom@yahoo.com"));
	this->txtFromAddr->SetRect(104, 124, 200, 23, false);
	this->lblToAddr = ui->NewLabel(this->pnlControl, CSTR("To"));
	this->lblToAddr->SetRect(4, 148, 100, 23, false);
	this->txtToAddr = ui->NewTextBox(this->pnlControl, CSTR(""));
	this->txtToAddr->SetRect(104, 148, 200, 23, false);
	this->lblCcAddr = ui->NewLabel(this->pnlControl, CSTR("Cc"));
	this->lblCcAddr->SetRect(4, 172, 100, 23, false);
	this->txtCcAddr = ui->NewTextBox(this->pnlControl, CSTR(""));
	this->txtCcAddr->SetRect(104, 172, 200, 23, false);
	this->lblBccAddr = ui->NewLabel(this->pnlControl, CSTR("Bcc"));
	this->lblBccAddr->SetRect(4, 196, 100, 23, false);
	this->txtBccAddr = ui->NewTextBox(this->pnlControl, CSTR(""));
	this->txtBccAddr->SetRect(104, 196, 200, 23, false);
	this->lblSubject = ui->NewLabel(this->pnlControl, CSTR("Subject"));
	this->lblSubject->SetRect(4, 220, 100, 23, false);
	this->txtSubject = ui->NewTextBox(this->pnlControl, CSTR(""));
	this->txtSubject->SetRect(4, 244, 300, 23, false);
	this->lblContent = ui->NewLabel(this->pnlControl, CSTR("Content"));
	this->lblContent->SetRect(4, 268, 100, 23, false);
	this->txtContent = ui->NewTextBox(this->pnlControl, CSTR(""), true);
	this->txtContent->SetRect(4, 292, 300, 71, false);
	this->btnSend = ui->NewButton(this->pnlControl, CSTR("Send"));
	this->btnSend->SetRect(4, 364, 75, 23, false);
	this->btnSend->HandleButtonClick(OnSendClicked, this);
	this->txtLog = ui->NewTextBox(*this, CSTR(""), true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtLog->SetReadOnly(true);
}

SSWR::AVIRead::AVIRSMTPClientForm::~AVIRSMTPClientForm()
{
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRSMTPClientForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
