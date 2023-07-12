#include "Stdafx.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Net/WebUtil.h"
#include "Net/Email/FileEmailStore.h"
#include "Sync/MutexUsage.h"
#include "Text/MIMEObj/MailMessage.h"

Net::Email::FileEmailStore::FileInfo *Net::Email::FileEmailStore::GetFileInfo(Int64 id)
{
	Sync::MutexUsage mutUsage(&this->fileMut);
	return this->fileMap.Get(id);
}

void Net::Email::FileEmailStore::AddMail(const Text::MIMEObj::MailMessage *mail, UTF8Char *filePath, UTF8Char *fileNameStart, UTF8Char *filePathEnd, UInt64 fileSize)
{
	Data::ArrayList<Text::String*> rcptList;
	Text::StringBuilderUTF8 sb;
	Data::DateTime recvTime;
	Text::String *remoteIP = 0;
	Text::String *fromAddr = 0;
	recvTime.SetTicks(0);
	
	UOSInt i = 0;
	UOSInt j = mail->GetHeaderCount();
	UOSInt k;
	UOSInt l;
	while (i < j)
	{
		Text::String *name = mail->GetHeaderName(i);
		Text::String *value = mail->GetHeaderValue(i);

		if (name->EqualsICase(UTF8STRC("Received")))
		{
			k = value->LastIndexOf(';');
			if (k != INVALID_INDEX)
			{
				recvTime.SetValue(value->ToCString().Substring(k + 1).LTrim());
			}
			k = value->IndexOf(' ');
			if (k != INVALID_INDEX)
			{
				l = value->IndexOf(' ', k + 1);
				if (l != INVALID_INDEX)
				{
					SDEL_STRING(remoteIP);
					remoteIP = Text::String::New(value->v + k + 1, l - k - 1);
				}
			}
		}
		else if (name->EqualsICase(UTF8STRC("X-Apparently-To")))
		{
			k = value->IndexOf(';');
			sb.ClearStr();
			sb.AppendC(UTF8STRC("RCPT TO: <"));
			if (k > 0)
			{
				sb.AppendC(value->v, k);
			}
			else
			{
				sb.Append(value);
			}
			sb.AppendUTF8Char('>');
			rcptList.Add(Text::String::New(sb.ToCString()));
		}
		else if (name->EqualsICase(UTF8STRC("From")))
		{
			fromAddr = value->Clone();
		}
		i++;
	}

	if (fromAddr != 0 && remoteIP != 0 && recvTime.ToTicks() != 0 && rcptList.GetCount() > 0)
	{
		Int64 id;
		k = Text::StrIndexOfCharC(fileNameStart, (UOSInt)(filePathEnd - fileNameStart), '.');
		sb.ClearStr();
		sb.AppendC(fileNameStart, k);
		id = sb.ToInt64();
		EmailInfo *email;
		email = MemAlloc(EmailInfo, 1);
		email->id = id;
		Net::SocketUtil::GetIPAddr(remoteIP->ToCString(), &email->remoteAddr);
		email->fromAddr = fromAddr->Clone();
		email->recvTime = recvTime.ToTicks();
		email->isDeleted = false;
		email->fileSize = (UOSInt)fileSize;

		FileInfo *file;
		NEW_CLASS(file, FileInfo());
		file->id = id;
		file->fileName = Text::String::NewP(filePath, filePathEnd);
		file->uid = Text::StrCopyNewC(sb.ToString(), sb.GetLength());

		k = 0;
		l = rcptList.GetCount();
		while (k < l)
		{
			file->rcptList.Add(rcptList.GetItem(k)->Clone());
			k++;
		}

		Sync::MutexUsage mutUsage(&this->mailMut);
		this->mailList.Add(email);
		mutUsage.ReplaceMutex(&this->fileMut);
		this->fileMap.Put(file->id, file);
	}
	SDEL_STRING(remoteIP);
	SDEL_STRING(fromAddr);
	LIST_FREE_STRING(&rcptList);
}

Net::Email::FileEmailStore::FileEmailStore()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("SMTP"));
	IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
	this->currId = Data::DateTimeUtil::GetCurrTimeMillis();
	this->recvIndex = 0;

	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr2 = Text::StrConcatC(sptr, UTF8STRC("*.eml"));
	IO::Path::FindFileSession *sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
	if (sess)
	{
		IO::Path::PathType pt;
		UInt64 fileSize;
		while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, &fileSize)) != 0)
		{
			IO::StmData::FileData fd(CSTRP(sbuff, sptr2), false);
			Text::MIMEObj::MailMessage *mail = Text::MIMEObj::MailMessage::ParseFile(&fd);
			if (mail)
			{
				AddMail(mail, sbuff, sptr, sptr2, fileSize);
				DEL_CLASS(mail);
			}
		}
		IO::Path::FindFileClose(sess);
	}
}

Net::Email::FileEmailStore::~FileEmailStore()
{
	FileInfo *file;
	UOSInt i = this->fileMap.GetCount();
	UOSInt j;
	while (i-- > 0)
	{
		file = this->fileMap.GetItem(i);
		file->fileName->Release();
		Text::StrDelNew(file->uid);
		j = file->rcptList.GetCount();
		while (j-- > 0)
		{
			file->rcptList.GetItem(j)->Release();
		}
		DEL_CLASS(file);
	}
}

Int64 Net::Email::FileEmailStore::NextEmailId()
{
	Sync::MutexUsage mutUsage(&this->currIdMut);
	Int64 id = this->currId++;
	mutUsage.EndUse();
	return id;
}

Bool Net::Email::FileEmailStore::NewEmail(Int64 id, const Net::SocketUtil::AddressInfo *remoteAddr, Text::CString serverName, const Net::Email::SMTPServer::MailStatus *mail)
{
	Data::DateTime currTime;
	EmailInfo *email;
	UOSInt i;
	UOSInt j;
	Text::StringBuilderUTF8 sb;
	IO::Path::GetProcessFileName(&sb);
	IO::Path::AppendPath(&sb, UTF8STRC("SMTP"));
	sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
	sb.AppendI64(id);
	sb.AppendC(UTF8STRC(".eml"));
	email = MemAlloc(EmailInfo, 1);
	email->id = id;
	email->remoteAddr = *remoteAddr;
	email->fromAddr = mail->mailFrom->Clone();
	email->recvTime = Data::DateTimeUtil::GetCurrTimeMillis();
	email->isDeleted = false;
	email->fileSize = (UOSInt)mail->dataStm->GetLength();

	UTF8Char sbuff[64];
	UTF8Char *sptr;
	FileInfo *file;
	NEW_CLASS(file, FileInfo());
	file->id = id;
	file->fileName = Text::String::New(sb.ToCString());
	sptr = Text::StrInt64(sbuff, id);
	file->uid = Text::StrCopyNewC(sbuff, (UOSInt)(sptr - sbuff));

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Received: from "));
	sptr = Net::SocketUtil::GetAddrName(sbuff, remoteAddr);
	sb.AppendP(sbuff, sptr);
	sb.AppendC(UTF8STRC("\r\n by "));
	sb.Append(serverName);
	sb.AppendC(UTF8STRC(" with "));
	sb.AppendC(UTF8STRC("ESMTPS"));
	sb.AppendC(UTF8STRC("; "));
	currTime.SetTicks(email->recvTime);
	sptr = Net::WebUtil::Date2Str(sbuff, &currTime);
	sb.AppendP(sbuff, sptr);
	sb.AppendC(UTF8STRC("\r\n"));

	i = 0;
	j = mail->rcptTo.GetCount();
	while (i < j)
	{
		file->rcptList.Add(mail->rcptTo.GetItem(i)->Clone());

		sb.AppendC(UTF8STRC("X-Apparently-To: "));
		Text::CString rcptTo = mail->rcptTo.GetItem(i)->ToCString();
		if (rcptTo.StartsWith(UTF8STRC("RCPT TO:")))
		{
			rcptTo = rcptTo.Substring(8).LTrim();
		}
		if (rcptTo.StartsWith('<') && rcptTo.EndsWith('>'))
		{
			sb.AppendC(rcptTo.v + 1, rcptTo.leng - 2);
		}
		else
		{
			sb.Append(rcptTo);
		}
		sb.AppendC(UTF8STRC("; "));
		sb.AppendP(sbuff, sptr);
		sb.AppendC(UTF8STRC("\r\n"));

		i++;
	}
	{
		UOSInt buffSize;
		UInt8 *buff;
		IO::FileStream fs(file->fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		buff = mail->dataStm->GetBuff(&buffSize);
		fs.Write(sb.ToString(), sb.GetLength());
		fs.Write(buff, buffSize);
	}

	Sync::MutexUsage mutUsage(&this->mailMut);
	this->mailList.Add(email);
	mutUsage.ReplaceMutex(&this->fileMut);
	this->fileMap.Put(file->id, file);
	return true;
}

Bool Net::Email::FileEmailStore::NewEmail(Int64 id, const Net::SocketUtil::AddressInfo *remoteAddr, Text::CString serverName, const Text::MIMEObj::MailMessage *mail)
{
	Data::DateTime currTime;
	UOSInt i;
	UOSInt j;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	IO::Path::GetProcessFileName(&sb);
	IO::Path::AppendPath(&sb, UTF8STRC("SMTP"));
	sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
	sb.AppendI64(id);
	sb.AppendC(UTF8STRC(".eml"));
	currTime.SetCurrTimeUTC();

	FileInfo *file;
	NEW_CLASS(file, FileInfo());
	file->id = id;
	file->fileName = Text::String::New(sb.ToCString());
	sptr = Text::StrInt64(sbuff, id);
	file->uid = Text::StrCopyNewC(sbuff, (UOSInt)(sptr - sbuff));

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Received: from "));
	sptr = Net::SocketUtil::GetAddrName(sbuff, remoteAddr);
	sb.AppendP(sbuff, sptr);
	sb.AppendC(UTF8STRC("\r\n by "));
	sb.Append(serverName);
	sb.AppendC(UTF8STRC(" with "));
	sb.AppendC(UTF8STRC("ESMTPS"));
	sb.AppendC(UTF8STRC("; "));
	sptr = Net::WebUtil::Date2Str(sbuff, &currTime);
	sb.AppendP(sbuff, sptr);
	sb.AppendC(UTF8STRC("\r\n"));

	Data::ArrayList<Text::MIMEObj::MailMessage::MailAddress*> recpList;
	mail->GetRecpList(&recpList);
	i = 0;
	j = recpList.GetCount();
	while (i < j)
	{
		file->rcptList.Add(recpList.GetItem(i)->address->Clone());

		sb.AppendC(UTF8STRC("X-Apparently-To: "));
		Text::CString rcptTo = recpList.GetItem(i)->address->ToCString();
		if (rcptTo.StartsWith(UTF8STRC("RCPT TO:")))
		{
			rcptTo = rcptTo.Substring(8).LTrim();
		}
		if (rcptTo.StartsWith('<') && rcptTo.EndsWith('>'))
		{
			sb.AppendC(rcptTo.v + 1, rcptTo.leng - 2);
		}
		else
		{
			sb.Append(rcptTo);
		}
		sb.AppendC(UTF8STRC("; "));
		sb.AppendP(sbuff, sptr);
		sb.AppendC(UTF8STRC("\r\n"));

		i++;
	}
	UInt64 fileSize = sb.GetLength();
	{
		IO::FileStream fs(file->fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		fs.Write(sb.ToString(), sb.GetLength());
		fileSize += mail->WriteStream(&fs);
	}

	EmailInfo *email;
	email = MemAlloc(EmailInfo, 1);
	email->id = id;
	email->remoteAddr = *remoteAddr;
	sptr = mail->GetFromAddr(sbuff);
	email->fromAddr = Text::String::NewP(sbuff, sptr);
	email->recvTime = currTime.ToTicks();
	email->isDeleted = false;
	email->fileSize = (UOSInt)fileSize;
	Sync::MutexUsage mutUsage(&this->mailMut);
	this->mailList.Add(email);
	mutUsage.ReplaceMutex(&this->fileMut);
	this->fileMap.Put(file->id, file);
	return true;
}

IO::StreamData *Net::Email::FileEmailStore::OpenEmailData(Int64 id)
{
	FileInfo *fileInfo = this->GetFileInfo(id);
	if (fileInfo == 0)
		return 0;
	return NEW_CLASS_D(IO::StmData::FileData(fileInfo->fileName, false));
}

const UTF8Char *Net::Email::FileEmailStore::GetEmailUid(Int64 id)
{
	FileInfo *fileInfo = this->GetFileInfo(id);
	if (fileInfo == 0)
		return 0;
	return fileInfo->uid;
}

UOSInt Net::Email::FileEmailStore::GetRcptList(Int64 id, Data::ArrayList<Text::String*> *rcptList)
{
	Sync::MutexUsage mutUsage(&this->fileMut);
	FileInfo *fileInfo = this->fileMap.Get(id);
	if (fileInfo == 0)
		return 0;
	return rcptList->AddAll(&fileInfo->rcptList);
}

Net::Email::MailController::RemoveStatus Net::Email::FileEmailStore::RemoveMessage(Text::CString userName, UOSInt msgIndex)
{
	Net::Email::EmailStore::EmailInfo *email;
	Sync::MutexUsage mutUsage(&this->mailMut);
	email = this->mailList.GetItem(msgIndex);
	if (email == 0)
		return Net::Email::MailController::RS_NOT_FOUND;
	if (email->isDeleted)
	{
		return Net::Email::MailController::RS_ALREADY_DELETED;
	}
	else
	{
		email->isDeleted = true;
		return Net::Email::MailController::RS_SUCCESS;
	}
}

Net::Email::EmailStore::EmailInfo *Net::Email::FileEmailStore::GetEmailByIndex(Text::CString userName, UOSInt msgIndex)
{
	Sync::MutexUsage mutUsage(&this->mailMut);
	return this->mailList.GetItem(msgIndex);
}

void Net::Email::FileEmailStore::GetMessageStat(Text::CString userName, MessageStat *stat)
{
	UOSInt unreadCount;
	UInt64 unreadSize;
	UOSInt totalCnt;
	UOSInt i;
	EmailInfo *email;
	unreadSize = 0;
	unreadCount = 0;

	Sync::MutexUsage mutUsage(&this->mailMut);
	totalCnt = this->mailList.GetCount();
	i = this->recvIndex;
	while (i < totalCnt)
	{
		email = this->mailList.GetItem(i);
		if (!email->isDeleted)
		{
			unreadCount++;
			unreadSize += email->fileSize;
		}
		i++;
	}
	stat->unreadCount = unreadCount;
	stat->unreadSize = unreadSize;
}

UOSInt Net::Email::FileEmailStore::GetUnreadIndices(Text::CString userName, Data::ArrayList<UOSInt> *indices)
{
	UOSInt totalCnt;
	UOSInt i;
	EmailInfo *email;

	Sync::MutexUsage mutUsage(&this->mailMut);
	totalCnt = this->mailList.GetCount();
	i = this->recvIndex;
	while (i < totalCnt)
	{
		email = this->mailList.GetItem(i);
		if (!email->isDeleted)
		{
			indices->Add(i);
		}
		i++;
	}
	return totalCnt;
}
