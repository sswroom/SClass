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

void __stdcall SSWR::AVIRead::AVIREmailServerForm::OnSMTPStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIREmailServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIREmailServerForm>();
	if (me->smtpSvr.NotNull())
	{
		me->smtpSvr.Delete();
		me->txtSMTPPort->SetReadOnly(false);
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
		NN<Net::SSLEngine> ssl;
		NN<Crypto::Cert::X509Cert> smtpSSLCert;
		NN<Crypto::Cert::X509File> smtpSSLKey;
		if (me->smtpSSL.SetTo(ssl) && me->smtpSSLCert.SetTo(smtpSSLCert) && me->smtpSSLKey.SetTo(smtpSSLKey))
		{
			ssl->ServerSetCertsASN1(smtpSSLCert, smtpSSLKey, me->smtpCACerts);
		}
		Net::Email::SMTPConn::ConnType connType = (Net::Email::SMTPConn::ConnType)me->cboSMTPType->GetSelectedItem().GetIntOS();
		NN<Net::Email::SMTPServer> smtpSvr;
		NEW_CLASSNN(smtpSvr, Net::Email::SMTPServer(me->clif->GetSocketFactory(), me->smtpSSL, port, connType, me->log, SERVER_DOMAIN, CSTR("SSWRSMTP"), OnMailReceived, OnMailLogin, me, true));
		if (smtpSvr->IsError())
		{
			smtpSvr.Delete();
		}
		else
		{
			me->smtpSvr = smtpSvr;
			me->txtSMTPPort->SetReadOnly(true);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIREmailServerForm::OnPOP3StartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIREmailServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIREmailServerForm>();
	if (me->pop3Svr.NotNull())
	{
		me->pop3Svr.Delete();
		me->txtPOP3Port->SetReadOnly(false);
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
		Optional<Net::SSLEngine> ssl = nullptr;
		Bool sslConn = me->chkPOP3SSL->IsChecked();
		NN<Crypto::Cert::X509Cert> pop3SSLCert;
		NN<Crypto::Cert::X509File> pop3SSLKey;
		if (me->pop3SSLCert.SetTo(pop3SSLCert) && me->pop3SSLKey.SetTo(pop3SSLKey))
		{
			ssl = me->pop3SSL;
			NN<Net::SSLEngine> nnssl;
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
		NN<Net::Email::POP3Server> pop3Svr;
		NEW_CLASSNN(pop3Svr, Net::Email::POP3Server(me->core->GetSocketFactory(), ssl, sslConn, port, me->log, CSTR("Welcome to SSWR POP3 Server"), me, true));
		if (pop3Svr->IsError())
		{
			pop3Svr.Delete();
		}
		else
		{
			me->pop3Svr = pop3Svr;
			me->txtPOP3Port->SetReadOnly(true);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIREmailServerForm::OnGCISStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIREmailServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIREmailServerForm>();
	if (me->gcisListener.NotNull())
	{
		me->gcisListener.Delete();
		me->gcisHdlr.Delete();
		me->txtGCISPort->SetReadOnly(false);
		me->txtGCISNotifPath->SetReadOnly(false);
		me->txtGCISBatchUplPath->SetReadOnly(false);
		me->btnGCISStart->SetText(CSTR("Start"));
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
		Optional<Net::SSLEngine> ssl = nullptr;
		NN<Crypto::Cert::X509Cert> gcisSSLCert;
		NN<Crypto::Cert::X509File> gcisSSLKey;
		if (!me->gcisSSLCert.SetTo(gcisSSLCert) || !me->gcisSSLKey.SetTo(gcisSSLKey))
		{
			me->ui->ShowMsgOK(CSTR("Please select SSL Cert/Key"), CSTR("SMTP Server"), me);
			return;
		}
		ssl = me->gcisSSL;
		NN<Net::SSLEngine> nnssl;
		Optional<Crypto::Cert::X509Cert> issuerCert = Crypto::Cert::CertUtil::FindIssuer(gcisSSLCert);
		if (ssl.SetTo(nnssl))
		{
			nnssl->ServerSetRequireClientCert(Net::SSLEngine::ClientCertType::Optional);
			nnssl->ServerSetCertsASN1(gcisSSLCert, gcisSSLKey, me->gcisCACerts);
		}
		issuerCert.Delete();
		NN<Net::WebServer::GCISNotifyHandler> gcisHdlr;
		NN<Net::WebServer::WebListener> gcisListener;
		NEW_CLASSNN(gcisHdlr, Net::WebServer::GCISNotifyHandler(sb.ToCString(), sb2.ToCString(), OnGCISMailReceived, me, me->log));
		NEW_CLASSNN(gcisListener, Net::WebServer::WebListener(me->core->GetTCPClientFactory(), ssl, gcisHdlr, port, 60, 1, 2, CSTR("SSWRGCIS/1.0"), false, Net::WebServer::KeepAlive::Default, true));
		if (gcisListener->IsError())
		{
			gcisListener.Delete();
			gcisHdlr.Delete();
			me->gcisListener = nullptr;
		}
		else
		{
			me->gcisListener = gcisListener;
			me->gcisHdlr = gcisHdlr;
			me->txtGCISPort->SetReadOnly(true);
			me->txtGCISNotifPath->SetReadOnly(true);
			me->txtGCISBatchUplPath->SetReadOnly(true);
			me->btnGCISStart->SetText(CSTR("Stop"));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIREmailServerForm::OnLogFileClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIREmailServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIREmailServerForm>();
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

void __stdcall SSWR::AVIRead::AVIREmailServerForm::OnEmailDblClicked(AnyType userObj, UIntOS index)
{
	NN<SSWR::AVIRead::AVIREmailServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIREmailServerForm>();
	NN<Net::Email::EmailStore::EmailInfo> email;
	if (me->lvEmail->GetItem(index).GetOpt<Net::Email::EmailStore::EmailInfo>().SetTo(email))
	{
		Optional<Text::MIMEObj::MailMessage> mail;
		NN<Text::MIMEObj::MailMessage> nnmail;
		NN<IO::StreamData> fd;
		if (me->store->OpenEmailData(email->id).SetTo(fd))
		{
			mail = Text::MIMEObj::MailMessage::ParseFile(fd);
			fd.Delete();
		}
		else
		{
			mail = nullptr;
		}
		if (mail.SetTo(nnmail))
		{
			SSWR::AVIRead::AVIRMIMEViewerForm frm(nullptr, me->ui, me->core, nnmail);
			frm.ShowDialog(me);
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Error in loading file"), CSTR("Error"), me);
		}
	}
}

UnsafeArrayOpt<UTF8Char> __stdcall SSWR::AVIRead::AVIREmailServerForm::OnMailReceived(UnsafeArray<UTF8Char> queryId, AnyType userObj, NN<Net::TCPClient> cli, NN<const Net::Email::SMTPServer::MailStatus> mail)
{
	NN<SSWR::AVIRead::AVIREmailServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIREmailServerForm>();
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Received email from "));
	sptr = cli->GetRemoteName(sbuff).Or(sbuff);
	sb.AppendP(sbuff, sptr);
	Int64 id = me->store->NextEmailId();
	sb.AppendC(UTF8STRC(", id = "));
	sb.AppendI64(id);
	me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);

	Net::SocketUtil::AddressInfo remoteAddr;
	cli->GetRemoteAddr(remoteAddr);
	me->store->NewEmail(id, remoteAddr, SERVER_DOMAIN, mail);
	me->mailChanged = true;
	me->totalSize += (UIntOS)mail->dataStm->GetLength();
	return Text::StrInt64(queryId, id);
}

void __stdcall SSWR::AVIRead::AVIREmailServerForm::OnGCISMailReceived(AnyType userObj, NN<Net::NetConnection> cli, NN<const Text::MIMEObj::MailMessage> mail)
{
	NN<SSWR::AVIRead::AVIREmailServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIREmailServerForm>();
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Received email from "));
	if (cli->GetRemoteName(sbuff).SetTo(sptr))
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
	NN<Text::MIMEObject> content;
	if (mail->GetContent().SetTo(content))
	{
		IO::MemoryStream mstm;
		content->WriteStream(mstm);
		me->totalSize += (UIntOS)mstm.GetLength();
	}
}

Bool __stdcall SSWR::AVIRead::AVIREmailServerForm::OnMailLogin(AnyType userObj, Text::CStringNN userName, Text::CStringNN pwd)
{
	Text::StringBuilderUTF8 sb;
	NN<SSWR::AVIRead::AVIREmailServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIREmailServerForm>();
	sb.AppendC(UTF8STRC("User: "));
	sb.Append(userName);
	sb.AppendC(UTF8STRC(", Pwd: "));
	sb.Append(pwd);
	me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);
	return true;
}

void __stdcall SSWR::AVIRead::AVIREmailServerForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIREmailServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIREmailServerForm>();
	NN<Net::Email::EmailStore::EmailInfo> email;
	UIntOS i;
	UIntOS j;
	UIntOS k;
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	Text::StringBuilderUTF8 sb;
	Data::DateTime dt;
	dt.ToLocalTime();
	if (me->mailChanged)
	{
		me->mailChanged = false;
		Data::ArrayListNN<Net::Email::EmailStore::EmailInfo> emailList;
		Data::ArrayListStringNN rcptList;
		me->store->GetAllEmails(emailList);
		me->lvEmail->ClearItems();
		i = 0;
		j = emailList.GetCount();
		while (i < j)
		{
			email = emailList.GetItemNoCheck(i);
			sb.ClearStr();
			sb.AppendI64(email->id);
			k = me->lvEmail->AddItem(sb.ToCString(), email);
			dt.SetTicks(email->recvTime);
			sptr = dt.ToString(sbuff);
			me->lvEmail->SetSubItem(k, 1, CSTRP(sbuff, sptr));
			me->lvEmail->SetSubItem(k, 2, email->fromAddr);
			sptr = Net::SocketUtil::GetAddrName(sbuff, email->remoteAddr).Or(sbuff);
			me->lvEmail->SetSubItem(k, 3, CSTRP(sbuff, sptr));
			rcptList.Clear();
			me->store->GetRcptList(email->id, rcptList);
			sb.ClearStr();
			Data::ArrayIterator<NN<Text::String>> it = rcptList.Iterator();
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

void __stdcall SSWR::AVIRead::AVIREmailServerForm::OnSMTPCertKeyClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIREmailServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIREmailServerForm>();
	if (me->smtpSvr.NotNull())
	{
		me->ui->ShowMsgOK(CSTR("Cannot change Cert/Key when SMTP server is started"), CSTR("SMTP Server"), me);
		return;
	}
	SSWR::AVIRead::AVIRSSLCertKeyForm frm(nullptr, me->ui, me->core, me->smtpSSL, me->smtpSSLCert, me->smtpSSLKey, me->smtpCACerts);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		NN<Crypto::Cert::X509Cert> nnCert;
		NN<Crypto::Cert::X509File> nnKey;
		me->smtpSSLCert.Delete();
		me->smtpSSLKey.Delete();
		me->ClearSMTPCACerts();
		me->smtpSSLCert = frm.GetCert();
		me->smtpSSLKey = frm.GetKey();
		frm.GetCACerts(me->smtpCACerts);
		Text::StringBuilderUTF8 sb;
		if (me->smtpSSLCert.SetTo(nnCert)) nnCert->ToShortString(sb);
		sb.AppendC(UTF8STRC(", "));
		if (me->smtpSSLKey.SetTo(nnKey)) nnKey->ToShortString(sb);
		me->lblSMTPCertKey->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIREmailServerForm::OnPOP3CertKeyClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIREmailServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIREmailServerForm>();
	if (me->pop3Svr.NotNull())
	{
		me->ui->ShowMsgOK(CSTR("Cannot change Cert/Key when POP3 server is started"), CSTR("SMTP Server"), me);
		return;
	}
	SSWR::AVIRead::AVIRSSLCertKeyForm frm(nullptr, me->ui, me->core, me->pop3SSL, me->pop3SSLCert, me->pop3SSLKey, me->pop3CACerts);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		NN<Crypto::Cert::X509Cert> nnCert;
		NN<Crypto::Cert::X509File> nnKey;
		me->pop3SSLCert.Delete();
		me->pop3SSLKey.Delete();
		me->ClearPOP3CACerts();
		me->pop3SSLCert = frm.GetCert();
		me->pop3SSLKey = frm.GetKey();
		frm.GetCACerts(me->pop3CACerts);
		Text::StringBuilderUTF8 sb;
		if (me->pop3SSLCert.SetTo(nnCert)) nnCert->ToShortString(sb);
		sb.AppendC(UTF8STRC(", "));
		if (me->pop3SSLKey.SetTo(nnKey)) nnKey->ToShortString(sb);
		me->lblPOP3CertKey->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIREmailServerForm::OnGCISCertKeyClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIREmailServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIREmailServerForm>();
	if (me->gcisListener.NotNull())
	{
		me->ui->ShowMsgOK(CSTR("Cannot change Cert/Key when GCIS server is started"), CSTR("SMTP Server"), me);
		return;
	}
	SSWR::AVIRead::AVIRSSLCertKeyForm frm(nullptr, me->ui, me->core, me->gcisSSL, me->gcisSSLCert, me->gcisSSLKey, me->gcisCACerts);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		NN<Crypto::Cert::X509Cert> nnCert;
		NN<Crypto::Cert::X509File> nnKey;
		me->gcisSSLCert.Delete();
		me->gcisSSLKey.Delete();
		me->ClearGCISCACerts();
		me->gcisSSLCert = frm.GetCert();
		me->gcisSSLKey = frm.GetKey();
		frm.GetCACerts(me->gcisCACerts);
		Text::StringBuilderUTF8 sb;
		if (me->gcisSSLCert.SetTo(nnCert)) nnCert->ToShortString(sb);
		sb.AppendC(UTF8STRC(", "));
		if (me->gcisSSLKey.SetTo(nnKey)) nnKey->ToShortString(sb);
		me->lblGCISCertKey->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIREmailServerForm::OnSMTPTypeSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIREmailServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIREmailServerForm>();
	UInt16 port;
	Text::StringBuilderUTF8 sb;
	Net::Email::SMTPConn::ConnType newType = (Net::Email::SMTPConn::ConnType)me->cboSMTPType->GetSelectedItem().GetIntOS();
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

void __stdcall SSWR::AVIRead::AVIREmailServerForm::OnPOP3SSLChanged(AnyType userObj, Bool isChecked)
{
	NN<SSWR::AVIRead::AVIREmailServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIREmailServerForm>();
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
	UIntOS i = this->smtpCACerts.GetCount();
	while (i-- > 0)
	{
		this->smtpCACerts.GetItem(i).Delete();
	}
	this->smtpCACerts.Clear();
}

void SSWR::AVIRead::AVIREmailServerForm::ClearPOP3CACerts()
{
	UIntOS i = this->pop3CACerts.GetCount();
	while (i-- > 0)
	{
		this->pop3CACerts.GetItem(i).Delete();
	}
	this->pop3CACerts.Clear();
}

void SSWR::AVIRead::AVIREmailServerForm::ClearGCISCACerts()
{
	UIntOS i = this->gcisCACerts.GetCount();
	while (i-- > 0)
	{
		this->gcisCACerts.GetItem(i).Delete();
	}
	this->gcisCACerts.Clear();
}

SSWR::AVIRead::AVIREmailServerForm::AVIREmailServerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("Email Server"));

	this->core = core;
	this->clif = core->GetTCPClientFactory();
	this->smtpSSL = Net::SSLEngineFactory::Create(this->clif, true);
	this->smtpSSLCert = nullptr;
	this->smtpSSLKey = nullptr;
	this->smtpSvr = nullptr;
	this->pop3SSL = Net::SSLEngineFactory::Create(this->clif, true);
	this->pop3SSLCert = nullptr;
	this->pop3SSLKey = nullptr;
	this->pop3Svr = nullptr;
	this->gcisSSL = Net::SSLEngineFactory::Create(this->clif, true);
	this->gcisSSLCert = nullptr;
	this->gcisSSLKey = nullptr;
	this->gcisListener = nullptr;
	this->gcisHdlr = nullptr;
	this->totalSize = 0;
	this->recvSize = 0;
	this->smtpType = Net::Email::SMTPConn::ConnType::Plain;
	this->mailChanged = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASSNN(this->store, Net::Email::FileEmailStore());

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
	this->cboSMTPType->AddItem(CSTR("Plain"), (void*)(IntOS)Net::Email::SMTPConn::ConnType::Plain);
	this->cboSMTPType->AddItem(CSTR("STARTTLS"), (void*)(IntOS)Net::Email::SMTPConn::ConnType::STARTTLS);
	this->cboSMTPType->AddItem(CSTR("SSL"), (void*)(IntOS)Net::Email::SMTPConn::ConnType::SSL);
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
	this->lvEmail = ui->NewListView(this->tpEmail, UI::ListViewStyle::Table, 5);
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
	this->lbLog = ui->NewListBox(this->tpLog, false);
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);

	NEW_CLASSNN(this->logger, UI::ListBoxLogger(*this, this->lbLog, 500, true));
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Command);

	this->mailChanged = true;
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIREmailServerForm::~AVIREmailServerForm()
{
	this->smtpSvr.Delete();
	this->pop3Svr.Delete();
	this->gcisListener.Delete();
	this->gcisHdlr.Delete();

	this->log.RemoveLogHandler(this->logger);
	this->logger.Delete();
	
	this->store.Delete();
	this->smtpSSL.Delete();
	this->smtpSSLCert.Delete();
	this->smtpSSLKey.Delete();
	this->ClearSMTPCACerts();
	this->pop3SSL.Delete();
	this->pop3SSLCert.Delete();
	this->pop3SSLKey.Delete();
	this->ClearPOP3CACerts();
	this->gcisSSL.Delete();
	this->gcisSSLCert.Delete();
	this->gcisSSLKey.Delete();
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
	UIntOS index = this->userMap.GetC(user);
	if (index == 0)
	{
		NN<Text::String> str = Text::String::New(user);
		index = this->userList.Add(str) + 1;
		this->userMap.PutNN(str, index);
	}
	userId.Set((Int32)index);
	return true;
}

UIntOS SSWR::AVIRead::AVIREmailServerForm::GetMessageStat(Int32 userId, OutParam<UIntOS> size)
{
	Net::Email::EmailStore::MessageStat stat;
	Optional<Text::String> userName = this->GetUserName(userId);
	this->store->GetMessageStat(OPTSTR_CSTR(userName), stat);
	size.Set((UIntOS)stat.unreadSize);
	return stat.unreadCount;
}

Bool SSWR::AVIRead::AVIREmailServerForm::GetUnreadList(Int32 userId, NN<Data::ArrayListNative<UInt32>> unreadList)
{
	Data::ArrayListNative<UIntOS> unreadIndices;
	Optional<Text::String> userName = this->GetUserName(userId);
	this->store->GetUnreadIndices(OPTSTR_CSTR(userName), unreadIndices);
	UIntOS i = 0;
	UIntOS j = unreadIndices.GetCount();
	while (i < j)
	{
		unreadList->Add((UInt32)unreadIndices.GetItem(i));
		i++;
	}
	return true;
}

Bool SSWR::AVIRead::AVIREmailServerForm::GetMessageInfo(Int32 userId, UInt32 msgId, NN<MessageInfo> info)
{
	Optional<Text::String> userName = this->GetUserName(userId);
	NN<Net::Email::EmailStore::EmailInfo> email;
	if (!this->store->GetEmailByIndex(OPTSTR_CSTR(userName), msgId).SetTo(email))
		return false;
	info->size = email->fileSize;
	info->uid = this->store->GetEmailUid(email->id);
	return true;
}

Bool SSWR::AVIRead::AVIREmailServerForm::GetMessageContent(Int32 userId, UInt32 msgId, NN<IO::Stream> stm)
{
	Optional<Text::String> userName = this->GetUserName(userId);
	NN<Net::Email::EmailStore::EmailInfo> email;
	if (!this->store->GetEmailByIndex(OPTSTR_CSTR(userName), msgId).SetTo(email))
		return false;
	NN<IO::StreamData> fd;
	if (!this->store->OpenEmailData(email->id).SetTo(fd))
		return false;
	
	Bool succ = false;
	UInt64 fileLength = fd->GetDataSize();
	if (fileLength < 1048576)
	{
		Data::ByteBuffer buff((UIntOS)fileLength);
		if (fd->GetRealData(0, (UIntOS)fileLength, buff) == fileLength)
		{
			stm->Write(buff.WithSize((UIntOS)fileLength));
			succ = true;
		}
	}
	else
	{
		UIntOS readSize;
		UInt64 ofst = 0;
		Data::ByteBuffer buff(1048576);
		succ = true;
		while (ofst < fileLength)
		{
			if (ofst + 1048576 > fileLength)
			{
				readSize = (UIntOS)(fileLength - ofst);
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
			else if (stm->Write(buff.WithSize(readSize)) != readSize)
			{
				succ = false;
				break;
			}
			ofst += readSize;
		}
	}
	fd.Delete();

	return succ;
}

Net::Email::MailController::RemoveStatus SSWR::AVIRead::AVIREmailServerForm::RemoveMessage(Int32 userId, UInt32 msgId)
{
	Optional<Text::String> userName = this->GetUserName(userId);
	return this->store->RemoveMessage(OPTSTR_CSTR(userName), msgId);
}
