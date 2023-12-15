#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Encrypt/Base64.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/StringBuilder.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/MIMEObj/MailMessage.h"
#include "Text/MIMEObj/MultipartMIMEObj.h"

Text::MIMEObj::MailMessage::MailMessage() : Text::MIMEObj::MIMEMessage()
{
}

Text::MIMEObj::MailMessage::~MailMessage()
{
}

Text::CStringNN Text::MIMEObj::MailMessage::GetClassName() const
{
	return CSTR("MailMessage");
}

Text::IMIMEObj *Text::MIMEObj::MailMessage::Clone() const
{
	Text::MIMEObj::MailMessage *msg;
	UOSInt i;
	UOSInt j;
	NotNullPtr<Text::String> name;
	NotNullPtr<Text::String> value;
	NEW_CLASS(msg, Text::MIMEObj::MailMessage());
	i = 0;
	j = this->headerName.GetCount();
	while (i < j)
	{
		if (this->headerName.GetItem(i).SetTo(name) && this->headerValue.GetItem(i).SetTo(value))
		{
			msg->AddHeader(name, value);
		}
		i++;
	}
	if (this->content)
	{
		msg->SetContent(this->content->Clone());
	}
	return msg;
}

Bool Text::MIMEObj::MailMessage::GetDate(Data::DateTime *dt) const
{
	NotNullPtr<Text::String> hdr;
	if (!GetHeader(UTF8STRC("Date")).SetTo(hdr))
		return false;
	dt->SetValue(hdr->ToCString());
	return true;
}

UTF8Char *Text::MIMEObj::MailMessage::GetFromAddr(UTF8Char *sbuff) const
{
	NotNullPtr<Text::String> hdr;
	if (!GetHeader(UTF8STRC("From")).SetTo(hdr))
		return 0;
	return ParseHeaderStr(sbuff, hdr->v);
}

UTF8Char *Text::MIMEObj::MailMessage::GetSubject(UTF8Char *sbuff) const
{
	NotNullPtr<Text::String> hdr;
	if (!GetHeader(UTF8STRC("Subject")).SetTo(hdr))
		return 0;
	return ParseHeaderStr(sbuff, hdr->v);
}

UTF8Char *Text::MIMEObj::MailMessage::GetReplyTo(UTF8Char *sbuff) const
{
	NotNullPtr<Text::String> hdr;
	if (!GetHeader(UTF8STRC("Reply-To")).SetTo(hdr))
		return 0;
	return ParseHeaderStr(sbuff, hdr->v);
}

UOSInt Text::MIMEObj::MailMessage::GetRecpList(NotNullPtr<Data::ArrayList<MailAddress*>> recpList) const
{
	UOSInt i = 0;
	NotNullPtr<Text::String> hdr;
	if (GetHeader(UTF8STRC("To")).SetTo(hdr))
		i += ParseAddrList(hdr->v, hdr->leng, recpList, AT_TO);
	if (GetHeader(UTF8STRC("CC")).SetTo(hdr))
		i += ParseAddrList(hdr->v, hdr->leng, recpList, AT_CC);
	return i;
}

void Text::MIMEObj::MailMessage::FreeRecpList(NotNullPtr<Data::ArrayList<MailAddress*>> recpList) const
{
	MailAddress *addr;
	UOSInt i;
	i = recpList->GetCount();
	while (i-- > 0)
	{
		addr = recpList->RemoveAt(i);
		if (addr->name)
			addr->name->Release();
		addr->address->Release();
		MemFree(addr);
	}
}

Text::MIMEObj::TextMIMEObj *Text::MIMEObj::MailMessage::GetContentText() const
{
	if (this->content == 0)
		return 0;
	Text::CString clsName = this->content->GetClassName();
	if (clsName.Equals(UTF8STRC("TextMIMEObj")))
		return (Text::MIMEObj::TextMIMEObj*)this->content;
	if (clsName.Equals(UTF8STRC("MultipartMIMEObj")))
	{
		Text::CString contType = this->content->GetContentType();
		if (contType.StartsWith(UTF8STRC("multipart/mixed")))
		{
			Text::MIMEObj::MultipartMIMEObj *mpart = (Text::MIMEObj::MultipartMIMEObj*)this->content;
			Text::IMIMEObj *obj = mpart->GetPartContent(0);
			if (obj)
			{
				if (clsName.Equals(UTF8STRC("TextMIMEObj")))
				{
					return (Text::MIMEObj::TextMIMEObj*)obj;
				}
			}
		}
	}
	return 0;
}

Text::IMIMEObj *Text::MIMEObj::MailMessage::GetContentMajor() const
{
	if (this->content == 0)
		return 0;
	Text::IMIMEObj *obj = content;
	Text::CString contType = obj->GetContentType();
	Text::CString clsName = obj->GetClassName();
	if (clsName.Equals(UTF8STRC("MultipartMIMEObj")) && contType.StartsWith(UTF8STRC("multipart/mixed")))
	{
		obj = ((Text::MIMEObj::MultipartMIMEObj*)obj)->GetPartContent(0);
		clsName = obj->GetClassName();
	}
	if (clsName.Equals(UTF8STRC("TextMIMEObj")))
		return obj;
	if (clsName.Equals(UTF8STRC("HTMLMIMEObj")))
		return obj;
	if (clsName.Equals(UTF8STRC("MultipartMIMEObj")))
	{
		contType = obj->GetContentType();
		if (contType.StartsWith(UTF8STRC("multipart/related")))
			return obj;
		if (contType.StartsWith(UTF8STRC("multipart/alternative")))
		{
			Text::MIMEObj::MultipartMIMEObj *mpart = (Text::MIMEObj::MultipartMIMEObj*)obj;
			return mpart->GetPartContent(mpart->GetPartCount() - 1);
		}
	}
	return 0;
}

Text::IMIMEObj *Text::MIMEObj::MailMessage::GetAttachment(OSInt index, NotNullPtr<Text::StringBuilderUTF8> name) const
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	NotNullPtr<Text::String> s;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Text::MIMEObj::MIMEMessage *part;
	if (this->content == 0)
		return 0;
	if (this->content->GetClassName().Equals(UTF8STRC("MultipartMIMEObj")))
	{
		Text::CString contType = this->content->GetContentType();
		if (Text::StrStartsWithC(contType.v, contType.leng, UTF8STRC("multipart/mixed")))
		{
			Text::MIMEObj::MultipartMIMEObj *mpart = (Text::MIMEObj::MultipartMIMEObj*)this->content;
			i = 0;
			j = mpart->GetPartCount();
			while (i < j)
			{
				part = mpart->GetPart(i);
				if (part->GetHeader(UTF8STRC("Content-Disposition")).SetTo(s) && s->StartsWith(UTF8STRC("attachment")))
				{
					if (index == 0)
					{
						sptr = ParseHeaderStr(sbuff, s->v);
						k = Text::StrIndexOfC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("filename="));
						if (k != INVALID_INDEX)
						{
							if (sbuff[k + 9] == '\"')
							{
								l = Text::StrIndexOfChar(&sbuff[k + 10], '\"');
								if (l != INVALID_INDEX)
								{
									name->AppendC(&sbuff[k + 10], l);
								}
								else
								{
									name->AppendC(&sbuff[k + 10], (UOSInt)(sptr - &sbuff[k + 10]));
								}
							}
							else
							{
								l = Text::StrIndexOfChar(&sbuff[k + 9], ' ');
								if (l != INVALID_INDEX)
								{
									name->AppendC(&sbuff[k + 9], l);
								}
								else
								{
									name->AppendC(&sbuff[k + 9], (UOSInt)(sptr - &sbuff[k + 9]));
								}
							}
						}
						return part->GetContent();
					}
					else
					{
						index--;
					}
				}
				i++;
			}
		}
	}
	return 0;
}

Text::IMIMEObj *Text::MIMEObj::MailMessage::GetRAWContent() const
{
	return this->content;
}

Text::MIMEObj::MailMessage *Text::MIMEObj::MailMessage::ParseFile(NotNullPtr<IO::StreamData> fd)
{
	Text::MIMEObj::MailMessage *mail;
	NEW_CLASS(mail, Text::MIMEObj::MailMessage());
	if (!mail->ParseFromData(fd))
	{
		DEL_CLASS(mail);
		return 0;
	}
	return mail;
}

UOSInt Text::MIMEObj::MailMessage::ParseAddrList(const UTF8Char *hdr, UOSInt hdrLen, NotNullPtr<Data::ArrayList<MailAddress*>> recpList, AddressType type) const
{
	UTF8Char *sbuff;
	UTF8Char *sptr;
	UTF8Char *ptr1;
	UTF8Char *ptr1End;
	UTF8Char *ptr2;
	UOSInt ret = 0;
	UTF8Char c;
	Bool isEnd;
	Bool quoted;
	MailAddress *addr;
	sbuff = MemAlloc(UTF8Char, hdrLen + 1);
	this->ParseHeaderStr(sbuff, hdr);

	sptr = sbuff;
	isEnd = false;
	while (true)
	{
		ptr1 = sptr;
		quoted = false;
		while (true)
		{
			c = *sptr;
			if (c == 0)
			{
				isEnd = true;
				break;
			}
			else if (c == '"')
			{
				quoted = !quoted;
			}
			else if (c == ',' && !quoted)
			{
				break;
			}
			sptr++;
		}
		*sptr = 0;

		addr = MemAlloc(MailAddress, 1);
		addr->type = type;
		quoted = false;
		ptr2 = ptr1;
		while (true)
		{
			c = *ptr2;
			if (c == '"')
			{
				quoted = !quoted;
				ptr2++;
			}
			else if (c == '<' && !quoted)
			{
				*ptr2++ = 0;
				ptr1End = Text::StrTrim(ptr1);
				addr->name = Text::String::New(ptr1, (UOSInt)(ptr1End - ptr1)).Ptr();
				ptr1 = ptr2;
				while (true)
				{
					c = *ptr2++;
					if (c == '0' || c == '>')
					{
						ptr2[-1] = 0;
						addr->address = Text::String::New(ptr1, (UOSInt)(ptr2 - ptr1 - 1));
						break;
					}
				}
				break;
			}
			else if (c == 0)
			{
				addr->address = Text::String::New(ptr1, (UOSInt)(ptr2 - ptr1));
				addr->name = 0;
				break;
			}
			else
			{
				ptr2++;
			}
		}
		recpList->Add(addr);
		ret++;
		
		if (isEnd)
			break;
		sptr++;
	}

	MemFree(sbuff);
	return ret;
}
