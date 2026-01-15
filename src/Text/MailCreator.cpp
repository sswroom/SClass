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

void Text::MailCreator::AppendStr(NN<Text::StringBuilderUTF8> sbc, Text::CStringNN s)
{
	UnsafeArray<const UTF8Char> sptr;
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
		b64Size = b64.Encrypt(s.v, strLen, b64Buff);
		b64Buff[b64Size] = 0;
		sbc->AppendC(b64Buff, b64Size);
		MemFree(b64Buff);
	}
	else
	{
		sbc->Append(s);
	}
}

void Text::MailCreator::AppendStr(NN<Text::StringBuilderUTF8> sbc, const WChar *s)
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
		NN<Text::String> str = Text::String::NewNotNull(s);
		UOSInt buffSize;
		UInt8 *b64Buff;
		UOSInt b64Size;
		Crypto::Encrypt::Base64 b64;
		buffSize = str->leng;
		b64Size = (buffSize / 3) * 4 + 4;
		b64Buff = MemAlloc(UInt8, b64Size + 1);
		b64Size = b64.Encrypt(str->v, buffSize, b64Buff);
		b64Buff[b64Size] = 0;
		sbc->AppendC(b64Buff, b64Size);
		MemFree(b64Buff);
		str->Release();
	}
	else
	{
		NN<Text::String> str = Text::String::NewNotNull(s);
		sbc->Append(str);
		str->Release();
	}
}

Optional<Text::MIMEObject> Text::MailCreator::ParseContentHTML(UnsafeArray<const UInt8> buff, UOSInt buffSize, UInt32 codePage, Text::CStringNN htmlPath)
{
	UOSInt j;
	UOSInt endOfst = buffSize - 6;
	UOSInt i;
	UOSInt k;
	UOSInt l;
	Data::ArrayListNN<Text::MIMEObject> imgs;
	NN<Text::MIMEObject> obj;
	Text::StringBuilderUTF8 sbc;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
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
						if (Text::MIMEObject::ParseFromFile(CSTRP(sbuff, sptr)).SetTo(obj))
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
		NEW_CLASSNN(obj, Text::MIMEObj::HTMLMIMEObj(sbc.ToString(), sbc.GetLength(), 65001));
		return obj;
	}
	else
	{
		Text::MIMEObj::MultipartMIMEObj *mpart;
		NEW_CLASS(mpart, Text::MIMEObj::MultipartMIMEObj(CSTR("multipart/related"), nullptr));
		NEW_CLASSNN(obj, Text::MIMEObj::HTMLMIMEObj(sbc.ToString(), sbc.GetLength(), 65001));
		i = mpart->AddPart(obj);
		Text::CStringNN contType = obj->GetContentType();
		mpart->AddPartHeader(i, CSTR("Content-Type"), contType);
		mpart->AddPartHeader(i, CSTR("Content-Transfer-Encoding"), CSTR("8bit"));

		i = 0;
		j = imgs.GetCount();
		while (i < j)
		{
			obj = imgs.GetItemNoCheck(i);
			k = mpart->AddPart(obj);
			contType = obj->GetContentType();
			mpart->AddPartHeader(k, CSTR("Content-Type"), contType);
			mpart->AddPartHeader(k, CSTR("Content-Transfer-Encoding"), CSTR("base64"));
			sbc.ClearStr();
			sbc.AppendC(UTF8STRC("<image"));
			sbc.AppendUOSInt(i + 1);
			sbc.AppendC(UTF8STRC(">"));
			mpart->AddPartHeader(k, CSTR("Content-ID"), sbc.ToCString());

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
	this->content = nullptr;
}

Text::MailCreator::~MailCreator()
{
	UOSInt i;
	NN<Text::MIMEObject> obj;

	SDEL_STRING(this->from);
	SDEL_STRING(this->replyTo);
	SDEL_STRING(this->subject);
	this->content.Delete();
	i = this->attachName.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(this->attachName.GetItem(i));
		obj = this->attachObj.GetItemNoCheck(i);
		obj.Delete();
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
	Text::CStringNN nnname;
	Text::CStringNN nnaddress;
	if (!address.SetTo(nnaddress) || nnaddress.leng == 0)
	{
		SDEL_STRING(this->from);
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		if (name.SetTo(nnname) && nnname.leng != 0)
		{
			this->AppendStr(sb, nnname);
			sb.AppendC(UTF8STRC(" <"));
			this->AppendStr(sb, nnaddress);
			sb.AppendC(UTF8STRC(">"));
		}
		else
		{
			this->AppendStr(sb, nnaddress);
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

void Text::MailCreator::ToAdd(Text::String *name, NN<Text::String> address)
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

void Text::MailCreator::CCAdd(Text::String *name, NN<Text::String> address)
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

void Text::MailCreator::SetSubject(NN<Text::String> subj)
{
	Text::StringBuilderUTF8 sb;
	this->AppendStr(sb, subj->ToCString());
	SDEL_STRING(this->subject);
	this->subject = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
}

void Text::MailCreator::SetContentHTML(const WChar *content, Text::CStringNN htmlPath)
{
	NN<Text::MIMEObject> obj;
	UOSInt strLen = Text::StrCharCnt(content);
	UOSInt buffSize = Text::StrWChar_UTF8CntC(content, strLen);
	UInt8 *buff = MemAlloc(UInt8, buffSize + 1);
	Text::StrWChar_UTF8C(buff, content, strLen);
	if (ParseContentHTML(buff, buffSize, 65001, htmlPath).SetTo(obj))
	{
		this->content.Delete();
		this->content = obj;
	}
	MemFree(buff);
}

void Text::MailCreator::SetContentHTML(NN<Text::String> content, Text::CStringNN htmlPath)
{
	NN<Text::MIMEObject> obj;
	if (ParseContentHTML(content->v, content->leng, 65001, htmlPath).SetTo(obj))
	{
		this->content.Delete();
		this->content = obj;
	}
}

void Text::MailCreator::SetContentText(const WChar *content, UInt32 codePage)
{
	NN<Text::MIMEObj::TextMIMEObj> obj;
	NEW_CLASSNN(obj, Text::MIMEObj::TextMIMEObj(content, codePage));
	this->content.Delete();
	this->content = obj;
}

void Text::MailCreator::SetContentText(NN<Text::String> content)
{
	NN<Text::MIMEObj::TextMIMEObj> obj;
	NEW_CLASSNN(obj, Text::MIMEObj::TextMIMEObj(content->v, content->leng, 65001));
	this->content.Delete();
	this->content = obj;
}

Bool Text::MailCreator::SetContentFile(Text::CStringNN filePath)
{
	NN<Text::MIMEObject> obj;
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
	if (ParseContentHTML(buff, buffSize, 65001, filePath).SetTo(obj))
	{
		MemFree(buff);
		this->content.Delete();
		this->content = obj;
		return true;
	}
	MemFree(buff);
	return false;
}

void Text::MailCreator::AddAttachment(Text::CStringNN fileName)
{
	NN<Text::MIMEObject> obj;
	if (Text::MIMEObject::ParseFromFile(fileName).SetTo(obj))
	{
		this->attachObj.Add(obj);
		this->attachName.Add(Text::String::New(fileName));
	}
}

NN<Text::MIMEObj::MailMessage> Text::MailCreator::CreateMail()
{
	NN<Text::MIMEObj::MailMessage> msg;
	NEW_CLASSNN(msg, Text::MIMEObj::MailMessage());
	if (this->from)
	{
		msg->AddHeader(CSTR("From"), this->from->ToCString());
	}
	if (this->toVals.GetLength() > 0)
	{
		msg->AddHeader(CSTR("To"), this->toVals.ToCString());
	}
	if (this->replyTo)
	{
		msg->AddHeader(CSTR("Reply-To"), this->replyTo->ToCString());
	}
	if (this->ccVals.GetLength() > 0)
	{
		msg->AddHeader(CSTR("CC"), this->ccVals.ToCString());
	}
	if (this->subject)
	{
		msg->AddHeader(CSTR("Subject"), this->subject->ToCString());
	}
	if (this->attachName.GetCount() > 0)
	{
		Text::MIMEObj::MultipartMIMEObj *mpart;
		NN<Text::MIMEObject> obj;
		Text::StringBuilderUTF8 sbc;
		Text::CStringNN contType;
		UOSInt i;
		UOSInt j;
		UOSInt k;
		UOSInt l;
		NN<Text::String> fname;
		NEW_CLASS(mpart, Text::MIMEObj::MultipartMIMEObj(CSTR("multipart/mixed"), CSTR("This is a multi-part message in MIME format.")));
		if (this->content.SetTo(obj))
		{
			i = mpart->AddPart(obj->Clone());
			contType = obj->GetContentType();
			mpart->AddPartHeader(i, CSTR("Content-Type"), contType);
		}

		i = 0;
		j = this->attachName.GetCount();
		while (i < j)
		{
			if (this->attachName.GetItem(i).SetTo(fname))
			{
				obj = this->attachObj.GetItemNoCheck(i);
				k = mpart->AddPart(obj->Clone());
				l = fname->LastIndexOf(IO::Path::PATH_SEPERATOR);
				sbc.ClearStr();
				contType = obj->GetContentType();
				sbc.AppendC(contType.v, contType.leng);
				sbc.AppendC(UTF8STRC(";\r\n\tname=\""));
				this->AppendStr(sbc, fname->ToCString().Substring(l + 1));
				sbc.AppendC(UTF8STRC("\""));
				mpart->AddPartHeader(k, CSTR("Content-Type"), sbc.ToCString());
				mpart->AddPartHeader(k, CSTR("Content-Transfer-Encoding"), CSTR("base64"));
				sbc.ClearStr();
				sbc.AppendC(UTF8STRC("attachment; \r\n\tfilename=\""));
				this->AppendStr(sbc, fname->ToCString().Substring(l + 1));
				sbc.AppendC(UTF8STRC("\""));
				mpart->AddPartHeader(k, CSTR("Content-Disposition"), sbc.ToCString());
			}
			i++;
		}
		msg->SetContent(mpart);

		contType = mpart->GetContentType();
		msg->AddHeader(CSTR("Content-Type"), contType);
	}
	else
	{
		NN<Text::MIMEObject> content;
		if (this->content.SetTo(content))
		{
			Text::CStringNN contType = content->GetContentType();
			msg->AddHeader(CSTR("Content-Type"), contType);
			msg->SetContent(content->Clone());
		}
	}
	return msg;
}
