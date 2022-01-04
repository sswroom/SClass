#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Encrypt/Base64.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Net/MIME.h"
#include "Text/Encoding.h"
#include "Text/MailCreator.h"
#include "Text/MIMEObj/HTMLMIMEObj.h"
#include "Text/MIMEObj/MultipartMIMEObj.h"
#include "Text/MIMEObj/TextMIMEObj.h"
#include "Text/MIMEObj/UnknownMIMEObj.h"

void Text::MailCreator::AppendStr(Text::StringBuilderUTF8 *sbc, const UTF8Char *s)
{
	const UTF8Char *sptr;
	UTF8Char c;
	Bool found = false;
	sptr = s;
	while (c = *sptr++)
	{
		if (c >= 0x80)
		{
			found = true;
			break;
		}
	}
	if (found)
	{
		OSInt strLen;
		UInt8 *b64Buff;
		OSInt b64Size;
		Crypto::Encrypt::Base64 b64;
		strLen = Text::StrCharCnt(s);
		b64Size = (strLen / 3) * 4 + 4;
		b64Buff = MemAlloc(UInt8, b64Size + 1);
		b64Size = b64.Encrypt(s, strLen, b64Buff, 0);
		b64Buff[b64Size] = 0;
		sbc->Append((const UTF8Char*)b64Buff);
		MemFree(b64Buff);
	}
	else
	{
		sptr = s;
		while (c = *sptr++)
		{
			sbc->AppendChar(c, 1);
		}
	}
}

void Text::MailCreator::AppendStr(Text::StringBuilderUTF8 *sbc, const WChar *s)
{
	const WChar *sptr;
	WChar c;
	Bool found = false;
	sptr = s;
	while (c = *sptr++)
	{
		if (c >= 0x80)
		{
			found = true;
			break;
		}
	}
	if (found)
	{
		const UTF8Char *u8ptr = Text::StrToUTF8New(s);
		OSInt buffSize;
		UInt8 *b64Buff;
		OSInt b64Size;
		Crypto::Encrypt::Base64 b64;
		buffSize = Text::StrCharCnt(u8ptr);
		b64Size = (buffSize / 3) * 4 + 4;
		b64Buff = MemAlloc(UInt8, b64Size + 1);
		b64Size = b64.Encrypt(u8ptr, buffSize, b64Buff, 0);
		b64Buff[b64Size] = 0;
		sbc->Append((const UTF8Char*)b64Buff);
		MemFree(b64Buff);
		Text::StrDelNew(u8ptr);
	}
	else
	{
		const UTF8Char *u8ptr = Text::StrToUTF8New(s);
		sbc->Append(u8ptr);
		Text::StrDelNew(u8ptr);
	}
}

Text::IMIMEObj *Text::MailCreator::ParseContentHTML(UInt8 *buff, UOSInt buffSize, UInt32 codePage, const UTF8Char *htmlPath)
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
		if (Text::StrStartsWithICase((Char*)&buff[i], " src=\""))
		{
			i += 6;
			found = true;
		}
		else if (Text::StrStartsWithICase((Char*)&buff[i], " background=\""))
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
					sptr = Text::StrConcat(sbuff, htmlPath);
					l = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR );
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
						obj = Text::IMIMEObj::ParseFromFile(sbuff);
						if (obj)
						{
							imgs.Add(obj);
							sbc.AppendC((const UTF8Char*)&buff[j], i - j);
							sbc.AppendC(UTF8STRC("cid:image"));
							sbc.AppendOSInt(imgs.GetCount());
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
		NEW_CLASS(obj, Text::MIMEObj::HTMLMIMEObj((UInt8*)sbc.ToString(), sbc.GetLength(), 65001));
		return obj;
	}
	else
	{
		Text::MIMEObj::MultipartMIMEObj *mpart;
		NEW_CLASS(mpart, Text::MIMEObj::MultipartMIMEObj((const UTF8Char*)"multipart/related", 0));
		NEW_CLASS(obj, Text::MIMEObj::HTMLMIMEObj((UInt8*)sbc.ToString(), sbc.GetLength(), 65001));
		i = mpart->AddPart(obj);
		mpart->AddPartHeader(i, (const UTF8Char*)"Content-Type", obj->GetContentType());
		mpart->AddPartHeader(i, (const UTF8Char*)"Content-Transfer-Encoding", (const UTF8Char*)"8bit");

		i = 0;
		j = imgs.GetCount();
		while (i < j)
		{
			obj = imgs.GetItem(i);
			k = mpart->AddPart(obj);
			mpart->AddPartHeader(k, (const UTF8Char*)"Content-Type", obj->GetContentType());
			mpart->AddPartHeader(k, (const UTF8Char*)"Content-Transfer-Encoding", (const UTF8Char*)"base64");
			sbc.ClearStr();
			sbc.AppendC(UTF8STRC("<image"));
			sbc.AppendOSInt(i + 1);
			sbc.AppendC(UTF8STRC(">"));
			mpart->AddPartHeader(k, (const UTF8Char*)"Content-ID", sbc.ToString());

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
	NEW_CLASS(this->toVals, Text::StringBuilderUTF8());
	NEW_CLASS(this->ccVals, Text::StringBuilderUTF8());
	NEW_CLASS(this->attachName, Data::ArrayList<const UTF8Char*>());
	NEW_CLASS(this->attachObj, Data::ArrayList<Text::IMIMEObj*>());
}

Text::MailCreator::~MailCreator()
{
	OSInt i;
	Text::IMIMEObj *obj;

	SDEL_TEXT(this->from);
	SDEL_TEXT(this->replyTo);
	SDEL_TEXT(this->subject);
	SDEL_CLASS(this->content);
	DEL_CLASS(this->toVals);
	DEL_CLASS(this->ccVals);
	i = this->attachName->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->attachName->GetItem(i));
		obj = this->attachObj->GetItem(i);
		DEL_CLASS(obj);
	}
	DEL_CLASS(this->attachName);
	DEL_CLASS(this->attachObj);
}

void Text::MailCreator::SetFrom(const WChar *name, const WChar *address)
{
	if (address == 0)
	{
		SDEL_TEXT(this->from);
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		if (name)
		{
			this->AppendStr(&sb, name);
			sb.AppendC(UTF8STRC(" <"));
			this->AppendStr(&sb, address);
			sb.AppendC(UTF8STRC(">"));
		}
		else
		{
			this->AppendStr(&sb, address);
		}
		SDEL_TEXT(this->from);
		this->from = Text::StrCopyNew(sb.ToString());
	}
}

void Text::MailCreator::SetReplyTo(const WChar *name, const WChar *address)
{
	if (address == 0)
	{
		SDEL_TEXT(this->replyTo);
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		if (name)
		{
			this->AppendStr(&sb, name);
			sb.AppendC(UTF8STRC(" <"));
			this->AppendStr(&sb, address);
			sb.AppendC(UTF8STRC(">"));
		}
		else
		{
			this->AppendStr(&sb, address);
		}
		SDEL_TEXT(this->replyTo);
		this->replyTo = Text::StrCopyNew(sb.ToString());
	}
}

void Text::MailCreator::ToAdd(const WChar *name, const WChar *address)
{
	if (this->toVals->GetLength() > 0)
	{
		this->toVals->AppendC(UTF8STRC(", "));
	}
	if (name)
	{
		this->AppendStr(this->toVals, name);
		this->toVals->AppendC(UTF8STRC(" <"));
		this->AppendStr(this->toVals, address);
		this->toVals->AppendC(UTF8STRC(">"));
	}
	else
	{
		this->AppendStr(this->toVals, address);
	}
}

void Text::MailCreator::ToClear()
{
	this->toVals->ClearStr();
}

void Text::MailCreator::CCAdd(const WChar *name, const WChar *address)
{
	if (this->ccVals->GetLength() > 0)
	{
		this->ccVals->AppendC(UTF8STRC(", "));
	}
	if (name)
	{
		this->AppendStr(this->ccVals, name);
		this->ccVals->AppendC(UTF8STRC(" <"));
		this->AppendStr(this->ccVals, address);
		this->ccVals->AppendC(UTF8STRC(">"));
	}
	else
	{
		this->AppendStr(this->ccVals, address);
	}
}

void Text::MailCreator::CCClear()
{
	this->toVals->ClearStr();
}

void Text::MailCreator::SetSubject(const WChar *subj)
{
	Text::StringBuilderUTF8 sb;
	this->AppendStr(&sb, subj);
	SDEL_TEXT(this->subject);
	this->subject = Text::StrCopyNew(sb.ToString());
}

void Text::MailCreator::SetContentHTML(const WChar *content, const UTF8Char *htmlPath)
{
	Text::IMIMEObj *obj;
	OSInt strLen = Text::StrCharCnt(content);
	OSInt buffSize = Text::StrWChar_UTF8CntC(content, strLen);
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

void Text::MailCreator::SetContentText(const WChar *content, UInt32 codePage)
{
	Text::MIMEObj::TextMIMEObj *obj;
	NEW_CLASS(obj, Text::MIMEObj::TextMIMEObj(content, codePage));
	SDEL_CLASS(this->content);
	this->content = obj;
}

Bool Text::MailCreator::SetContentFile(const UTF8Char *filePath)
{
	Text::IMIMEObj *obj;
	OSInt buffSize;
	UInt8 *buff;
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(filePath, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}
	buffSize = (OSInt)fs->GetLength();
	buff = MemAlloc(UInt8, buffSize);
	fs->Read(buff, buffSize);
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

void Text::MailCreator::AddAttachment(const UTF8Char *fileName)
{
	Text::IMIMEObj *obj = Text::IMIMEObj::ParseFromFile(fileName);

	this->attachObj->Add(obj);
	this->attachName->Add(Text::StrCopyNew(fileName));
}

Text::MIMEObj::MailMessage *Text::MailCreator::CreateMail()
{
	Text::MIMEObj::MailMessage *msg;
	NEW_CLASS(msg, Text::MIMEObj::MailMessage());
	if (this->from)
	{
		msg->AddHeader((const UTF8Char*)"From", this->from);
	}
	if (this->toVals->GetLength() > 0)
	{
		msg->AddHeader((const UTF8Char*)"To", this->toVals->ToString());
	}
	if (this->replyTo)
	{
		msg->AddHeader((const UTF8Char*)"Reply-To", this->replyTo);
	}
	if (this->ccVals->GetLength() > 0)
	{
		msg->AddHeader((const UTF8Char*)"CC", this->ccVals->ToString());
	}
	if (this->subject)
	{
		msg->AddHeader((const UTF8Char*)"Subject", this->subject);
	}
	if (this->attachName->GetCount() > 0)
	{
		Text::MIMEObj::MultipartMIMEObj *mpart;
		Text::IMIMEObj *obj;
		Text::StringBuilderUTF8 sbc;
		UOSInt i;
		UOSInt j;
		UOSInt k;
		UOSInt l;
		const UTF8Char *fname;
		NEW_CLASS(mpart, Text::MIMEObj::MultipartMIMEObj((const UTF8Char*)"multipart/mixed", (const UTF8Char*)"This is a multi-part message in MIME format."));
		if (this->content)
		{
			i = mpart->AddPart(this->content->Clone());
			mpart->AddPartHeader(i, (const UTF8Char*)"Content-Type", this->content->GetContentType());
		}

		i = 0;
		j = this->attachName->GetCount();
		while (i < j)
		{
			obj = this->attachObj->GetItem(i);
			fname = this->attachName->GetItem(i);
			k = mpart->AddPart(obj->Clone());
			l = Text::StrLastIndexOf(fname, '\\');
			sbc.ClearStr();
			sbc.Append(obj->GetContentType());
			sbc.AppendC(UTF8STRC(";\r\n\tname=\""));
			this->AppendStr(&sbc, &fname[l + 1]);
			sbc.AppendC(UTF8STRC("\""));
			mpart->AddPartHeader(k, (const UTF8Char*)"Content-Type", sbc.ToString());
			mpart->AddPartHeader(k, (const UTF8Char*)"Content-Transfer-Encoding", (const UTF8Char*)"base64");
			sbc.ClearStr();
			sbc.AppendC(UTF8STRC("attachment; \r\n\tfilename=\""));
			this->AppendStr(&sbc, &fname[l + 1]);
			sbc.AppendC(UTF8STRC("\""));
			mpart->AddPartHeader(k, (const UTF8Char*)"Content-Disposition", sbc.ToString());
			i++;
		}
		msg->SetContent(mpart);
		msg->AddHeader((const UTF8Char*)"Content-Type", mpart->GetContentType());
	}
	else
	{
		if (this->content)
		{
			msg->AddHeader((const UTF8Char*)"Content-Type", this->content->GetContentType());
			msg->SetContent(this->content->Clone());
		}
	}
	return msg;
}
