#include "Stdafx.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Net/WebUtil.h"
#include "Net/Email/FileEmailStore.h"
#include "Sync/MutexUsage.h"
#include "Text/MIMEObj/MailMessage.h"

Optional<Net::Email::FileEmailStore::FileInfo> Net::Email::FileEmailStore::GetFileInfo(Int64 id)
{
	Sync::MutexUsage mutUsage(this->fileMut);
	return this->fileMap.Get(id);
}

void Net::Email::FileEmailStore::AddMail(NN<const Text::MIMEObj::MailMessage> mail, UnsafeArray<UTF8Char> filePath, UnsafeArray<UTF8Char> fileNameStart, UnsafeArray<UTF8Char> filePathEnd, UInt64 fileSize)
{
	Data::ArrayListStringNN rcptList;
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
		NN<Text::String> name;
		NN<Text::String> value;
		if (mail->GetHeaderName(i).SetTo(name) && mail->GetHeaderValue(i).SetTo(value))
		{
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
						remoteIP = Text::String::New(value->v + k + 1, l - k - 1).Ptr();
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
				fromAddr = value->Clone().Ptr();
			}
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
		NN<EmailInfo> email;
		email = MemAllocNN(EmailInfo);
		email->id = id;
		Net::SocketUtil::SetAddrInfo(email->remoteAddr, remoteIP->ToCString());
		email->fromAddr = fromAddr->Clone();
		email->recvTime = recvTime.ToTicks();
		email->isDeleted = false;
		email->fileSize = (UOSInt)fileSize;

		NN<FileInfo> file;
		NEW_CLASSNN(file, FileInfo());
		file->id = id;
		file->fileName = Text::String::NewP(filePath, filePathEnd);
		file->uid = Text::StrCopyNewC(sb.ToString(), sb.GetLength()).Ptr();

		Data::ArrayIterator<NN<Text::String>> it = rcptList.Iterator();
		while (it.HasNext())
		{
			file->rcptList.Add(it.Next()->Clone());
		}

		Sync::MutexUsage mutUsage(this->mailMut);
		this->mailList.Add(email);
		mutUsage.ReplaceMutex(this->fileMut);
		this->fileMap.Put(file->id, file);
	}
	SDEL_STRING(remoteIP);
	SDEL_STRING(fromAddr);
	rcptList.FreeAll();
}

Net::Email::FileEmailStore::FileEmailStore()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
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
		while (IO::Path::FindNextFile(sptr, sess, 0, &pt, &fileSize).SetTo(sptr2))
		{
			IO::StmData::FileData fd(CSTRP(sbuff, sptr2), false);
			NN<Text::MIMEObj::MailMessage> mail;
			if (Text::MIMEObj::MailMessage::ParseFile(fd).SetTo(mail))
			{
				AddMail(mail, sbuff, sptr, sptr2, fileSize);
				mail.Delete();
			}
		}
		IO::Path::FindFileClose(sess);
	}
}

Net::Email::FileEmailStore::~FileEmailStore()
{
	NN<FileInfo> file;
	UOSInt i = this->fileMap.GetCount();
	UOSInt j;
	while (i-- > 0)
	{
		file = this->fileMap.GetItemNoCheck(i);
		file->fileName->Release();
		Text::StrDelNew(file->uid);
		j = file->rcptList.GetCount();
		while (j-- > 0)
		{
			OPTSTR_DEL(file->rcptList.GetItem(j));
		}
		file.Delete();
	}
}

Int64 Net::Email::FileEmailStore::NextEmailId()
{
	Sync::MutexUsage mutUsage(this->currIdMut);
	Int64 id = this->currId++;
	mutUsage.EndUse();
	return id;
}

Bool Net::Email::FileEmailStore::NewEmail(Int64 id, NN<const Net::SocketUtil::AddressInfo> remoteAddr, Text::CStringNN serverName, NN<const Net::Email::SMTPServer::MailStatus> mail)
{
	Data::DateTime currTime;
	NN<EmailInfo> email;
	UOSInt i;
	UOSInt j;
	Text::StringBuilderUTF8 sb;
	IO::Path::GetProcessFileName(sb);
	IO::Path::AppendPath(sb, UTF8STRC("SMTP"));
	sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
	sb.AppendI64(id);
	sb.AppendC(UTF8STRC(".eml"));
	email = MemAllocNN(EmailInfo);
	email->id = id;
	email->remoteAddr = remoteAddr.Ptr()[0];
	email->fromAddr = mail->mailFrom->Clone();
	email->recvTime = Data::DateTimeUtil::GetCurrTimeMillis();
	email->isDeleted = false;
	email->fileSize = (UOSInt)mail->dataStm->GetLength();

	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	NN<FileInfo> file;
	NEW_CLASSNN(file, FileInfo());
	file->id = id;
	file->fileName = Text::String::New(sb.ToCString());
	sptr = Text::StrInt64(sbuff, id);
	file->uid = Text::StrCopyNewC(sbuff, (UOSInt)(sptr - sbuff)).Ptr();

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Received: from "));
	sptr = Net::SocketUtil::GetAddrName(sbuff, remoteAddr).Or(sbuff);
	sb.AppendP(sbuff, sptr);
	sb.AppendC(UTF8STRC("\r\n by "));
	sb.Append(serverName);
	sb.AppendC(UTF8STRC(" with "));
	sb.AppendC(UTF8STRC("ESMTPS"));
	sb.AppendC(UTF8STRC("; "));
	currTime.SetTicks(email->recvTime);
	sptr = Net::WebUtil::Date2Str(sbuff, currTime);
	sb.AppendP(sbuff, sptr);
	sb.AppendC(UTF8STRC("\r\n"));

	i = 0;
	j = mail->rcptTo.GetCount();
	while (i < j)
	{
		file->rcptList.Add(mail->rcptTo.GetItem(i)->Clone());

		sb.AppendC(UTF8STRC("X-Apparently-To: "));
		Text::CStringNN rcptTo = mail->rcptTo.GetItem(i)->ToCString();
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
		buff = mail->dataStm->GetBuff(buffSize);
		fs.Write(sb.ToByteArray());
		fs.Write(Data::ByteArrayR(buff, buffSize));
	}

	Sync::MutexUsage mutUsage(this->mailMut);
	this->mailList.Add(email);
	mutUsage.ReplaceMutex(this->fileMut);
	this->fileMap.Put(file->id, file);
	return true;
}

Bool Net::Email::FileEmailStore::NewEmail(Int64 id, NN<const Net::SocketUtil::AddressInfo> remoteAddr, Text::CStringNN serverName, NN<const Text::MIMEObj::MailMessage> mail)
{
	Data::DateTime currTime;
	UOSInt i;
	UOSInt j;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Text::StringBuilderUTF8 sb;
	IO::Path::GetProcessFileName(sb);
	IO::Path::AppendPath(sb, UTF8STRC("SMTP"));
	sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
	sb.AppendI64(id);
	sb.AppendC(UTF8STRC(".eml"));
	currTime.SetCurrTimeUTC();

	NN<FileInfo> file;
	NEW_CLASSNN(file, FileInfo());
	file->id = id;
	file->fileName = Text::String::New(sb.ToCString());
	sptr = Text::StrInt64(sbuff, id);
	file->uid = Text::StrCopyNewC(sbuff, (UOSInt)(sptr - sbuff)).Ptr();

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Received: from "));
	sptr = Net::SocketUtil::GetAddrName(sbuff, remoteAddr).Or(sbuff);
	sb.AppendP(sbuff, sptr);
	sb.AppendC(UTF8STRC("\r\n by "));
	sb.Append(serverName);
	sb.AppendC(UTF8STRC(" with "));
	sb.AppendC(UTF8STRC("ESMTPS"));
	sb.AppendC(UTF8STRC("; "));
	sptr = Net::WebUtil::Date2Str(sbuff, currTime);
	sb.AppendP(sbuff, sptr);
	sb.AppendC(UTF8STRC("\r\n"));

	Data::ArrayListNN<Text::MIMEObj::MailMessage::MailAddress> recpList;
	mail->GetRecpList(recpList);
	i = 0;
	j = recpList.GetCount();
	while (i < j)
	{
		file->rcptList.Add(recpList.GetItemNoCheck(i)->address->Clone());

		sb.AppendC(UTF8STRC("X-Apparently-To: "));
		Text::CStringNN rcptTo = recpList.GetItemNoCheck(i)->address->ToCString();
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
	mail->FreeRecpList(recpList);
	UInt64 fileSize = sb.GetLength();
	{
		IO::FileStream fs(file->fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		fs.Write(sb.ToByteArray());
		fileSize += mail->WriteStream(fs);
	}

	NN<EmailInfo> email;
	email = MemAllocNN(EmailInfo);
	email->id = id;
	email->remoteAddr = remoteAddr.Ptr()[0];
	sbuff[0] = 0;
	sptr = mail->GetFromAddr(sbuff).Or(sbuff);
	email->fromAddr = Text::String::NewP(sbuff, sptr);
	email->recvTime = currTime.ToTicks();
	email->isDeleted = false;
	email->fileSize = (UOSInt)fileSize;
	Sync::MutexUsage mutUsage(this->mailMut);
	this->mailList.Add(email);
	mutUsage.ReplaceMutex(this->fileMut);
	this->fileMap.Put(file->id, file);
	return true;
}

Optional<IO::StreamData> Net::Email::FileEmailStore::OpenEmailData(Int64 id)
{
	NN<FileInfo> fileInfo;
	if (!this->GetFileInfo(id).SetTo(fileInfo))
		return 0;
	return NEW_CLASS_D(IO::StmData::FileData(fileInfo->fileName, false));
}

UnsafeArrayOpt<const UTF8Char> Net::Email::FileEmailStore::GetEmailUid(Int64 id)
{
	NN<FileInfo> fileInfo;
	if (!this->GetFileInfo(id).SetTo(fileInfo))
		return 0;
	return fileInfo->uid;
}

UOSInt Net::Email::FileEmailStore::GetRcptList(Int64 id, NN<Data::ArrayListStringNN> rcptList)
{
	Sync::MutexUsage mutUsage(this->fileMut);
	NN<FileInfo> fileInfo;
	if (!this->fileMap.Get(id).SetTo(fileInfo))
		return 0;
	return rcptList->AddAll(fileInfo->rcptList);
}

Net::Email::MailController::RemoveStatus Net::Email::FileEmailStore::RemoveMessage(Text::CString userName, UOSInt msgIndex)
{
	NN<Net::Email::EmailStore::EmailInfo> email;
	Sync::MutexUsage mutUsage(this->mailMut);
	if (!this->mailList.GetItem(msgIndex).SetTo(email))
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

Optional<Net::Email::EmailStore::EmailInfo> Net::Email::FileEmailStore::GetEmailByIndex(Text::CString userName, UOSInt msgIndex)
{
	Sync::MutexUsage mutUsage(this->mailMut);
	return this->mailList.GetItem(msgIndex);
}

void Net::Email::FileEmailStore::GetMessageStat(Text::CString userName, NN<MessageStat> stat)
{
	UOSInt unreadCount;
	UInt64 unreadSize;
	UOSInt totalCnt;
	UOSInt i;
	NN<EmailInfo> email;
	unreadSize = 0;
	unreadCount = 0;

	Sync::MutexUsage mutUsage(this->mailMut);
	totalCnt = this->mailList.GetCount();
	i = this->recvIndex;
	while (i < totalCnt)
	{
		email = this->mailList.GetItemNoCheck(i);
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

UOSInt Net::Email::FileEmailStore::GetUnreadIndices(Text::CString userName, NN<Data::ArrayList<UOSInt>> indices)
{
	UOSInt totalCnt;
	UOSInt i;
	NN<EmailInfo> email;

	Sync::MutexUsage mutUsage(this->mailMut);
	totalCnt = this->mailList.GetCount();
	i = this->recvIndex;
	while (i < totalCnt)
	{
		email = this->mailList.GetItemNoCheck(i);
		if (!email->isDeleted)
		{
			indices->Add(i);
		}
		i++;
	}
	return totalCnt;
}
