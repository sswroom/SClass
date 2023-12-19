#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"
#include "Data/ByteBuffer.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Net/SSLEngineFactory.h"
#include "Net/Email/FileEmailStore.h"
#include "SSWR/AVIRead/AVIREmailServerForm.h"
#include "SSWR/AVIRead/AVIRMIMEViewerForm.h"
#include "SSWR/AVIRead/AVIRSSLCertKeyForm.h"
#include "Sync/MutexUsage.h"
#include "Text/MIMEObj/MailMessage.h"
#include "Text/MyString.h"

#define SERVER_DOMAIN CSTR("127.0.0.1")

void __stdcall SSWR::AVIRead::AVIREmailServerForm::OnSMTPStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIREmailServerForm *me = (SSWR::AVIRead::AVIREmailServerForm*)userObj;
	if (me->smtpSvr)
	{
		DEL_CLASS(me->smtpSvr);
		me->txtSMTPPort->SetReadOnly(false);
		me->smtpSvr = 0;
	}
	else
	{
		UInt16 port;
		Text::StringBuilderUTF8 sb;
		me->txtSMTPPort->GetText(sb);
		if (!sb.ToUInt16(port))
		{
			me->ui->ShowMsgOK(CSTR("Please enter valid port number"), CSTR("Error"), me);
			return;
		}
		if (port == 0 || port > 65535)
		{
			me->ui->ShowMsgOK(CSTR("Please enter valid port number"), CSTR("Error"), me);
			return;
		}
		NotNullPtr<Net::SSLEngine> ssl;
		NotNullPtr<Crypto::Cert::X509Cert> smtpSSLCert;
		NotNullPtr<Crypto::Cert::X509File> smtpSSLKey;
		if (me->smtpSSL.SetTo(ssl) && smtpSSLCert.Set(me->smtpSSLCert) && smtpSSLKey.Set(me->smtpSSLKey))
		{
			ssl->ServerSetCertsASN1(smtpSSLCert, smtpSSLKey, me->smtpCACerts);
		}
		Net::Email::SMTPConn::ConnType connType = (Net::Email::SMTPConn::ConnType)(OSInt)me->cboSMTPType->GetSelectedItem();
		NEW_CLASS(me->smtpSvr, Net::Email::SMTPServer(me->sockf, me->smtpSSL, port, connType, me->log, SERVER_DOMAIN, CSTR("SSWRSMTP"), OnMailReceived, OnMailLogin, me, true));
		if (me->smtpSvr->IsError())
		{
			DEL_CLASS(me->smtpSvr);
		}
		me->txtSMTPPort->SetReadOnly(true);
	}
}

void __stdcall SSWR::AVIRead::AVIREmailServerForm::OnPOP3StartClicked(void *userObj)
{
	SSWR::AVIRead::AVIREmailServerForm *me = (SSWR::AVIRead::AVIREmailServerForm*)userObj;
	if (me->pop3Svr)
	{
		DEL_CLASS(me->pop3Svr);
		me->txtPOP3Port->SetReadOnly(false);
		me->pop3Svr = 0;
	}
	else
	{
		UInt16 port;
		Text::StringBuilderUTF8 sb;
		me->txtPOP3Port->GetText(sb);
		if (!sb.ToUInt16(port))
		{
			me->ui->ShowMsgOK(CSTR("Please enter valid port number"), CSTR("Error"), me);
			return;
		}
		if (port == 0 || port > 65535)
		{
			me->ui->ShowMsgOK(CSTR("Please enter valid port number"), CSTR("Error"), me);
			return;
		}
		Optional<Net::SSLEngine> ssl = 0;
		Bool sslConn = me->chkPOP3SSL->IsChecked();
		NotNullPtr<Crypto::Cert::X509Cert> pop3SSLCert;
		NotNullPtr<Crypto::Cert::X509File> pop3SSLKey;
		if (pop3SSLCert.Set(me->pop3SSLCert) && pop3SSLKey.Set(me->pop3SSLKey))
		{
			ssl = me->pop3SSL;
			NotNullPtr<Net::SSLEngine> nnssl;
			if (ssl.SetTo(nnssl))
			{
				nnssl->ServerSetCertsASN1(pop3SSLCert, pop3SSLKey, me->pop3CACerts);
			}
		}
		else if (sslConn)
		{
			me->ui->ShowMsgOK(CSTR("Please select SSL Cert/Key"), CSTR("SMTP Server"), me);
			return;
		}
		NEW_CLASS(me->pop3Svr, Net::Email::POP3Server(me->core->GetSocketFactory(), ssl, sslConn, port, me->log, CSTR("Welcome to SSWR POP3 Server"), me, true));
		if (me->pop3Svr->IsError())
		{
			DEL_CLASS(me->pop3Svr);
			me->pop3Svr = 0;
		}
		else
		{
			me->txtPOP3Port->SetReadOnly(true);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIREmailServerForm::OnGCISStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIREmailServerForm *me = (SSWR::AVIRead::AVIREmailServerForm*)userObj;
	if (me->gcisListener)
	{
		DEL_CLASS(me->gcisListener);
		DEL_CLASS(me->gcisHdlr);
		me->txtGCISPort->SetReadOnly(false);
		me->txtGCISNotifPath->SetReadOnly(false);
		me->txtGCISBatchUplPath->SetReadOnly(false);
		me->btnGCISStart->SetText(CSTR("Start"));
		me->gcisListener = 0;
		me->gcisHdlr = 0;
	}
	else
	{
		UInt16 port;
		Text::StringBuilderUTF8 sb;
		Text::StringBuilderUTF8 sb2;
		me->txtGCISPort->GetText(sb);
		if (!sb.ToUInt16(port))
		{
			me->ui->ShowMsgOK(CSTR("Please enter valid port number"), CSTR("Error"), me);
			return;
		}
		if (port == 0 || port > 65535)
		{
			me->ui->ShowMsgOK(CSTR("Please enter valid port number"), CSTR("Error"), me);
			return;
		}
		sb.ClearStr();
		me->txtGCISNotifPath->GetText(sb);
		if (!sb.StartsWith('/'))
		{
			me->ui->ShowMsgOK(CSTR("Please enter valid Notif path"), CSTR("Error"), me);
			return;
		}
		me->txtGCISBatchUplPath->GetText(sb2);
		if (!sb2.StartsWith('/'))
		{
			me->ui->ShowMsgOK(CSTR("Please enter valid Batch Upload path"), CSTR("Error"), me);
			return;
		}
		Optional<Net::SSLEngine> ssl = 0;
		NotNullPtr<Crypto::Cert::X509Cert> gcisSSLCert;
		NotNullPtr<Crypto::Cert::X509File> gcisSSLKey;
		if (!gcisSSLCert.Set(me->gcisSSLCert) || !gcisSSLKey.Set(me->gcisSSLKey))
		{
			me->ui->ShowMsgOK(CSTR("Please select SSL Cert/Key"), CSTR("SMTP Server"), me);
			return;
		}
		ssl = me->gcisSSL;
		NotNullPtr<Net::SSLEngine> nnssl;
		Crypto::Cert::X509Cert *issuerCert = Crypto::Cert::CertUtil::FindIssuer(gcisSSLCert);
		if (ssl.SetTo(nnssl))
		{
			nnssl->ServerSetRequireClientCert(Net::SSLEngine::ClientCertType::Optional);
			nnssl->ServerSetCertsASN1(gcisSSLCert, gcisSSLKey, me->gcisCACerts);
		}
		SDEL_CLASS(issuerCert);
		NotNullPtr<Net::WebServer::GCISNotifyHandler> gcisHdlr;
		NEW_CLASSNN(gcisHdlr, Net::WebServer::GCISNotifyHandler(sb.ToCString(), sb2.ToCString(), OnGCISMailReceived, me, me->log));
		NEW_CLASS(me->gcisListener, Net::WebServer::WebListener(me->core->GetSocketFactory(), ssl, gcisHdlr, port, 60, 1, 2, CSTR("SSWRGCIS/1.0"), false, Net::WebServer::KeepAlive::Default, true));
		if (me->gcisListener->IsError())
		{
			DEL_CLASS(me->gcisListener);
			gcisHdlr.Delete();
			me->gcisListener = 0;
		}
		else
		{
			me->gcisHdlr = gcisHdlr.Ptr();
			me->txtGCISPort->SetReadOnly(true);
			me->txtGCISNotifPath->SetReadOnly(true);
			me->txtGCISBatchUplPath->SetReadOnly(true);
			me->btnGCISStart->SetText(CSTR("Stop"));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIREmailServerForm::OnLogFileClicked(void *userObj)
{
	SSWR::AVIRead::AVIREmailServerForm *me = (SSWR::AVIRead::AVIREmailServerForm*)userObj;
	Text::StringBuilderUTF8 sb;
	IO::Path::GetProcessFileName(sb);
	if (IO::Path::PATH_SEPERATOR == '\\')
	{
		IO::Path::AppendPath(sb, UTF8STRC("log\\SMTP"));
	}
	else
	{
		IO::Path::AppendPath(sb, UTF8STRC("log/SMTP"));
	}
	me->log.AddFileLog(sb.ToCString(), IO::LogHandler::LogType::PerDay, IO::LogHandler::LogGroup::PerMonth, IO::LogHandler::LogLevel::Command, "yyyy-MM-dd HH:mm:ss.fff", false);
	me->btnLogFile->SetEnabled(false);
}

void __stdcall SSWR::AVIRead::AVIREmailServerForm::OnEmailDblClicked(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIREmailServerForm *me = (SSWR::AVIRead::AVIREmailServerForm*)userObj;
	Net::Email::EmailStore::EmailInfo *email;
	email = (Net::Email::EmailStore::EmailInfo*)me->lvEmail->GetItem(index);
	if (email)
	{
		Text::MIMEObj::MailMessage *mail;
		NotNullPtr<IO::StreamData> fd;
		if (fd.Set(me->store->OpenEmailData(email->id)))
		{
			mail = Text::MIMEObj::MailMessage::ParseFile(fd);
			fd.Delete();
		}
		else
		{
			mail = 0;
		}
		if (mail)
		{
			SSWR::AVIRead::AVIRMIMEViewerForm frm(0, me->ui, me->core, mail);
			frm.ShowDialog(me);
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Error in loading file"), CSTR("Error"), me);
		}
	}
}

UTF8Char *__stdcall SSWR::AVIRead::AVIREmailServerForm::OnMailReceived(UTF8Char *queryId, void *userObj, NotNullPtr<Net::TCPClient> cli, NotNullPtr<const Net::Email::SMTPServer::MailStatus> mail)
{
	SSWR::AVIRead::AVIREmailServerForm *me = (SSWR::AVIRead::AVIREmailServerForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Received email from "));
	sptr = cli->GetRemoteName(sbuff);
	sb.AppendP(sbuff, sptr);
	Int64 id = me->store->NextEmailId();
	sb.AppendC(UTF8STRC(", id = "));
	sb.AppendI64(id);
	me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);

	Net::SocketUtil::AddressInfo remoteAddr;
	cli->GetRemoteAddr(remoteAddr);
	me->store->NewEmail(id, remoteAddr, SERVER_DOMAIN, mail);
	me->mailChanged = true;
	me->totalSize += (UOSInt)mail->dataStm->GetLength();
	return Text::StrInt64(queryId, id);
}

void __stdcall SSWR::AVIRead::AVIREmailServerForm::OnGCISMailReceived(void *userObj, NotNullPtr<Net::NetConnection> cli, NotNullPtr<const Text::MIMEObj::MailMessage> mail)
{
	SSWR::AVIRead::AVIREmailServerForm *me = (SSWR::AVIRead::AVIREmailServerForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Received email from "));
	sptr = cli->GetRemoteName(sbuff);
	if (sptr)
	{
		sb.AppendP(sbuff, sptr);
	}
	Int64 id = me->store->NextEmailId();
	sb.AppendC(UTF8STRC(", id = "));
	sb.AppendI64(id);
	me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);

	Net::SocketUtil::AddressInfo remoteAddr;
	cli->GetRemoteAddr(remoteAddr);
	me->store->NewEmail(id, remoteAddr, SERVER_DOMAIN, mail);
	me->mailChanged = true;
	{
		IO::MemoryStream mstm;
		mail->GetContent()->WriteStream(&mstm);
		me->totalSize += (UOSInt)mstm.GetLength();
	}
}

Bool __stdcall SSWR::AVIRead::AVIREmailServerForm::OnMailLogin(void *userObj, Text::CString userName, Text::CString pwd)
{
	Text::StringBuilderUTF8 sb;
	SSWR::AVIRead::AVIREmailServerForm *me = (SSWR::AVIRead::AVIREmailServerForm *)userObj;
	sb.AppendC(UTF8STRC("User: "));
	sb.Append(userName);
	sb.AppendC(UTF8STRC(", Pwd: "));
	sb.Append(pwd);
	me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);
	return true;
}

void __stdcall SSWR::AVIRead::AVIREmailServerForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIREmailServerForm *me = (SSWR::AVIRead::AVIREmailServerForm*)userObj;
	Net::Email::EmailStore::EmailInfo *email;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	Data::DateTime dt;
	dt.ToLocalTime();
	if (me->mailChanged)
	{
		me->mailChanged = false;
		Data::ArrayList<Net::Email::EmailStore::EmailInfo*> emailList;
		Data::ArrayListStringNN rcptList;
		me->store->GetAllEmails(&emailList);
		me->lvEmail->ClearItems();
		i = 0;
		j = emailList.GetCount();
		while (i < j)
		{
			email = emailList.GetItem(i);
			sb.ClearStr();
			sb.AppendI64(email->id);
			k = me->lvEmail->AddItem(sb.ToCString(), email);
			dt.SetTicks(email->recvTime);
			sptr = dt.ToString(sbuff);
			me->lvEmail->SetSubItem(k, 1, CSTRP(sbuff, sptr));
			me->lvEmail->SetSubItem(k, 2, email->fromAddr);
			sptr = Net::SocketUtil::GetAddrName(sbuff, email->remoteAddr);
			me->lvEmail->SetSubItem(k, 3, CSTRP(sbuff, sptr));
			rcptList.Clear();
			me->store->GetRcptList(email->id, rcptList);
			sb.ClearStr();
			Data::ArrayIterator<NotNullPtr<Text::String>> it = rcptList.Iterator();
			Bool found = false;
			while (it.HasNext())
			{
				if (found)
					sb.AppendC(UTF8STRC(", "));
				sb.Append(it.Next());
				found = true;
			}
			me->lvEmail->SetSubItem(k, 4, sb.ToCString());
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIREmailServerForm::OnSMTPCertKeyClicked(void *userObj)
{
	SSWR::AVIRead::AVIREmailServerForm *me = (SSWR::AVIRead::AVIREmailServerForm*)userObj;
	if (me->smtpSvr)
	{
		me->ui->ShowMsgOK(CSTR("Cannot change Cert/Key when SMTP server is started"), CSTR("SMTP Server"), me);
		return;
	}
	SSWR::AVIRead::AVIRSSLCertKeyForm frm(0, me->ui, me->core, me->smtpSSL, me->smtpSSLCert, me->smtpSSLKey, me->smtpCACerts);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		SDEL_CLASS(me->smtpSSLCert);
		SDEL_CLASS(me->smtpSSLKey);
		me->ClearSMTPCACerts();
		me->smtpSSLCert = frm.GetCert();
		me->smtpSSLKey = frm.GetKey();
		frm.GetCACerts(me->smtpCACerts);
		Text::StringBuilderUTF8 sb;
		me->smtpSSLCert->ToShortString(sb);
		sb.AppendC(UTF8STRC(", "));
		me->smtpSSLKey->ToShortString(sb);
		me->lblSMTPCertKey->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIREmailServerForm::OnPOP3CertKeyClicked(void *userObj)
{
	SSWR::AVIRead::AVIREmailServerForm *me = (SSWR::AVIRead::AVIREmailServerForm*)userObj;
	if (me->pop3Svr)
	{
		me->ui->ShowMsgOK(CSTR("Cannot change Cert/Key when POP3 server is started"), CSTR("SMTP Server"), me);
		return;
	}
	SSWR::AVIRead::AVIRSSLCertKeyForm frm(0, me->ui, me->core, me->pop3SSL, me->pop3SSLCert, me->pop3SSLKey, me->pop3CACerts);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		SDEL_CLASS(me->pop3SSLCert);
		SDEL_CLASS(me->pop3SSLKey);
		me->ClearPOP3CACerts();
		me->pop3SSLCert = frm.GetCert();
		me->pop3SSLKey = frm.GetKey();
		frm.GetCACerts(me->pop3CACerts);
		Text::StringBuilderUTF8 sb;
		me->pop3SSLCert->ToShortString(sb);
		sb.AppendC(UTF8STRC(", "));
		me->pop3SSLKey->ToShortString(sb);
		me->lblPOP3CertKey->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIREmailServerForm::OnGCISCertKeyClicked(void *userObj)
{
	SSWR::AVIRead::AVIREmailServerForm *me = (SSWR::AVIRead::AVIREmailServerForm*)userObj;
	if (me->gcisListener)
	{
		me->ui->ShowMsgOK(CSTR("Cannot change Cert/Key when GCIS server is started"), CSTR("SMTP Server"), me);
		return;
	}
	SSWR::AVIRead::AVIRSSLCertKeyForm frm(0, me->ui, me->core, me->gcisSSL, me->gcisSSLCert, me->gcisSSLKey, me->gcisCACerts);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		SDEL_CLASS(me->gcisSSLCert);
		SDEL_CLASS(me->gcisSSLKey);
		me->ClearGCISCACerts();
		me->gcisSSLCert = frm.GetCert();
		me->gcisSSLKey = frm.GetKey();
		frm.GetCACerts(me->gcisCACerts);
		Text::StringBuilderUTF8 sb;
		me->gcisSSLCert->ToShortString(sb);
		sb.AppendC(UTF8STRC(", "));
		me->gcisSSLKey->ToShortString(sb);
		me->lblGCISCertKey->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIREmailServerForm::OnSMTPTypeSelChg(void *userObj)
{
	SSWR::AVIRead::AVIREmailServerForm *me = (SSWR::AVIRead::AVIREmailServerForm*)userObj;
	UInt16 port;
	Text::StringBuilderUTF8 sb;
	Net::Email::SMTPConn::ConnType newType = (Net::Email::SMTPConn::ConnType)(OSInt)me->cboSMTPType->GetSelectedItem();
	me->txtSMTPPort->GetText(sb);
	if (sb.ToUInt16(port))
	{
		if (port == Net::Email::SMTPServer::GetDefaultPort(me->smtpType))
		{
			sb.ClearStr();
			sb.AppendU16(Net::Email::SMTPServer::GetDefaultPort(newType));
			me->txtSMTPPort->SetText(sb.ToCString());
		}
	}
	me->smtpType = newType;
}

void __stdcall SSWR::AVIRead::AVIREmailServerForm::OnPOP3SSLChanged(void *userObj, Bool isChecked)
{
	SSWR::AVIRead::AVIREmailServerForm *me = (SSWR::AVIRead::AVIREmailServerForm*)userObj;
	UInt16 port;
	Text::StringBuilderUTF8 sb;
	me->txtPOP3Port->GetText(sb);
	if (sb.ToUInt16(port))
	{
		if (port == Net::Email::POP3Server::GetDefaultPort(!isChecked))
		{
			sb.ClearStr();
			sb.AppendU16(Net::Email::POP3Server::GetDefaultPort(isChecked));
			me->txtPOP3Port->SetText(sb.ToCString());
		}
	}
}

Optional<Text::String> SSWR::AVIRead::AVIREmailServerForm::GetUserName(Int32 userId)
{
	Sync::MutexUsage mutUsage(this->userMut);
	return this->userList.GetItem((UInt32)userId - 1);
}

void SSWR::AVIRead::AVIREmailServerForm::ClearSMTPCACerts()
{
	UOSInt i = this->smtpCACerts.GetCount();
	while (i-- > 0)
	{
		this->smtpCACerts.GetItem(i).Delete();
	}
	this->smtpCACerts.Clear();
}

void SSWR::AVIRead::AVIREmailServerForm::ClearPOP3CACerts()
{
	UOSInt i = this->pop3CACerts.GetCount();
	while (i-- > 0)
	{
		this->pop3CACerts.GetItem(i).Delete();
	}
	this->pop3CACerts.Clear();
}

void SSWR::AVIRead::AVIREmailServerForm::ClearGCISCACerts()
{
	UOSInt i = this->gcisCACerts.GetCount();
	while (i-- > 0)
	{
		this->gcisCACerts.GetItem(i).Delete();
	}
	this->gcisCACerts.Clear();
}

SSWR::AVIRead::AVIREmailServerForm::AVIREmailServerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Email Server"));

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->smtpSSL = Net::SSLEngineFactory::Create(this->sockf, true);
	this->smtpSSLCert = 0;
	this->smtpSSLKey = 0;
	this->smtpSvr = 0;
	this->pop3SSL = Net::SSLEngineFactory::Create(this->sockf, true);
	this->pop3SSLCert = 0;
	this->pop3SSLKey = 0;
	this->pop3Svr = 0;
	this->gcisSSL = Net::SSLEngineFactory::Create(this->sockf, true);
	this->gcisSSLCert = 0;
	this->gcisSSLKey = 0;
	this->gcisListener = 0;
	this->gcisHdlr = 0;
	this->totalSize = 0;
	this->recvSize = 0;
	this->smtpType = Net::Email::SMTPConn::ConnType::Plain;
	this->mailChanged = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASS(this->store, Net::Email::FileEmailStore());

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpSMTP = this->tcMain->AddTabPage(CSTR("SMTP"));
	this->btnSMTPCertKey = ui->NewButton(this->tpSMTP, CSTR("Cert/Key"));
	this->btnSMTPCertKey->SetRect(0, 0, 75, 23, false);
	this->btnSMTPCertKey->HandleButtonClick(OnSMTPCertKeyClicked, this);
	this->lblSMTPCertKey = ui->NewLabel(this->tpSMTP, CSTR("No Cert/Keys"));
	this->lblSMTPCertKey->SetRect(80, 0, 200, 23, false);
	this->lblSMTPPort = ui->NewLabel(this->tpSMTP, CSTR("Port"));
	this->lblSMTPPort->SetRect(0, 24, 100, 23, false);
	this->txtSMTPPort = ui->NewTextBox(this->tpSMTP, CSTR("25"));
	this->txtSMTPPort->SetRect(100, 24, 100, 23, false);
	this->lblSMTPType = ui->NewLabel(this->tpSMTP, CSTR("Type"));
	this->lblSMTPType->SetRect(0, 48, 100, 23, false);
	this->cboSMTPType = ui->NewComboBox(this->tpSMTP, false);
	this->cboSMTPType->SetRect(100, 48, 100, 23, false);
	this->cboSMTPType->AddItem(CSTR("Plain"), (void*)(OSInt)Net::Email::SMTPConn::ConnType::Plain);
	this->cboSMTPType->AddItem(CSTR("STARTTLS"), (void*)(OSInt)Net::Email::SMTPConn::ConnType::STARTTLS);
	this->cboSMTPType->AddItem(CSTR("SSL"), (void*)(OSInt)Net::Email::SMTPConn::ConnType::SSL);
	this->cboSMTPType->SetSelectedIndex(0);
	this->cboSMTPType->HandleSelectionChange(OnSMTPTypeSelChg, this);
	this->btnSMTPStart = ui->NewButton(this->tpSMTP, CSTR("Start"));
	this->btnSMTPStart->SetRect(100, 72, 75, 23, false);
	this->btnSMTPStart->HandleButtonClick(OnSMTPStartClicked, this);
	this->btnLogFile = ui->NewButton(this->tpSMTP, CSTR("Log To File"));
	this->btnLogFile->SetRect(4, 96, 75, 23, false);
	this->btnLogFile->HandleButtonClick(OnLogFileClicked, this);

	this->tpPOP3 = this->tcMain->AddTabPage(CSTR("POP3"));
	this->btnPOP3CertKey = ui->NewButton(this->tpPOP3, CSTR("Cert/Key"));
	this->btnPOP3CertKey->SetRect(0, 0, 75, 23, false);
	this->btnPOP3CertKey->HandleButtonClick(OnPOP3CertKeyClicked, this);
	this->lblPOP3CertKey = ui->NewLabel(this->tpPOP3, CSTR("No Cert/Keys"));
	this->lblPOP3CertKey->SetRect(80, 0, 200, 23, false);
	this->lblPOP3Port = ui->NewLabel(this->tpPOP3, CSTR("Port"));
	this->lblPOP3Port->SetRect(0, 24, 100, 23, false);
	this->txtPOP3Port = ui->NewTextBox(this->tpPOP3, CSTR("110"));
	this->txtPOP3Port->SetRect(100, 24, 100, 23, false);
	this->chkPOP3SSL = ui->NewCheckBox(this->tpPOP3, CSTR("Enable SSL"), false);
	this->chkPOP3SSL->SetRect(100, 48, 100, 23, false);
	this->chkPOP3SSL->HandleCheckedChange(OnPOP3SSLChanged, this);
	this->btnPOP3Start = ui->NewButton(this->tpPOP3, CSTR("Start"));
	this->btnPOP3Start->SetRect(100, 72, 75, 23, false);
	this->btnPOP3Start->HandleButtonClick(OnPOP3StartClicked, this);

	this->tpGCIS = this->tcMain->AddTabPage(CSTR("GCIS"));
	this->btnGCISCertKey = ui->NewButton(this->tpGCIS, CSTR("Cert/Key"));
	this->btnGCISCertKey->SetRect(0, 0, 75, 23, false);
	this->btnGCISCertKey->HandleButtonClick(OnGCISCertKeyClicked, this);
	this->lblGCISCertKey = ui->NewLabel(this->tpGCIS, CSTR("No Cert/Keys"));
	this->lblGCISCertKey->SetRect(80, 0, 200, 23, false);
	this->lblGCISPort = ui->NewLabel(this->tpGCIS, CSTR("Port"));
	this->lblGCISPort->SetRect(0, 24, 100, 23, false);
	this->txtGCISPort = ui->NewTextBox(this->tpGCIS, CSTR("9444"));
	this->txtGCISPort->SetRect(100, 24, 100, 23, false);
	this->lblGCISNotifPath = ui->NewLabel(this->tpGCIS, CSTR("Notif Path"));
	this->lblGCISNotifPath->SetRect(0, 48, 100, 23, false);
	this->txtGCISNotifPath = ui->NewTextBox(this->tpGCIS, CSTR("/messaging/ssl/RESTful/NotiSenderRest"));
	this->txtGCISNotifPath->SetRect(100, 48, 400, 23, false);
	this->lblGCISBatchUplPath = ui->NewLabel(this->tpGCIS, CSTR("Batch Upload"));
	this->lblGCISBatchUplPath->SetRect(0, 72, 100, 23, false);
	this->txtGCISBatchUplPath = ui->NewTextBox(this->tpGCIS, CSTR("/messaging/ssl/RESTful/BatchUploadRest"));
	this->txtGCISBatchUplPath->SetRect(100, 72, 400, 23, false);
	this->btnGCISStart = ui->NewButton(this->tpGCIS, CSTR("Start"));
	this->btnGCISStart->SetRect(100, 96, 75, 23, false);
	this->btnGCISStart->HandleButtonClick(OnGCISStartClicked, this);

	this->tpEmail = this->tcMain->AddTabPage(CSTR("Email"));
	NEW_CLASS(this->lvEmail, UI::GUIListView(ui, this->tpEmail, UI::GUIListView::LVSTYLE_TABLE, 5));
	this->lvEmail->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvEmail->SetShowGrid(true);
	this->lvEmail->SetFullRowSelect(true);
	this->lvEmail->AddColumn(CSTR("Id"), 100);
	this->lvEmail->AddColumn(CSTR("Time"), 150);
	this->lvEmail->AddColumn(CSTR("From"), 150);
	this->lvEmail->AddColumn(CSTR("Remote IP"), 100);
	this->lvEmail->AddColumn(CSTR("Rcpt"), 200);
	this->lvEmail->HandleDblClk(OnEmailDblClicked, this);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	this->txtLog = ui->NewTextBox(this->tpLog, CSTR(""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASSNN(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);

	NEW_CLASSNN(this->logger, UI::ListBoxLogger(*this, this->lbLog, 500, true));
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Command);

	this->mailChanged = true;
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIREmailServerForm::~AVIREmailServerForm()
{
	SDEL_CLASS(this->smtpSvr);
	SDEL_CLASS(this->pop3Svr);
	SDEL_CLASS(this->gcisListener);
	SDEL_CLASS(this->gcisHdlr);

	this->log.RemoveLogHandler(this->logger);
	this->logger.Delete();
	
	DEL_CLASS(this->store);
	this->smtpSSL.Delete();
	SDEL_CLASS(this->smtpSSLCert);
	SDEL_CLASS(this->smtpSSLKey);
	this->ClearSMTPCACerts();
	this->pop3SSL.Delete();
	SDEL_CLASS(this->pop3SSLCert);
	SDEL_CLASS(this->pop3SSLKey);
	this->ClearPOP3CACerts();
	this->gcisSSL.Delete();
	SDEL_CLASS(this->gcisSSLCert);
	SDEL_CLASS(this->gcisSSLKey);
	this->ClearGCISCACerts();
	this->userList.FreeAll();
}

void SSWR::AVIRead::AVIREmailServerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Bool SSWR::AVIRead::AVIREmailServerForm::Login(Text::CStringNN user, Text::CStringNN pwd, OutParam<Int32> userId)
{
	Sync::MutexUsage mutUsage(this->userMut);
	UOSInt index = this->userMap.GetC(user);
	if (index == 0)
	{
		NotNullPtr<Text::String> str = Text::String::New(user);
		index = this->userList.Add(str) + 1;
		this->userMap.PutNN(str, index);
	}
	userId.Set((Int32)index);
	return true;
}

UOSInt SSWR::AVIRead::AVIREmailServerForm::GetMessageStat(Int32 userId, UOSInt *size)
{
	Net::Email::EmailStore::MessageStat stat;
	Optional<Text::String> userName = this->GetUserName(userId);
	this->store->GetMessageStat(OPTSTR_CSTR(userName), &stat);
	*size = (UOSInt)stat.unreadSize;
	return stat.unreadCount;
}

Bool SSWR::AVIRead::AVIREmailServerForm::GetUnreadList(Int32 userId, Data::ArrayList<UInt32> *unreadList)
{
	Data::ArrayList<UOSInt> unreadIndices;
	Optional<Text::String> userName = this->GetUserName(userId);
	this->store->GetUnreadIndices(OPTSTR_CSTR(userName), &unreadIndices);
	UOSInt i = 0;
	UOSInt j = unreadIndices.GetCount();
	while (i < j)
	{
		unreadList->Add((UInt32)unreadIndices.GetItem(i));
		i++;
	}
	return true;
}

Bool SSWR::AVIRead::AVIREmailServerForm::GetMessageInfo(Int32 userId, UInt32 msgId, MessageInfo *info)
{
	Optional<Text::String> userName = this->GetUserName(userId);
	Net::Email::EmailStore::EmailInfo *email = this->store->GetEmailByIndex(OPTSTR_CSTR(userName), msgId);
	if (email == 0)
		return false;
	info->size = email->fileSize;
	info->uid = this->store->GetEmailUid(email->id);
	return true;
}

Bool SSWR::AVIRead::AVIREmailServerForm::GetMessageContent(Int32 userId, UInt32 msgId, IO::Stream *stm)
{
	Optional<Text::String> userName = this->GetUserName(userId);
	Net::Email::EmailStore::EmailInfo *email = this->store->GetEmailByIndex(OPTSTR_CSTR(userName), msgId);
	if (email == 0)
		return false;
	IO::StreamData *fd = this->store->OpenEmailData(email->id);
	if (fd == 0)
		return false;
	
	Bool succ = false;
	UInt64 fileLength = fd->GetDataSize();
	if (fileLength < 1048576)
	{
		Data::ByteBuffer buff((UOSInt)fileLength);
		if (fd->GetRealData(0, (UOSInt)fileLength, buff) == fileLength)
		{
			stm->Write(buff.Ptr(), (UOSInt)fileLength);
			succ = true;
		}
	}
	else
	{
		UOSInt readSize;
		UInt64 ofst = 0;
		Data::ByteBuffer buff(1048576);
		succ = true;
		while (ofst < fileLength)
		{
			if (ofst + 1048576 > fileLength)
			{
				readSize = (UOSInt)(fileLength - ofst);
			}
			else
			{
				readSize = 1048576;
			}
			if (fd->GetRealData(ofst, readSize, buff) != readSize)
			{
				succ = false;
				break;
			}
			else if (stm->Write(buff.Ptr(), readSize) != readSize)
			{
				succ = false;
				break;
			}
			ofst += readSize;
		}
	}
	DEL_CLASS(fd);

	return succ;
}

Net::Email::MailController::RemoveStatus SSWR::AVIRead::AVIREmailServerForm::RemoveMessage(Int32 userId, UInt32 msgId)
{
	Optional<Text::String> userName = this->GetUserName(userId);
	return this->store->RemoveMessage(OPTSTR_CSTR(userName), msgId);
}
