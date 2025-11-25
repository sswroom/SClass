#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Encrypt/Base64.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/StringBuilder.hpp"
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

NN<Text::MIMEObject> Text::MIMEObj::MailMessage::Clone() const
{
	NN<Text::MIMEObj::MailMessage> msg;
	UOSInt i;
	UOSInt j;
	NN<Text::String> name;
	NN<Text::String> value;
	NEW_CLASSNN(msg, Text::MIMEObj::MailMessage());
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
	NN<MIMEObject> content;
	if (this->content.SetTo(content))
	{
		msg->SetContent(content->Clone());
	}
	return msg;
}

Bool Text::MIMEObj::MailMessage::GetDate(Data::DateTime *dt) const
{
	NN<Text::String> hdr;
	if (!GetHeader(UTF8STRC("Date")).SetTo(hdr))
		return false;
	dt->SetValue(hdr->ToCString());
	return true;
}

UnsafeArrayOpt<UTF8Char> Text::MIMEObj::MailMessage::GetFromAddr(UnsafeArray<UTF8Char> sbuff) const
{
	NN<Text::String> hdr;
	if (!GetHeader(UTF8STRC("From")).SetTo(hdr))
		return 0;
	return ParseHeaderStr(sbuff, hdr->v);
}

UnsafeArrayOpt<UTF8Char> Text::MIMEObj::MailMessage::GetSubject(UnsafeArray<UTF8Char> sbuff) const
{
	NN<Text::String> hdr;
	if (!GetHeader(UTF8STRC("Subject")).SetTo(hdr))
		return 0;
	return ParseHeaderStr(sbuff, hdr->v);
}

UnsafeArrayOpt<UTF8Char> Text::MIMEObj::MailMessage::GetReplyTo(UnsafeArray<UTF8Char> sbuff) const
{
	NN<Text::String> hdr;
	if (!GetHeader(UTF8STRC("Reply-To")).SetTo(hdr))
		return 0;
	return ParseHeaderStr(sbuff, hdr->v);
}

UOSInt Text::MIMEObj::MailMessage::GetRecpList(NN<Data::ArrayListNN<MailAddress>> recpList) const
{
	UOSInt i = 0;
	NN<Text::String> hdr;
	if (GetHeader(UTF8STRC("To")).SetTo(hdr))
		i += ParseAddrList(hdr->v, hdr->leng, recpList, AT_TO);
	if (GetHeader(UTF8STRC("CC")).SetTo(hdr))
		i += ParseAddrList(hdr->v, hdr->leng, recpList, AT_CC);
	return i;
}

void Text::MIMEObj::MailMessage::FreeRecpList(NN<Data::ArrayListNN<MailAddress>> recpList) const
{
	NN<MailAddress> addr;
	UOSInt i;
	i = recpList->GetCount();
	while (i-- > 0)
	{
		addr = recpList->GetItemNoCheck(i);
		if (addr->name)
			addr->name->Release();
		addr->address->Release();
		MemFreeNN(addr);
	}
	recpList->Clear();
}

Optional<Text::MIMEObj::TextMIMEObj> Text::MIMEObj::MailMessage::GetContentText() const
{
	NN<MIMEObject> content;
	if (!this->content.SetTo(content))
		return 0;
	Text::CStringNN clsName = content->GetClassName();
	if (clsName.Equals(UTF8STRC("TextMIMEObj")))
		return NN<Text::MIMEObj::TextMIMEObj>::ConvertFrom(content);
	if (clsName.Equals(UTF8STRC("MultipartMIMEObj")))
	{
		Text::CStringNN contType = content->GetContentType();
		if (contType.StartsWith(UTF8STRC("multipart/mixed")))
		{
			NN<Text::MIMEObj::MultipartMIMEObj> mpart = NN<Text::MIMEObj::MultipartMIMEObj>::ConvertFrom(content);
			NN<Text::MIMEObject> obj;
			if (mpart->GetPartContent(0).SetTo(obj))
			{
				if (clsName.Equals(UTF8STRC("TextMIMEObj")))
				{
					return NN<Text::MIMEObj::TextMIMEObj>::ConvertFrom(obj);
				}
			}
		}
	}
	return 0;
}

Optional<Text::MIMEObject> Text::MIMEObj::MailMessage::GetContentMajor() const
{
	NN<MIMEObject> content;
	if (!this->content.SetTo(content))
		return 0;
	Optional<Text::MIMEObject> obj = content;
	Text::CStringNN contType = content->GetContentType();
	Text::CStringNN clsName = content->GetClassName();
	if (clsName.Equals(UTF8STRC("MultipartMIMEObj")) && contType.StartsWith(UTF8STRC("multipart/mixed")))
	{
		obj = NN<Text::MIMEObj::MultipartMIMEObj>::ConvertFrom(content)->GetPartContent(0);
		if (obj.SetTo(content))
		{
			clsName = content->GetClassName();
		}
	}
	if (clsName.Equals(UTF8STRC("TextMIMEObj")))
		return obj;
	if (clsName.Equals(UTF8STRC("HTMLMIMEObj")))
		return obj;
	if (clsName.Equals(UTF8STRC("MultipartMIMEObj")))
	{
		contType = content->GetContentType();
		if (contType.StartsWith(UTF8STRC("multipart/related")))
			return obj;
		if (contType.StartsWith(UTF8STRC("multipart/alternative")))
		{
			NN<Text::MIMEObj::MultipartMIMEObj> mpart = NN<Text::MIMEObj::MultipartMIMEObj>::ConvertFrom(content);
			return mpart->GetPartContent(mpart->GetPartCount() - 1);
		}
	}
	return 0;
}

Optional<Text::MIMEObject> Text::MIMEObj::MailMessage::GetAttachment(OSInt index, NN<Text::StringBuilderUTF8> name) const
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	NN<Text::String> s;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::MIMEObj::MIMEMessage> part;
	NN<MIMEObject> content;
	if (!this->content.SetTo(content))
		return 0;
	if (content->GetClassName().Equals(UTF8STRC("MultipartMIMEObj")))
	{
		Text::CStringNN contType = content->GetContentType();
		if (Text::StrStartsWithC(contType.v, contType.leng, UTF8STRC("multipart/mixed")))
		{
			NN<Text::MIMEObj::MultipartMIMEObj> mpart = NN<Text::MIMEObj::MultipartMIMEObj>::ConvertFrom(content);
			i = 0;
			j = mpart->GetPartCount();
			while (i < j)
			{
				if (mpart->GetPart(i).SetTo(part))
				{
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
				}
				i++;
			}
		}
	}
	return 0;
}

Optional<Text::MIMEObject> Text::MIMEObj::MailMessage::GetRAWContent() const
{
	return this->content;
}

Optional<Text::MIMEObj::MailMessage> Text::MIMEObj::MailMessage::ParseFile(NN<IO::StreamData> fd)
{
	NN<Text::MIMEObj::MailMessage> mail;
	NEW_CLASSNN(mail, Text::MIMEObj::MailMessage());
	if (!mail->ParseFromData(fd))
	{
		mail.Delete();
		return 0;
	}
	return mail;
}

UOSInt Text::MIMEObj::MailMessage::ParseAddrList(UnsafeArray<const UTF8Char> hdr, UOSInt hdrLen, NN<Data::ArrayListNN<MailAddress>> recpList, AddressType type) const
{
	UnsafeArray<UTF8Char> sbuff;
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> ptr1;
	UnsafeArray<UTF8Char> ptr1End;
	UnsafeArray<UTF8Char> ptr2;
	UOSInt ret = 0;
	UTF8Char c;
	Bool isEnd;
	Bool quoted;
	NN<MailAddress> addr;
	sbuff = MemAllocArr(UTF8Char, hdrLen + 1);
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

		addr = MemAllocNN(MailAddress);
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

	MemFreeArr(sbuff);
	return ret;
}
