#include "Stdafx.h"
#include "Net/DefaultSSLEngine.h"
#include "Net/Email/SMTPClient.h"
#include "SSWR/AVIRead/AVIRSMTPClientForm.h"
#include "Text/StringBuilderWriter.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRSMTPClientForm::OnSendClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSMTPClientForm *me = (SSWR::AVIRead::AVIRSMTPClientForm*)userObj;
	Text::StringBuilderUTF8 sb1;
	Text::StringBuilderUTF8 sb2;
	UInt16 port;
	Net::SocketUtil::AddressInfo addr;
	me->txtHost->GetText(&sb1);
	me->txtPort->GetText(&sb2);
	if (!me->core->GetSocketFactory()->DNSResolveIP(sb1.ToString(), &addr))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in resolve host name", (const UTF8Char*)"SMTP Client", me);
		me->txtHost->Focus();
		return;
	}
	if (!sb2.ToUInt16(&port))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in port number", (const UTF8Char*)"SMTP Client", me);
		me->txtPort->Focus();
		return;
	}
	Net::Email::SMTPClient *cli;
	Text::StringBuilderUTF8 sbLog;
	Text::StringBuilderWriter writer(&sbLog);
	NEW_CLASS(cli, Net::Email::SMTPClient(me->core->GetSocketFactory(), me->ssl, sb1.ToString(), port, (Net::Email::SMTPConn::ConnType)(OSInt)me->cboSSLType->GetSelectedItem(), &writer));
	sb1.ClearStr();
	sb2.ClearStr();
	me->txtUsername->GetText(&sb1);
	me->txtPassword->GetText(&sb2);
	if (sb1.GetCharCnt() != 0 && sb2.GetCharCnt() != 0)
	{
		//cli->SetPlainAuth(sb1.ToString(), sb2.ToString());
	}
	Net::Email::EmailMessage *msg;
	NEW_CLASS(msg, Net::Email::EmailMessage());
	sb1.ClearStr();
	sb2.ClearStr();
	me->txtFromAddr->GetText(&sb1);
	me->txtToAddr->GetText(&sb2);
	if (sb1.GetCharCnt() == 0 || !msg->SetFrom(0, sb1.ToString()))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter valid From Address", (const UTF8Char*)"SMTP Client", me);
		me->txtFromAddr->Focus();
		DEL_CLASS(msg);
		DEL_CLASS(cli);
		return;
	}
	if (sb2.GetCharCnt() == 0 || !msg->AddTo(0, sb2.ToString()))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter valid To Address", (const UTF8Char*)"SMTP Client", me);
		me->txtToAddr->Focus();
		DEL_CLASS(msg);
		DEL_CLASS(cli);
		return;
	}
	sb1.ClearStr();
	me->txtCcAddr->GetText(&sb1);
	if (sb1.GetCharCnt() > 0 && !msg->AddCc(0, sb1.ToString()))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter valid Cc Address", (const UTF8Char*)"SMTP Client", me);
		me->txtCcAddr->Focus();
		DEL_CLASS(msg);
		DEL_CLASS(cli);
		return;
	}
	sb1.ClearStr();
	me->txtBccAddr->GetText(&sb1);
	if (sb1.GetCharCnt() > 0 && !msg->AddBcc(sb1.ToString()))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter valid Cc Address", (const UTF8Char*)"SMTP Client", me);
		me->txtBccAddr->Focus();
		DEL_CLASS(msg);
		DEL_CLASS(cli);
		return;
	}
	sb1.ClearStr();
	me->txtSubject->GetText(&sb1);
	if (sb1.GetCharCnt() == 0 || !msg->SetSubject(sb1.ToString()))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter valid Subject", (const UTF8Char*)"SMTP Client", me);
		me->txtSubject->Focus();
		DEL_CLASS(msg);
		DEL_CLASS(cli);
		return;
	}
	sb1.ClearStr();
	me->txtContent->GetText(&sb1);
	if (sb1.GetCharCnt() == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter content", (const UTF8Char*)"SMTP Client", me);
		me->txtSubject->Focus();
		DEL_CLASS(msg);
		DEL_CLASS(cli);
		return;
	}
	msg->SetContent(sb1.ToString(), "text/plain; charset=UTF-8");
	cli->Send(msg);
	DEL_CLASS(msg);
	DEL_CLASS(cli);
	me->txtLog->SetText(sbLog.ToString());
}

SSWR::AVIRead::AVIRSMTPClientForm::AVIRSMTPClientForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"SMTP Client");

	this->core = core;
	this->ssl = Net::DefaultSSLEngine::Create(this->core->GetSocketFactory(), true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 300, 23, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->lblHost, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)"Host"));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtHost, UI::GUITextBox(ui, this->pnlControl, (const UTF8Char*)"127.0.0.1"));
	this->txtHost->SetRect(104, 4, 200, 23, false);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)"Port"));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->pnlControl, (const UTF8Char*)"25"));
	this->txtPort->SetRect(104, 28, 200, 23, false);
	NEW_CLASS(this->lblSSLType, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)"SSL Type"));
	this->lblSSLType->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->cboSSLType, UI::GUIComboBox(ui, this->pnlControl, false));
	this->cboSSLType->SetRect(104, 52, 200, 23, false);
	this->cboSSLType->AddItem((const UTF8Char*)"Plain", (void*)(OSInt)Net::Email::SMTPConn::CT_PLAIN);
	this->cboSSLType->AddItem((const UTF8Char*)"STARTTLS", (void*)(OSInt)Net::Email::SMTPConn::CT_STARTTLS);
	this->cboSSLType->AddItem((const UTF8Char*)"SSL", (void*)(OSInt)Net::Email::SMTPConn::CT_SSL);
	this->cboSSLType->SetSelectedIndex(0);
	NEW_CLASS(this->lblUsername, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)"Username"));
	this->lblUsername->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtUsername, UI::GUITextBox(ui, this->pnlControl, (const UTF8Char*)""));
	this->txtUsername->SetRect(104, 76, 200, 23, false);
	NEW_CLASS(this->lblPassword, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)"Password"));
	this->lblPassword->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtPassword, UI::GUITextBox(ui, this->pnlControl, (const UTF8Char*)""));
	this->txtPassword->SetRect(104, 100, 200, 23, false);
	this->txtPassword->SetPasswordChar('*');
	NEW_CLASS(this->lblFromAddr, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)"From"));
	this->lblFromAddr->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtFromAddr, UI::GUITextBox(ui, this->pnlControl, (const UTF8Char*)"sswroom@yahoo.com"));
	this->txtFromAddr->SetRect(104, 124, 200, 23, false);
	NEW_CLASS(this->lblToAddr, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)"To"));
	this->lblToAddr->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtToAddr, UI::GUITextBox(ui, this->pnlControl, (const UTF8Char*)""));
	this->txtToAddr->SetRect(104, 148, 200, 23, false);
	NEW_CLASS(this->lblCcAddr, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)"Cc"));
	this->lblCcAddr->SetRect(4, 172, 100, 23, false);
	NEW_CLASS(this->txtCcAddr, UI::GUITextBox(ui, this->pnlControl, (const UTF8Char*)""));
	this->txtCcAddr->SetRect(104, 172, 200, 23, false);
	NEW_CLASS(this->lblBccAddr, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)"Bcc"));
	this->lblBccAddr->SetRect(4, 196, 100, 23, false);
	NEW_CLASS(this->txtBccAddr, UI::GUITextBox(ui, this->pnlControl, (const UTF8Char*)""));
	this->txtBccAddr->SetRect(104, 196, 200, 23, false);
	NEW_CLASS(this->lblSubject, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)"Subject"));
	this->lblSubject->SetRect(4, 220, 100, 23, false);
	NEW_CLASS(this->txtSubject, UI::GUITextBox(ui, this->pnlControl, (const UTF8Char*)""));
	this->txtSubject->SetRect(4, 244, 300, 23, false);
	NEW_CLASS(this->lblContent, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)"Content"));
	this->lblContent->SetRect(4, 268, 100, 23, false);
	NEW_CLASS(this->txtContent, UI::GUITextBox(ui, this->pnlControl, (const UTF8Char*)"", true));
	this->txtContent->SetRect(4, 292, 300, 71, false);
	NEW_CLASS(this->btnSend, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Send"));
	this->btnSend->SetRect(4, 364, 75, 23, false);
	this->btnSend->HandleButtonClick(OnSendClicked, this);
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this, (const UTF8Char*)"", true));
	this->txtLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtLog->SetReadOnly(true);
}

SSWR::AVIRead::AVIRSMTPClientForm::~AVIRSMTPClientForm()
{
	SDEL_CLASS(this->ssl);
}

void SSWR::AVIRead::AVIRSMTPClientForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}