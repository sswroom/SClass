#include "Stdafx.h"
#include "Crypto/Hash/CRC32R.h"
#include "Data/ByteTool.h"
#include "Net/Email/EmailMessage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringTool.h"
#include "Text/TextBinEnc/Base64Enc.h"

UOSInt Net::Email::EmailMessage::GetHeaderIndex(const Char *name)
{
	UOSInt len = Text::StrCharCnt(name);
	const UTF8Char *header;
	UOSInt i = 0;
	UOSInt j = this->headerList->GetCount();
	while (i < j)
	{
		header = this->headerList->GetItem(i);
		if (Text::StrStartsWith(header, (const UTF8Char*)"name") && header[len] == ':' && header[len + 1] == ' ')
		{
			return i;
		}
		i++;
	}
	return INVALID_INDEX;
}

Bool Net::Email::EmailMessage::SetHeader(const Char *name, const UTF8Char *val)
{
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)name);
	sb.AppendChar(':', 1);
	sb.AppendChar(' ', 1);
	sb.Append(val);
	UOSInt i = this->GetHeaderIndex(name);
	if (i == INVALID_INDEX)
	{
		this->headerList->Add(Text::StrCopyNew(sb.ToString()));
	}
	else
	{
		Text::StrDelNew(this->headerList->GetItem(i));
		this->headerList->SetItem(i, Text::StrCopyNew(sb.ToString()));
	}
	return true;
}

Bool Net::Email::EmailMessage::AppendUTF8Header(Text::StringBuilderUTF *sb, const UTF8Char *val)
{
	Text::TextBinEnc::Base64Enc b64;
	sb->Append((const UTF8Char*)"=?UTF-8?B?");
	b64.EncodeBin(sb, val, Text::StrCharCnt(val));
	sb->Append((const UTF8Char*)"?=");
	return true;
}

Net::Email::EmailMessage::EmailMessage()
{
	this->fromAddr = 0;
	NEW_CLASS(this->recpList, Data::ArrayList<const UTF8Char*>());
	NEW_CLASS(this->headerList, Data::ArrayList<const UTF8Char*>());
	this->content = 0;
	this->contentLen = 0;
}

Net::Email::EmailMessage::~EmailMessage()
{
	SDEL_TEXT(this->fromAddr);
	LIST_FREE_FUNC(this->recpList, Text::StrDelNew);
	DEL_CLASS(this->recpList);
	LIST_FREE_FUNC(this->headerList, Text::StrDelNew);
	DEL_CLASS(this->headerList);
	if (this->content)
	{
		MemFree(this->content);
	}
}

Bool Net::Email::EmailMessage::SetSubject(const UTF8Char *subject)
{
	if (Text::StringTool::IsNonASCII(subject))
	{
		Text::StringBuilderUTF8 sb;
		this->AppendUTF8Header(&sb, subject);
		this->SetHeader("Subject", sb.ToString());
	}
	else
	{
		this->SetHeader("Subject", subject);
	}
	return true;
}

Bool Net::Email::EmailMessage::SetContent(const UTF8Char *content, const Char *contentType)
{
	this->SetHeader("Content-Type", (const UTF8Char*)contentType);
	UOSInt contentLen = Text::StrCharCnt(content);
	if (this->content)
		MemFree(this->content);
	this->content = MemAlloc(UInt8, contentLen);
	MemCopyNO(this->content, content, contentLen);
	this->contentLen = contentLen;
	return true;
}

Bool Net::Email::EmailMessage::SetSentDate(Data::DateTime *dt)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	switch (dt->GetWeekday())
	{
	case Data::DateTime::W_SUNDAY:
		sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Sun, ");
		break;
	case Data::DateTime::W_MONDAY:
		sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Mon, ");
		break;
	case Data::DateTime::W_TUESDAY:
		sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Tue, ");
		break;
	case Data::DateTime::W_WEDNESDAY:
		sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Wed, ");
		break;
	case Data::DateTime::W_THURSDAY:
		sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Thu, ");
		break;
	case Data::DateTime::W_FRIDAY:
		sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Fri, ");
		break;
	case Data::DateTime::W_SATURDAY:
		sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Sat, ");
		break;
	default:
		sptr = sbuff;
		break;
	};
	dt->ToString(sptr, "dd MMM yyyy HH:mm:ss zzz");
	return this->SetHeader("Date", sbuff);
}

Bool Net::Email::EmailMessage::SetMessageId(const UTF8Char *msgId)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendChar('<', 1);
	sb.Append(msgId);
	sb.AppendChar('>', 1);
	return this->SetHeader("Message-ID", sb.ToString());
}

Bool Net::Email::EmailMessage::SetFrom(const UTF8Char *name, const UTF8Char *addr)
{
	Text::StringBuilderUTF8 sb;
	if (name)
	{
		if (Text::StringTool::IsNonASCII(name))
		{
			this->AppendUTF8Header(&sb, name);			
		}
		else
		{
			sb.AppendChar('"', 1);
			sb.Append(name);
			sb.AppendChar('"', 1);
		}
		sb.AppendChar(' ', 1);
	}
	sb.AppendChar('<', 1);
	sb.Append(addr);
	sb.AppendChar('>', 1);
	this->SetHeader("From", sb.ToString());
	SDEL_TEXT(this->fromAddr);
	this->fromAddr = Text::StrCopyNew(addr);
	return true;
}

Bool Net::Email::EmailMessage::AddTo(const UTF8Char *name, const UTF8Char *addr)
{
	UOSInt i = this->GetHeaderIndex("To");
	Text::StringBuilderUTF8 sb;
	if (i != INVALID_INDEX)
	{
		sb.Append(this->headerList->GetItem(i) + 4);
		sb.Append((const UTF8Char*)", ");
	}
	if (name)
	{
		if (Text::StringTool::IsNonASCII(name))
		{
			this->AppendUTF8Header(&sb, name);			
		}
		else
		{
			sb.AppendChar('"', 1);
			sb.Append(name);
			sb.AppendChar('"', 1);
		}
		sb.AppendChar(' ', 1);
	}
	sb.AppendChar('<', 1);
	sb.Append(addr);
	sb.AppendChar('>', 1);
	this->SetHeader("To", sb.ToString());
	this->recpList->Add(Text::StrCopyNew(addr));
	return true;
}

Bool Net::Email::EmailMessage::AddCc(const UTF8Char *name, const UTF8Char *addr)
{
	UOSInt i = this->GetHeaderIndex("Cc");
	Text::StringBuilderUTF8 sb;
	if (i != INVALID_INDEX)
	{
		sb.Append(this->headerList->GetItem(i) + 4);
		sb.Append((const UTF8Char*)", ");
	}
	if (name)
	{
		if (Text::StringTool::IsNonASCII(name))
		{
			this->AppendUTF8Header(&sb, name);			
		}
		else
		{
			sb.AppendChar('"', 1);
			sb.Append(name);
			sb.AppendChar('"', 1);
		}
		sb.AppendChar(' ', 1);
	}
	sb.AppendChar('<', 1);
	sb.Append(addr);
	sb.AppendChar('>', 1);
	this->SetHeader("Cc", sb.ToString());
	this->recpList->Add(Text::StrCopyNew(addr));
	return true;
}

Bool Net::Email::EmailMessage::AddBcc(const UTF8Char *addr)
{
	this->recpList->Add(Text::StrCopyNew(addr));
	return true;
}

Bool Net::Email::EmailMessage::CompletedMessage()
{
	if (this->fromAddr == 0 || this->recpList->GetCount() == 0 || this->contentLen == 0)
	{
		return false;
	}
	return true;
}

const UTF8Char *Net::Email::EmailMessage::GetFromAddr()
{
	return this->fromAddr;
}

Data::ArrayList<const UTF8Char*> *Net::Email::EmailMessage::GetRecpList()
{
	return this->recpList;
}

Bool Net::Email::EmailMessage::WriteToStream(IO::Stream *stm)
{
	if (!this->CompletedMessage())
	{
		return false;
	}
	const UTF8Char *header;
	UOSInt i = 0;
	UOSInt j = this->headerList->GetCount();
	while (i < j)
	{
		header = this->headerList->GetItem(i);
		stm->Write(header, Text::StrCharCnt(header));
		stm->Write((const UTF8Char*)"\r\n", 2);
		i++;
	}
	stm->Write((const UTF8Char*)"\r\n", 2);
	stm->Write(this->content, this->contentLen);
	return true;
}

Bool Net::Email::EmailMessage::GenerateMessageID(Text::StringBuilderUTF *sb, const UTF8Char *mailFrom)
{
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	sb->AppendHex64(dt.ToTicks());
	sb->AppendChar('.', 1);
	UInt8 crcVal[4];
	Crypto::Hash::CRC32R crc;
	UOSInt i;
	i = Text::StrIndexOf(mailFrom, '@');
	crc.Calc((UInt8*)mailFrom, i);
	crc.GetValue((UInt8*)&crcVal);
	sb->AppendHex32(ReadMUInt32(crcVal));
	sb->Append(&mailFrom[i]);
	return true;
}
