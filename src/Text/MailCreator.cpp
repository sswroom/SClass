#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Encrypt/Base64.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Net/MIME.h"
#include "Text/Encoding.h"
#include "Text/MailCreator.h"
#include "Text/MyStringW.h"
#include "Text/MIMEObj/HTMLMIMEObj.h"
#include "Text/MIMEObj/MultipartMIMEObj.h"
#include "Text/MIMEObj/TextMIMEObj.h"
#include "Text/MIMEObj/UnknownMIMEObj.h"

void Text::MailCreator::AppendStr(NotNullPtr<Text::StringBuilderUTF8> sbc, Text::CString s)
{
	const UTF8Char *sptr;
	UTF8Char c;
	Bool found = false;
	sptr = s.v;
	while ((c = *sptr++) != 0)
	{
		if (c >= 0x80)
		{
			found = true;
			break;
		}
	}
	if (found)
	{
		UOSInt strLen;
		UInt8 *b64Buff;
		UOSInt b64Size;
		Crypto::Encrypt::Base64 b64;
		strLen = s.leng;
		b64Size = (strLen / 3) * 4 + 4;
		b64Buff = MemAlloc(UInt8, b64Size + 1);
		b64Size = b64.Encrypt(s.v, strLen, b64Buff, 0);
		b64Buff[b64Size] = 0;
		sbc->AppendC(b64Buff, b64Size);
		MemFree(b64Buff);
	}
	else
	{
		sbc->Append(s);
	}
}

void Text::MailCreator::AppendStr(NotNullPtr<Text::StringBuilderUTF8> sbc, const WChar *s)
{
	const WChar *wptr;
	WChar c;
	Bool found = false;
	wptr = s;
	while ((c = *wptr++) != 0)
	{
		if (c >= 0x80)
		{
			found = true;
			break;
		}
	}
	if (found)
	{
		NotNullPtr<Text::String> str = Text::String::NewNotNull(s);
		UOSInt buffSize;
		UInt8 *b64Buff;
		UOSInt b64Size;
		Crypto::Encrypt::Base64 b64;
		buffSize = str->leng;
		b64Size = (buffSize / 3) * 4 + 4;
		b64Buff = MemAlloc(UInt8, b64Size + 1);
		b64Size = b64.Encrypt(str->v, buffSize, b64Buff, 0);
		b64Buff[b64Size] = 0;
		sbc->AppendC(b64Buff, b64Size);
		MemFree(b64Buff);
		str->Release();
	}
	else
	{
		NotNullPtr<Text::String> str = Text::String::NewNotNull(s);
		sbc->Append(str);
		str->Release();
	}
}

Text::IMIMEObj *Text::MailCreator::ParseContentHTML(const UInt8 *buff, UOSInt buffSize, UInt32 codePage, Text::CString htmlPath)
{
	UOSInt j;
	UOSInt endOfst = buffSize - 6;
	UOSInt i;
	UOSInt k;
	UOSInt l;
	Data::ArrayList<Text::IMIMEObj*> imgs;
	Text::IMIMEObj *obj;
	Text::StringBuilderUTF8 sbc;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char c;
	Bool found;
	i = 0;
	j = 0;
	while (i < endOfst)
	{
		found = false;
		if (Text::StrStartsWithICaseC(&buff[i], buffSize - i, UTF8STRC(" src=\"")))
		{
			i += 6;
			found = true;
		}
		else if (Text::StrStartsWithICaseC(&buff[i], buffSize - i, UTF8STRC(" background=\"")))
		{
			i += 13;
			found = true;
		}
		if (found)
		{
			k = i;
			while (k < buffSize)
			{
				if (buff[k] == '\"')
				{
					UOSInt tmpI;
					found = false;
					sptr = htmlPath.ConcatTo(sbuff);
					l = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR );
					sptr = &sbuff[l + 1];
					tmpI = i;
					if (buff[i] == '/' || buff[i] == '\\')
					{
						tmpI++;
					}
					while (tmpI < k)
					{
						c = buff[tmpI];
						if (c == ':')
						{
							found = true;
							break;
						}
						else if (c == '/')
						{
							*sptr++ = IO::Path::PATH_SEPERATOR;
						}
						else
						{
							*sptr++ = c;
						}
						tmpI++;
					}
					if (!found)
					{
						*sptr = 0;
						obj = Text::IMIMEObj::ParseFromFile(CSTRP(sbuff, sptr));
						if (obj)
						{
							imgs.Add(obj);
							sbc.AppendC((const UTF8Char*)&buff[j], i - j);
							sbc.AppendC(UTF8STRC("cid:image"));
							sbc.AppendUOSInt(imgs.GetCount());
							j = k;
						}
					}
				}
				k++;
			}
		}
		i++;
	}
	if (j < buffSize)
	{
		sbc.AppendC((UTF8Char*)&buff[j], buffSize - j);
		j = buffSize;
	}
	
	if (imgs.GetCount() == 0)
	{
		NEW_CLASS(obj, Text::MIMEObj::HTMLMIMEObj(sbc.ToString(), sbc.GetLength(), 65001));
		return obj;
	}
	else
	{
		Text::MIMEObj::MultipartMIMEObj *mpart;
		NEW_CLASS(mpart, Text::MIMEObj::MultipartMIMEObj(CSTR("multipart/related"), CSTR_NULL));
		NEW_CLASS(obj, Text::MIMEObj::HTMLMIMEObj(sbc.ToString(), sbc.GetLength(), 65001));
		i = mpart->AddPart(obj);
		Text::CString contType = obj->GetContentType();
		mpart->AddPartHeader(i, UTF8STRC("Content-Type"), contType.v, contType.leng);
		mpart->AddPartHeader(i, UTF8STRC("Content-Transfer-Encoding"), UTF8STRC("8bit"));

		i = 0;
		j = imgs.GetCount();
		while (i < j)
		{
			obj = imgs.GetItem(i);
			k = mpart->AddPart(obj);
			contType = obj->GetContentType();
			mpart->AddPartHeader(k, UTF8STRC("Content-Type"), contType.v, contType.leng);
			mpart->AddPartHeader(k, UTF8STRC("Content-Transfer-Encoding"), UTF8STRC("base64"));
			sbc.ClearStr();
			sbc.AppendC(UTF8STRC("<image"));
			sbc.AppendUOSInt(i + 1);
			sbc.AppendC(UTF8STRC(">"));
			mpart->AddPartHeader(k, UTF8STRC("Content-ID"), sbc.ToString(), sbc.GetLength());

			i++;
		}

		return mpart;
	}
}

Text::MailCreator::MailCreator()
{
	this->from = 0;
	this->replyTo = 0;
	this->subject = 0;
	this->content = 0;
}

Text::MailCreator::~MailCreator()
{
	UOSInt i;
	Text::IMIMEObj *obj;

	SDEL_STRING(this->from);
	SDEL_STRING(this->replyTo);
	SDEL_STRING(this->subject);
	SDEL_CLASS(this->content);
	i = this->attachName.GetCount();
	while (i-- > 0)
	{
		this->attachName.GetItem(i)->Release();
		obj = this->attachObj.GetItem(i);
		DEL_CLASS(obj);
	}
}

void Text::MailCreator::SetFrom(const WChar *name, const WChar *address)
{
	if (address == 0)
	{
		SDEL_STRING(this->from);
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		if (name)
		{
			this->AppendStr(sb, name);
			sb.AppendC(UTF8STRC(" <"));
			this->AppendStr(sb, address);
			sb.AppendC(UTF8STRC(">"));
		}
		else
		{
			this->AppendStr(sb, address);
		}
		SDEL_STRING(this->from);
		this->from = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
	}
}

void Text::MailCreator::SetFrom(Text::CString name, Text::CString address)
{
	if (address.leng == 0)
	{
		SDEL_STRING(this->from);
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		if (name.leng != 0)
		{
			this->AppendStr(sb, name);
			sb.AppendC(UTF8STRC(" <"));
			this->AppendStr(sb, address);
			sb.AppendC(UTF8STRC(">"));
		}
		else
		{
			this->AppendStr(sb, address);
		}
		SDEL_STRING(this->from);
		this->from = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
	}
}

void Text::MailCreator::SetReplyTo(const WChar *name, const WChar *address)
{
	if (address == 0)
	{
		SDEL_STRING(this->replyTo);
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		if (name)
		{
			this->AppendStr(sb, name);
			sb.AppendC(UTF8STRC(" <"));
			this->AppendStr(sb, address);
			sb.AppendC(UTF8STRC(">"));
		}
		else
		{
			this->AppendStr(sb, address);
		}
		SDEL_STRING(this->replyTo);
		this->replyTo = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
	}
}

void Text::MailCreator::ToAdd(const WChar *name, const WChar *address)
{
	if (this->toVals.GetLength() > 0)
	{
		this->toVals.AppendC(UTF8STRC(", "));
	}
	if (name)
	{
		this->AppendStr(this->toVals, name);
		this->toVals.AppendC(UTF8STRC(" <"));
		this->AppendStr(this->toVals, address);
		this->toVals.AppendC(UTF8STRC(">"));
	}
	else
	{
		this->AppendStr(this->toVals, address);
	}
}

void Text::MailCreator::ToAdd(Text::String *name, NotNullPtr<Text::String> address)
{
	if (this->toVals.GetLength() > 0)
	{
		this->toVals.AppendC(UTF8STRC(", "));
	}
	if (name)
	{
		this->AppendStr(this->toVals, name->ToCString());
		this->toVals.AppendC(UTF8STRC(" <"));
		this->AppendStr(this->toVals, address->ToCString());
		this->toVals.AppendC(UTF8STRC(">"));
	}
	else
	{
		this->AppendStr(this->toVals, address->ToCString());
	}
}

void Text::MailCreator::ToClear()
{
	this->toVals.ClearStr();
}

void Text::MailCreator::CCAdd(const WChar *name, const WChar *address)
{
	if (this->ccVals.GetLength() > 0)
	{
		this->ccVals.AppendC(UTF8STRC(", "));
	}
	if (name)
	{
		this->AppendStr(this->ccVals, name);
		this->ccVals.AppendC(UTF8STRC(" <"));
		this->AppendStr(this->ccVals, address);
		this->ccVals.AppendC(UTF8STRC(">"));
	}
	else
	{
		this->AppendStr(this->ccVals, address);
	}
}

void Text::MailCreator::CCAdd(Text::String *name, NotNullPtr<Text::String> address)
{
	if (this->ccVals.GetLength() > 0)
	{
		this->ccVals.AppendC(UTF8STRC(", "));
	}
	if (name)
	{
		this->AppendStr(this->ccVals, name->ToCString());
		this->ccVals.AppendC(UTF8STRC(" <"));
		this->AppendStr(this->ccVals, address->ToCString());
		this->ccVals.AppendC(UTF8STRC(">"));
	}
	else
	{
		this->AppendStr(this->ccVals, address->ToCString());
	}
}

void Text::MailCreator::CCClear()
{
	this->toVals.ClearStr();
}

void Text::MailCreator::SetSubject(const WChar *subj)
{
	Text::StringBuilderUTF8 sb;
	this->AppendStr(sb, subj);
	SDEL_STRING(this->subject);
	this->subject = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
}

void Text::MailCreator::SetSubject(NotNullPtr<Text::String> subj)
{
	Text::StringBuilderUTF8 sb;
	this->AppendStr(sb, subj->ToCString());
	SDEL_STRING(this->subject);
	this->subject = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
}

void Text::MailCreator::SetContentHTML(const WChar *content, Text::CString htmlPath)
{
	Text::IMIMEObj *obj;
	UOSInt strLen = Text::StrCharCnt(content);
	UOSInt buffSize = Text::StrWChar_UTF8CntC(content, strLen);
	UInt8 *buff = MemAlloc(UInt8, buffSize + 1);
	Text::StrWChar_UTF8C(buff, content, strLen);
	obj = ParseContentHTML(buff, buffSize, 65001, htmlPath);
	MemFree(buff);
	if (obj)
	{
		SDEL_CLASS(this->content);
		this->content = obj;
	}
}

void Text::MailCreator::SetContentHTML(Text::String *content, Text::CString htmlPath)
{
	Text::IMIMEObj *obj = ParseContentHTML(content->v, content->leng, 65001, htmlPath);
	if (obj)
	{
		SDEL_CLASS(this->content);
		this->content = obj;
	}
}

void Text::MailCreator::SetContentText(const WChar *content, UInt32 codePage)
{
	Text::MIMEObj::TextMIMEObj *obj;
	NEW_CLASS(obj, Text::MIMEObj::TextMIMEObj(content, codePage));
	SDEL_CLASS(this->content);
	this->content = obj;
}

void Text::MailCreator::SetContentText(Text::String *content)
{
	Text::MIMEObj::TextMIMEObj *obj;
	NEW_CLASS(obj, Text::MIMEObj::TextMIMEObj(content->v, content->leng, 65001));
	SDEL_CLASS(this->content);
	this->content = obj;
}

Bool Text::MailCreator::SetContentFile(Text::CStringNN filePath)
{
	Text::IMIMEObj *obj;
	UOSInt buffSize;
	UInt8 *buff;
	{
		IO::FileStream fs(filePath, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (fs.IsError())
		{
			return false;
		}
		buffSize = (UOSInt)fs.GetLength();
		buff = MemAlloc(UInt8, buffSize);
		fs.Read(Data::ByteArray(buff, buffSize));
	}
	obj = ParseContentHTML(buff, buffSize, 65001, filePath);
	MemFree(buff);
	if (obj)
	{
		SDEL_CLASS(this->content);
		this->content = obj;
		return true;
	}

	return false;
}

void Text::MailCreator::AddAttachment(Text::CStringNN fileName)
{
	Text::IMIMEObj *obj = Text::IMIMEObj::ParseFromFile(fileName);

	this->attachObj.Add(obj);
	this->attachName.Add(Text::String::New(fileName.v, fileName.leng));
}

Text::MIMEObj::MailMessage *Text::MailCreator::CreateMail()
{
	Text::MIMEObj::MailMessage *msg;
	NEW_CLASS(msg, Text::MIMEObj::MailMessage());
	if (this->from)
	{
		msg->AddHeader(UTF8STRC("From"), this->from->v, this->from->leng);
	}
	if (this->toVals.GetLength() > 0)
	{
		msg->AddHeader(UTF8STRC("To"), this->toVals.ToString(), this->toVals.GetLength());
	}
	if (this->replyTo)
	{
		msg->AddHeader(UTF8STRC("Reply-To"), this->replyTo->v, this->replyTo->leng);
	}
	if (this->ccVals.GetLength() > 0)
	{
		msg->AddHeader(UTF8STRC("CC"), this->ccVals.ToString(), this->ccVals.GetLength());
	}
	if (this->subject)
	{
		msg->AddHeader(UTF8STRC("Subject"), this->subject->v, this->subject->leng);
	}
	if (this->attachName.GetCount() > 0)
	{
		Text::MIMEObj::MultipartMIMEObj *mpart;
		Text::IMIMEObj *obj;
		Text::StringBuilderUTF8 sbc;
		Text::CString contType;
		UOSInt i;
		UOSInt j;
		UOSInt k;
		UOSInt l;
		Text::String *fname;
		NEW_CLASS(mpart, Text::MIMEObj::MultipartMIMEObj(CSTR("multipart/mixed"), CSTR("This is a multi-part message in MIME format.")));
		if (this->content)
		{
			i = mpart->AddPart(this->content->Clone());
			contType = this->content->GetContentType();
			mpart->AddPartHeader(i, UTF8STRC("Content-Type"), contType.v, contType.leng);
		}

		i = 0;
		j = this->attachName.GetCount();
		while (i < j)
		{
			obj = this->attachObj.GetItem(i);
			fname = this->attachName.GetItem(i);
			k = mpart->AddPart(obj->Clone());
			l = fname->LastIndexOf(IO::Path::PATH_SEPERATOR);
			sbc.ClearStr();
			contType = obj->GetContentType();
			sbc.AppendC(contType.v, contType.leng);
			sbc.AppendC(UTF8STRC(";\r\n\tname=\""));
			this->AppendStr(sbc, fname->ToCString().Substring(l + 1));
			sbc.AppendC(UTF8STRC("\""));
			mpart->AddPartHeader(k, UTF8STRC("Content-Type"), sbc.ToString(), sbc.GetLength());
			mpart->AddPartHeader(k, UTF8STRC("Content-Transfer-Encoding"), UTF8STRC("base64"));
			sbc.ClearStr();
			sbc.AppendC(UTF8STRC("attachment; \r\n\tfilename=\""));
			this->AppendStr(sbc, fname->ToCString().Substring(l + 1));
			sbc.AppendC(UTF8STRC("\""));
			mpart->AddPartHeader(k, UTF8STRC("Content-Disposition"), sbc.ToString(), sbc.GetLength());
			i++;
		}
		msg->SetContent(mpart);

		contType = mpart->GetContentType();
		msg->AddHeader(UTF8STRC("Content-Type"), contType.v, contType.leng);
	}
	else
	{
		if (this->content)
		{
			Text::CString contType = this->content->GetContentType();
			msg->AddHeader(UTF8STRC("Content-Type"), contType.v, contType.leng);
			msg->SetContent(this->content->Clone());
		}
	}
	return msg;
}
