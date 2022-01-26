#include "Stdafx.h"
#include "Crypto/Hash/CRC32R.h"
#include "Data/ByteTool.h"
#include "Net/Email/EmailMessage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringTool.h"
#include "Text/TextBinEnc/Base64Enc.h"

UOSInt Net::Email::EmailMessage::GetHeaderIndex(const UTF8Char *name, UOSInt nameLen)
{
	Text::String *header;
	UOSInt i = 0;
	UOSInt j = this->headerList->GetCount();
	while (i < j)
	{
		header = this->headerList->GetItem(i);
		if (header->StartsWith(name, nameLen) && header->v[nameLen] == ':' && header->v[nameLen + 1] == ' ')
		{
			return i;
		}
		i++;
	}
	return INVALID_INDEX;
}

Bool Net::Email::EmailMessage::SetHeader(const UTF8Char *name, UOSInt nameLen, const UTF8Char *val)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(name, nameLen);
	sb.AppendChar(':', 1);
	sb.AppendChar(' ', 1);
	sb.AppendSlow(val);
	UOSInt i = this->GetHeaderIndex(name, nameLen);
	if (i == INVALID_INDEX)
	{
		this->headerList->Add(Text::String::New(sb.ToString(), sb.GetLength()));
	}
	else
	{
		this->headerList->GetItem(i)->Release();
		this->headerList->SetItem(i, Text::String::New(sb.ToString(), sb.GetLength()));
	}
	return true;
}

Bool Net::Email::EmailMessage::AppendUTF8Header(Text::StringBuilderUTF8 *sb, const UTF8Char *val)
{
	Text::TextBinEnc::Base64Enc b64;
	sb->AppendC(UTF8STRC("=?UTF-8?B?"));
	b64.EncodeBin(sb, val, Text::StrCharCnt(val));
	sb->AppendC(UTF8STRC("?="));
	return true;
}

Net::Email::EmailMessage::EmailMessage()
{
	this->fromAddr = 0;
	NEW_CLASS(this->recpList, Data::ArrayList<const UTF8Char*>());
	NEW_CLASS(this->headerList, Data::ArrayList<Text::String*>());
	this->content = 0;
	this->contentLen = 0;
}

Net::Email::EmailMessage::~EmailMessage()
{
	SDEL_TEXT(this->fromAddr);
	LIST_FREE_FUNC(this->recpList, Text::StrDelNew);
	DEL_CLASS(this->recpList);
	LIST_FREE_STRING(this->headerList);
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
		this->SetHeader(UTF8STRC("Subject"), sb.ToString());
	}
	else
	{
		this->SetHeader(UTF8STRC("Subject"), subject);
	}
	return true;
}

Bool Net::Email::EmailMessage::SetContent(const UTF8Char *content, const Char *contentType)
{
	this->SetHeader(UTF8STRC("Content-Type"), (const UTF8Char*)contentType);
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
	case Data::DateTime::Weekday::Sunday:
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Sun, "));
		break;
	case Data::DateTime::Weekday::Monday:
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Mon, "));
		break;
	case Data::DateTime::Weekday::Tuesday:
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Tue, "));
		break;
	case Data::DateTime::Weekday::Wednesday:
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Wed, "));
		break;
	case Data::DateTime::Weekday::Thursday:
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Thu, "));
		break;
	case Data::DateTime::Weekday::Friday:
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Fri, "));
		break;
	case Data::DateTime::Weekday::Saturday:
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Sat, "));
		break;
	default:
		sptr = sbuff;
		break;
	};
	dt->ToString(sptr, "dd MMM yyyy HH:mm:ss zzz");
	return this->SetHeader(UTF8STRC("Date"), sbuff);
}

Bool Net::Email::EmailMessage::SetMessageId(const UTF8Char *msgId)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendChar('<', 1);
	sb.AppendSlow(msgId);
	sb.AppendChar('>', 1);
	return this->SetHeader(UTF8STRC("Message-ID"), sb.ToString());
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
			sb.AppendSlow(name);
			sb.AppendChar('"', 1);
		}
		sb.AppendChar(' ', 1);
	}
	sb.AppendChar('<', 1);
	sb.AppendSlow(addr);
	sb.AppendChar('>', 1);
	this->SetHeader(UTF8STRC("From"), sb.ToString());
	SDEL_TEXT(this->fromAddr);
	this->fromAddr = Text::StrCopyNew(addr);
	return true;
}

Bool Net::Email::EmailMessage::AddTo(const UTF8Char *name, const UTF8Char *addr)
{
	UOSInt i = this->GetHeaderIndex(UTF8STRC("To"));
	Text::StringBuilderUTF8 sb;
	if (i != INVALID_INDEX)
	{
		sb.Append(this->headerList->GetItem(i) + 4);
		sb.AppendC(UTF8STRC(", "));
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
			sb.AppendSlow(name);
			sb.AppendChar('"', 1);
		}
		sb.AppendChar(' ', 1);
	}
	sb.AppendChar('<', 1);
	sb.AppendSlow(addr);
	sb.AppendChar('>', 1);
	this->SetHeader(UTF8STRC("To"), sb.ToString());
	this->recpList->Add(Text::StrCopyNew(addr));
	return true;
}

Bool Net::Email::EmailMessage::AddToList(const UTF8Char *addrs)
{
	Bool succ;
	UOSInt i;
	UTF8Char *sarr[2];
	Text::StringBuilderUTF8 sb;
	sb.AppendSlow(addrs);
	sarr[1] = sb.ToString();
	succ = true;
	while (true)
	{
		i = Text::StrSplitTrim(sarr, 2, sarr[1], ',');
		if (!Text::StringTool::IsEmailAddress(sarr[0]))
		{
			succ = false;
		}
		else
		{
			succ = succ && this->AddTo(0, sarr[0]);
		}
		if (i == 1)
			break;
	}
	return succ;
}

Bool Net::Email::EmailMessage::AddCc(const UTF8Char *name, const UTF8Char *addr)
{
	UOSInt i = this->GetHeaderIndex(UTF8STRC("Cc"));
	Text::StringBuilderUTF8 sb;
	if (i != INVALID_INDEX)
	{
		Text::String *s = this->headerList->GetItem(i);
		sb.AppendC(s->v + 4, s->leng - 4);
		sb.AppendC(UTF8STRC(", "));
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
			sb.AppendSlow(name);
			sb.AppendChar('"', 1);
		}
		sb.AppendChar(' ', 1);
	}
	sb.AppendChar('<', 1);
	sb.AppendSlow(addr);
	sb.AppendChar('>', 1);
	this->SetHeader(UTF8STRC("Cc"), sb.ToString());
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
	Text::String *header;
	UOSInt i = 0;
	UOSInt j = this->headerList->GetCount();
	while (i < j)
	{
		header = this->headerList->GetItem(i);
		stm->Write(header->v, header->leng);
		stm->Write((const UInt8*)"\r\n", 2);
		i++;
	}
	stm->Write((const UInt8*)"\r\n", 2);
	stm->Write(this->content, this->contentLen);
	return true;
}

Bool Net::Email::EmailMessage::GenerateMessageID(Text::StringBuilderUTF8 *sb, const UTF8Char *mailFrom)
{
	UOSInt mailFromLen = Text::StrCharCnt(mailFrom);
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	sb->AppendHex64((UInt64)dt.ToTicks());
	sb->AppendChar('.', 1);
	UInt8 crcVal[4];
	Crypto::Hash::CRC32R crc;
	UOSInt i;
	i = Text::StrIndexOfCharC(mailFrom, mailFromLen, '@');
	crc.Calc((UInt8*)mailFrom, i);
	crc.GetValue((UInt8*)&crcVal);
	sb->AppendHex32(ReadMUInt32(crcVal));
	sb->AppendC(&mailFrom[i], mailFromLen - i);
	return true;
}
