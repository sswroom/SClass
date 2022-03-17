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

Bool Net::Email::EmailMessage::SetHeader(const UTF8Char *name, UOSInt nameLen, const UTF8Char *val, UOSInt valLen)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(name, nameLen);
	sb.AppendUTF8Char(':');
	sb.AppendUTF8Char(' ');
	sb.AppendC(val, valLen);
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

Bool Net::Email::EmailMessage::AppendUTF8Header(Text::StringBuilderUTF8 *sb, const UTF8Char *val, UOSInt valLen)
{
	Text::TextBinEnc::Base64Enc b64;
	sb->AppendC(UTF8STRC("=?UTF-8?B?"));
	b64.EncodeBin(sb, val, valLen);
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
	SDEL_STRING(this->fromAddr);
	LIST_FREE_FUNC(this->recpList, Text::StrDelNew);
	DEL_CLASS(this->recpList);
	LIST_FREE_STRING(this->headerList);
	DEL_CLASS(this->headerList);
	if (this->content)
	{
		MemFree(this->content);
	}
}

Bool Net::Email::EmailMessage::SetSubject(Text::CString subject)
{
	if (Text::StringTool::IsNonASCII(subject.v))
	{
		Text::StringBuilderUTF8 sb;
		this->AppendUTF8Header(&sb, subject.v, subject.leng);
		this->SetHeader(UTF8STRC("Subject"), sb.ToString(), sb.GetLength());
	}
	else
	{
		this->SetHeader(UTF8STRC("Subject"), subject.v, subject.leng);
	}
	return true;
}

Bool Net::Email::EmailMessage::SetContent(Text::CString content, Text::CString contentType)
{
	this->SetHeader(UTF8STRC("Content-Type"), contentType.v, contentType.leng);
	if (this->content)
		MemFree(this->content);
	this->content = MemAlloc(UInt8, content.leng);
	MemCopyNO(this->content, content.v, content.leng);
	this->contentLen = content.leng;
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
	sptr = dt->ToString(sptr, "dd MMM yyyy HH:mm:ss zzz");
	return this->SetHeader(UTF8STRC("Date"), sbuff, (UOSInt)(sptr - sbuff));
}

Bool Net::Email::EmailMessage::SetMessageId(Text::CString msgId)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendUTF8Char('<');
	sb.Append(msgId);
	sb.AppendUTF8Char('>');
	return this->SetHeader(UTF8STRC("Message-ID"), sb.ToString(), sb.GetLength());
}

Bool Net::Email::EmailMessage::SetFrom(Text::CString name, Text::CString addr)
{
	Text::StringBuilderUTF8 sb;
	if (name.leng > 0)
	{
		if (Text::StringTool::IsNonASCII(name.v))
		{
			this->AppendUTF8Header(&sb, name.v, name.leng);			
		}
		else
		{
			sb.AppendUTF8Char('"');
			sb.Append(name);
			sb.AppendUTF8Char('"');
		}
		sb.AppendUTF8Char(' ');
	}
	sb.AppendUTF8Char('<');
	sb.Append(addr);
	sb.AppendUTF8Char('>');
	this->SetHeader(UTF8STRC("From"), sb.ToString(), sb.GetLength());
	SDEL_STRING(this->fromAddr);
	this->fromAddr = Text::String::New(addr);
	return true;
}

Bool Net::Email::EmailMessage::AddTo(Text::CString name, Text::CString addr)
{
	UOSInt i = this->GetHeaderIndex(UTF8STRC("To"));
	Text::StringBuilderUTF8 sb;
	if (i != INVALID_INDEX)
	{
		sb.Append(this->headerList->GetItem(i) + 4);
		sb.AppendC(UTF8STRC(", "));
	}
	if (name.leng > 0)
	{
		if (Text::StringTool::IsNonASCII(name.v))
		{
			this->AppendUTF8Header(&sb, name.v, name.leng);			
		}
		else
		{
			sb.AppendUTF8Char('"');
			sb.Append(name);
			sb.AppendUTF8Char('"');
		}
		sb.AppendUTF8Char(' ');
	}
	sb.AppendUTF8Char('<');
	sb.Append(addr);
	sb.AppendUTF8Char('>');
	this->SetHeader(UTF8STRC("To"), sb.ToString(), sb.GetLength());
	this->recpList->Add(Text::StrCopyNewC(addr.v, addr.leng));
	return true;
}

Bool Net::Email::EmailMessage::AddToList(Text::CString addrs)
{
	Bool succ;
	UOSInt i;
	Text::PString sarr[2];
	Text::StringBuilderUTF8 sb;
	sb.Append(addrs);
	sarr[1] = sb;
	succ = true;
	while (true)
	{
		i = Text::StrSplitTrimP(sarr, 2, sarr[1], ',');
		if (!Text::StringTool::IsEmailAddress(sarr[0].v))
		{
			succ = false;
		}
		else
		{
			succ = succ && this->AddTo(CSTR_NULL, sarr[0].ToCString());
		}
		if (i == 1)
			break;
	}
	return succ;
}

Bool Net::Email::EmailMessage::AddCc(Text::CString name, Text::CString addr)
{
	UOSInt i = this->GetHeaderIndex(UTF8STRC("Cc"));
	Text::StringBuilderUTF8 sb;
	if (i != INVALID_INDEX)
	{
		Text::String *s = this->headerList->GetItem(i);
		sb.AppendC(s->v + 4, s->leng - 4);
		sb.AppendC(UTF8STRC(", "));
	}
	if (name.leng > 0)
	{
		if (Text::StringTool::IsNonASCII(name.v))
		{
			this->AppendUTF8Header(&sb, name.v, name.leng);
		}
		else
		{
			sb.AppendUTF8Char('"');
			sb.Append(name);
			sb.AppendUTF8Char('"');
		}
		sb.AppendUTF8Char(' ');
	}
	sb.AppendUTF8Char('<');
	sb.Append(addr);
	sb.AppendUTF8Char('>');
	this->SetHeader(UTF8STRC("Cc"), sb.ToString(), sb.GetLength());
	this->recpList->Add(Text::StrCopyNewC(addr.v, addr.leng));
	return true;
}

Bool Net::Email::EmailMessage::AddBcc(Text::CString addr)
{
	this->recpList->Add(Text::StrCopyNewC(addr.v, addr.leng));
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

Text::String *Net::Email::EmailMessage::GetFromAddr()
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

Bool Net::Email::EmailMessage::GenerateMessageID(Text::StringBuilderUTF8 *sb, Text::CString mailFrom)
{
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	sb->AppendHex64((UInt64)dt.ToTicks());
	sb->AppendUTF8Char('.');
	UInt8 crcVal[4];
	Crypto::Hash::CRC32R crc;
	UOSInt i;
	i = Text::StrIndexOfCharC(mailFrom.v, mailFrom.leng, '@');
	crc.Calc((UInt8*)mailFrom.v, i);
	crc.GetValue((UInt8*)&crcVal);
	sb->AppendHex32(ReadMUInt32(crcVal));
	sb->AppendC(&mailFrom.v[i], mailFrom.leng - i);
	return true;
}
