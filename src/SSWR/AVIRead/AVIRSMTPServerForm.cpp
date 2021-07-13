#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "SSWR/AVIRead/AVIRMIMEViewerForm.h"
#include "SSWR/AVIRead/AVIRSMTPServerForm.h"
#include "Sync/MutexUsage.h"
#include "Text/MIMEObj/MailMessage.h"
#include "Text/MyString.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRSMTPServerForm::OnSMTPStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSMTPServerForm *me = (SSWR::AVIRead::AVIRSMTPServerForm*)userObj;
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
		me->txtSMTPPort->GetText(&sb);
		if (!sb.ToUInt16(&port))
		{
			UI::MessageDialog::ShowDialog((const UTF8Char *)"Please enter valid port number", (const UTF8Char *)"Error", me);
			return;
		}
		if (port == 0 || port > 65535)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char *)"Please enter valid port number", (const UTF8Char *)"Error", me);
			return;
		}
		NEW_CLASS(me->smtpSvr, Net::SMTPServer(me->core->GetSocketFactory(), port, me->log, (const UTF8Char *)"127.0.0.1", (const UTF8Char *)"SSWRSMTP", OnMailReceived, OnMailLogin, me));
		if (me->smtpSvr->IsError())
		{
			DEL_CLASS(me->smtpSvr);
		}
		me->txtSMTPPort->SetReadOnly(true);
	}
}

void __stdcall SSWR::AVIRead::AVIRSMTPServerForm::OnPOP3StartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSMTPServerForm *me = (SSWR::AVIRead::AVIRSMTPServerForm*)userObj;
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
		me->txtPOP3Port->GetText(&sb);
		if (!sb.ToUInt16(&port))
		{
			UI::MessageDialog::ShowDialog((const UTF8Char *)"Please enter valid port number", (const UTF8Char *)"Error", me);
			return;
		}
		if (port == 0 || port > 65535)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char *)"Please enter valid port number", (const UTF8Char *)"Error", me);
			return;
		}
		NEW_CLASS(me->pop3Svr, Net::POP3Server(me->core->GetSocketFactory(), port, me->log, (const UTF8Char *)"Welcome to SSWR POP3 Server", me));
		if (me->pop3Svr->IsError())
		{
			DEL_CLASS(me->pop3Svr);
		}
		me->txtPOP3Port->SetReadOnly(true);
	}
}

void __stdcall SSWR::AVIRead::AVIRSMTPServerForm::OnLogFileClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSMTPServerForm *me = (SSWR::AVIRead::AVIRSMTPServerForm*)userObj;
	Text::StringBuilderUTF8 sb;
	IO::Path::GetProcessFileName(&sb);
	IO::Path::AppendPath(&sb, (const UTF8Char*)"log\\SMTP");
	me->log->AddFileLog(sb.ToString(), IO::ILogHandler::LOG_TYPE_PER_DAY, IO::ILogHandler::LOG_GROUP_TYPE_PER_MONTH, IO::ILogHandler::LOG_LEVEL_COMMAND, "yyyy-MM-dd HH:mm:ss.fff", false);
	me->btnLogFile->SetEnabled(false);
}

void __stdcall SSWR::AVIRead::AVIRSMTPServerForm::OnEmailDblClicked(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRSMTPServerForm *me = (SSWR::AVIRead::AVIRSMTPServerForm*)userObj;
	EmailInfo *email;
	email = (EmailInfo*)me->lvEmail->GetItem(index);
	if (email)
	{
		Text::MIMEObj::MailMessage *mail;
		IO::StmData::FileData *fd;
		NEW_CLASS(fd, IO::StmData::FileData(email->fileName, false));
		mail = Text::MIMEObj::MailMessage::ParseFile(fd);
		DEL_CLASS(fd);
		if (mail)
		{
			SSWR::AVIRead::AVIRMIMEViewerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRMIMEViewerForm(0, me->ui, me->core, mail));
			frm->ShowDialog(me);
			DEL_CLASS(frm);
		}
		else
		{
			UI::MessageDialog::ShowDialog((const UTF8Char *)"Error in loading file", (const UTF8Char *)"Error", me);
		}
	}
}

UTF8Char *__stdcall SSWR::AVIRead::AVIRSMTPServerForm::OnMailReceived(UTF8Char *queryId, void *userObj, Net::TCPClient *cli, Net::SMTPServer::MailStatus *mail)
{
	SSWR::AVIRead::AVIRSMTPServerForm *me = (SSWR::AVIRead::AVIRSMTPServerForm*)userObj;
	UTF8Char sbuff[32];
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)"Received email from ");
	cli->GetRemoteName(sbuff);
	sb.Append(sbuff);
	Int64 id = me->NextEmailId();
	sb.Append((const UTF8Char*)", id = ");
	sb.AppendI64(id);
	me->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_COMMAND);

	Data::DateTime currTime;
	EmailInfo *email;
	IO::FileStream *fs;
	UOSInt buffSize;
	UInt8 *buff;
	UOSInt i;
	UOSInt j;
	sb.ClearStr();
	IO::Path::GetProcessFileName(&sb);
	IO::Path::AppendPath(&sb, (const UTF8Char*)"SMTP");
	sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
	sb.AppendI64(id);
	sb.Append((const UTF8Char*)".eml");
	email = MemAlloc(EmailInfo, 1);
	email->id = id;
	cli->GetRemoteAddr(&email->remoteAddr);
	email->remotePort = cli->GetRemotePort();
	NEW_CLASS(email->rcptList, Data::ArrayList<const UTF8Char*>());
	email->fileName = Text::StrCopyNew(sb.ToString());
	sb.ClearStr();
	sb.Append(mail->mailFrom);
	email->fromAddr = Text::StrCopyNew(sb.ToString());
	currTime.SetCurrTimeUTC();
	email->recvTime = currTime.ToTicks();
	email->isDeleted = false;
	Text::StrInt64(sbuff, id);
	email->uid = Text::StrCopyNew(sbuff);
	i = 0;
	j = mail->rcptTo->GetCount();
	while (i < j)
	{
		sb.ClearStr();
		sb.Append(mail->rcptTo->GetItem(i));
		email->rcptList->Add(Text::StrCopyNew(sb.ToString()));
		i++;
	}
	NEW_CLASS(fs, IO::FileStream(email->fileName, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	buff = mail->dataStm->GetBuff(&buffSize);
	fs->Write(buff, buffSize);
	DEL_CLASS(fs);
	email->fileSize = buffSize;

	Sync::MutexUsage mutUsage(me->mailMut);
	me->mailList->Add(email);
	me->mailChanged = true;
	me->totalSize += buffSize;
	mutUsage.EndUse();

	return Text::StrInt64(queryId, id);
}

Bool __stdcall SSWR::AVIRead::AVIRSMTPServerForm::OnMailLogin(void *userObj, const UTF8Char *userName, const UTF8Char *pwd)
{
	Text::StringBuilderUTF8 sb;
	SSWR::AVIRead::AVIRSMTPServerForm *me = (SSWR::AVIRead::AVIRSMTPServerForm *)userObj;
	sb.Append((const UTF8Char*)"User: ");
	sb.Append(userName);
	sb.Append((const UTF8Char*)", Pwd: ");
	sb.Append(pwd);
	me->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_COMMAND);
	return true;
}

void __stdcall SSWR::AVIRead::AVIRSMTPServerForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRSMTPServerForm *me = (SSWR::AVIRead::AVIRSMTPServerForm*)userObj;
	EmailInfo *email;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	UOSInt m;
	UTF8Char sbuff[32];
	Text::StringBuilderUTF8 sb;
	Data::DateTime dt;
	dt.ToLocalTime();
	if (me->mailChanged)
	{
		me->mailChanged = false;
		Sync::MutexUsage mutUsage(me->mailMut);
		me->lvEmail->ClearItems();
		i = 0;
		j = me->mailList->GetCount();
		while (i < j)
		{
			email = me->mailList->GetItem(i);
			sb.ClearStr();
			sb.AppendI64(email->id);
			k = me->lvEmail->AddItem(sb.ToString(), email);
			dt.SetTicks(email->recvTime);
			dt.ToString(sbuff);
			me->lvEmail->SetSubItem(k, 1, sbuff);
			me->lvEmail->SetSubItem(k, 2, email->fromAddr);
			Net::SocketUtil::GetAddrName(sbuff, &email->remoteAddr);
			me->lvEmail->SetSubItem(k, 3, sbuff);
			sb.ClearStr();
			l = 0;
			m = email->rcptList->GetCount();
			while (l < m)
			{
				if (l != 0)
				{
					sb.Append((const UTF8Char*)", ");
				}
				sb.Append(email->rcptList->GetItem(l));
				l++;
			}
			me->lvEmail->SetSubItem(k, 4, sb.ToString());
			i++;
		}
		mutUsage.EndUse();
	}
}

Int64 SSWR::AVIRead::AVIRSMTPServerForm::NextEmailId()
{
	Sync::MutexUsage mutUsage(this->currIdMut);
	Int64 id = this->currId++;
	mutUsage.EndUse();
	return id;
}

SSWR::AVIRead::AVIRSMTPServerForm::AVIRSMTPServerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"SMTP Server");

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->smtpSvr = 0;
	this->pop3Svr = 0;
	this->totalSize = 0;
	this->recvIndex = 0;
	this->recvSize = 0;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	this->currId = dt.ToTicks();
	NEW_CLASS(this->currIdMut, Sync::Mutex());
	NEW_CLASS(this->mailList, Data::ArrayList<EmailInfo*>());
	NEW_CLASS(this->mailMut, Sync::Mutex());
	this->mailChanged = false;
	Text::StringBuilderUTF8 sb;
	IO::Path::GetProcessFileName(&sb);
	IO::Path::AppendPath(&sb, (const UTF8Char*)"SMTP");
	IO::Path::CreateDirectory(sb.ToString());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpControl = this->tcMain->AddTabPage((const UTF8Char*)"Control");
	this->tpEmail = this->tcMain->AddTabPage((const UTF8Char*)"Email");
	this->tpLog = this->tcMain->AddTabPage((const UTF8Char*)"Log");

	NEW_CLASS(this->grpSMTP, UI::GUIGroupBox(ui, this->tpControl, (const UTF8Char*)"SMTP"));
	this->grpSMTP->SetRect(0, 0, 250, 64, false);
	NEW_CLASS(this->lblSMTPPort, UI::GUILabel(ui, this->grpSMTP, (const UTF8Char*)"Port"));
	this->lblSMTPPort->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->txtSMTPPort, UI::GUITextBox(ui, this->grpSMTP, (const UTF8Char*)"25"));
	this->txtSMTPPort->SetRect(100, 0, 100, 23, false);
	NEW_CLASS(this->btnSMTPStart, UI::GUIButton(ui, this->grpSMTP, (const UTF8Char*)"Start"));
	this->btnSMTPStart->SetRect(100, 24, 75, 23, false);
	this->btnSMTPStart->HandleButtonClick(OnSMTPStartClicked, this);
	NEW_CLASS(this->grpPOP3, UI::GUIGroupBox(ui, this->tpControl, (const UTF8Char*)"POP3"));
	this->grpPOP3->SetRect(250, 0, 250, 64, false);
	NEW_CLASS(this->lblPOP3Port, UI::GUILabel(ui, this->grpPOP3, (const UTF8Char*)"Port"));
	this->lblPOP3Port->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->txtPOP3Port, UI::GUITextBox(ui, this->grpPOP3, (const UTF8Char*)"110"));
	this->txtPOP3Port->SetRect(100, 0, 100, 23, false);
	NEW_CLASS(this->btnPOP3Start, UI::GUIButton(ui, this->grpPOP3, (const UTF8Char*)"Start"));
	this->btnPOP3Start->SetRect(100, 24, 75, 23, false);
	this->btnPOP3Start->HandleButtonClick(OnPOP3StartClicked, this);
	NEW_CLASS(this->btnLogFile, UI::GUIButton(ui, this->tpControl, (const UTF8Char*)"Log To File"));
	this->btnLogFile->SetRect(4, 64, 75, 23, false);

	NEW_CLASS(this->lvEmail, UI::GUIListView(ui, this->tpEmail, UI::GUIListView::LVSTYLE_TABLE, 5));
	this->lvEmail->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvEmail->SetShowGrid(true);
	this->lvEmail->SetFullRowSelect(true);
	this->lvEmail->AddColumn((const UTF8Char*)"Id", 100);
	this->lvEmail->AddColumn((const UTF8Char*)"Time", 150);
	this->lvEmail->AddColumn((const UTF8Char*)"From", 150);
	this->lvEmail->AddColumn((const UTF8Char*)"Remote IP", 100);
	this->lvEmail->AddColumn((const UTF8Char*)"Rcpt", 200);
	this->lvEmail->HandleDblClk(OnEmailDblClicked, this);

	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this->tpLog, (const UTF8Char*)""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);

	NEW_CLASS(this->log, IO::LogTool());
	NEW_CLASS(this->logger, UI::ListBoxLogger(this, this->lbLog, 500, true));
	this->log->AddLogHandler(this->logger, IO::ILogHandler::LOG_LEVEL_COMMAND);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRSMTPServerForm::~AVIRSMTPServerForm()
{
	UOSInt i;
	UOSInt j;
	EmailInfo *email;
	SDEL_CLASS(this->smtpSvr);
	SDEL_CLASS(this->pop3Svr);

	DEL_CLASS(this->log);
	DEL_CLASS(this->logger);
	
	i = this->mailList->GetCount();
	while (i-- > 0)
	{
		email = this->mailList->GetItem(i);
		Text::StrDelNew(email->fromAddr);
		Text::StrDelNew(email->fileName);
		Text::StrDelNew(email->uid);
		j = email->rcptList->GetCount();
		while (j-- > 0)
		{
			Text::StrDelNew(email->rcptList->GetItem(j));
		}
		DEL_CLASS(email->rcptList);
		MemFree(email);
	}
	DEL_CLASS(this->mailList);
	DEL_CLASS(this->mailMut);
	DEL_CLASS(this->currIdMut);
}

void SSWR::AVIRead::AVIRSMTPServerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Bool SSWR::AVIRead::AVIRSMTPServerForm::Login(const Char *user, const Char *pwd, Int32 *userId)
{
	*userId = 1;
	return true;
}

UOSInt SSWR::AVIRead::AVIRSMTPServerForm::GetMessageStat(Int32 userId, UOSInt *size)
{
	UOSInt retCnt;
	UOSInt retSize;
	UOSInt totalCnt;
	UOSInt i;
	EmailInfo *email;
	retSize = 0;
	retCnt = 0;

	Sync::MutexUsage mutUsage(this->mailMut);
	totalCnt = this->mailList->GetCount();
	i = this->recvIndex;
	while (i < totalCnt)
	{
		email = this->mailList->GetItem(i);
		if (!email->isDeleted)
		{
			retCnt++;
			retSize += email->fileSize;
		}
		i++;
	}
	mutUsage.EndUse();
	*size = retSize;
	return retCnt;
}

Bool SSWR::AVIRead::AVIRSMTPServerForm::GetUnreadList(Int32 userId, Data::ArrayList<UInt32> *unreadList)
{
	UOSInt totalCnt;
	UOSInt i;
	EmailInfo *email;

	Sync::MutexUsage mutUsage(this->mailMut);
	totalCnt = this->mailList->GetCount();
	i = this->recvIndex;
	while (i < totalCnt)
	{
		email = this->mailList->GetItem(i);
		if (!email->isDeleted)
		{
			unreadList->Add((UInt32)i);
		}
		i++;
	}
	mutUsage.EndUse();
	return true;
}

Bool SSWR::AVIRead::AVIRSMTPServerForm::GetMessageInfo(Int32 userId, UInt32 msgId, MessageInfo *info)
{
	EmailInfo *email;
	Bool succ = false;

	Sync::MutexUsage mutUsage(this->mailMut);
	email = this->mailList->GetItem(msgId);
	if (email)
	{
		succ = true;
		info->size = email->fileSize;
		info->uid = email->uid;
	}
	mutUsage.EndUse();
	return succ;
}

Bool SSWR::AVIRead::AVIRSMTPServerForm::GetMessageContent(Int32 userId, UInt32 msgId, IO::Stream *stm)
{
	EmailInfo *email;
	Bool succ = false;
	IO::FileStream *fs;

	Sync::MutexUsage mutUsage(this->mailMut);
	email = this->mailList->GetItem(msgId);
	if (email)
	{
		NEW_CLASS(fs, IO::FileStream(email->fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NO_BUFFER));
		if (fs->IsError())
		{

		}
		else
		{
			UInt8 *buff;
			UOSInt readSize;
			buff = MemAlloc(UInt8, 1048576);
			while ((readSize = fs->Read(buff, 1048576)) > 0)
			{
				stm->Write(buff, readSize);
			}
			MemFree(buff);

			succ = true;
		}
		DEL_CLASS(fs);
	}
	mutUsage.EndUse();
	return succ;
}

SSWR::AVIRead::AVIRSMTPServerForm::RemoveStatus SSWR::AVIRead::AVIRSMTPServerForm::RemoveMessage(Int32 userId, UInt32 msgId)
{
	EmailInfo *email;
	SSWR::AVIRead::AVIRSMTPServerForm::RemoveStatus ret = RS_NOT_FOUND;

	Sync::MutexUsage mutUsage(this->mailMut);
	email = this->mailList->GetItem(msgId);
	if (email)
	{
		if (email->isDeleted)
		{
			ret = RS_ALREADY_DELETED;
		}
		else
		{
			email->isDeleted = true;
			ret = RS_SUCCESS;
		}
	}
	mutUsage.EndUse();
	return ret;
}
