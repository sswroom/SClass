#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Encrypt/Base64.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/StringBuilder.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/MIMEObj/MailMessage.h"
#include "Text/MIMEObj/MultipartMIMEObj.h"

#define BUFFSIZE 4096

Text::MIMEObj::MailMessage::MailMessage() : Text::IMIMEObj(CSTR("message/rfc822"))
{
	this->content = 0;
}

Text::MIMEObj::MailMessage::~MailMessage()
{
	SDEL_CLASS(this->content);
}

Text::CString Text::MIMEObj::MailMessage::GetClassName()
{
	return {UTF8STRC("MailMessage")};
}

Text::CString Text::MIMEObj::MailMessage::GetContentType()
{
	return {UTF8STRC("message/rfc822")};
}

UOSInt Text::MIMEObj::MailMessage::WriteStream(IO::Stream *stm)
{
	UOSInt i;
	UOSInt j;
	Text::String *s;
	Text::StringBuilderUTF8 sbc;
	i = 0;
	j = this->headerName->GetCount();
	while (i < j)
	{
		sbc.Append(this->headerName->GetItem(i));
		sbc.AppendC(UTF8STRC(": "));
		s = this->headerValue->GetItem(i);
		sbc.Append(s);
		sbc.AppendC(UTF8STRC("\r\n"));
		i++;
	}
	sbc.AppendC(UTF8STRC("\r\n"));
	stm->Write((UInt8*)sbc.ToString(), sbc.GetLength());
	i = sbc.GetLength();
	if (this->content)
	{
		i += this->content->WriteStream(stm);
	}
	return i;
}

Text::IMIMEObj *Text::MIMEObj::MailMessage::Clone()
{
	Text::MIMEObj::MailMessage *msg;
	UOSInt i;
	UOSInt j;
	NEW_CLASS(msg, Text::MIMEObj::MailMessage());
	i = 0;
	j = this->headerName->GetCount();
	while (i < j)
	{
		msg->AddHeader(this->headerName->GetItem(i), this->headerValue->GetItem(i));
		i++;
	}
	if (this->content)
	{
		msg->SetContent(this->content->Clone());
	}
	return msg;
}

void Text::MIMEObj::MailMessage::SetContent(Text::IMIMEObj *obj)
{
	SDEL_CLASS(this->content);
	this->content = obj;
}

Bool Text::MIMEObj::MailMessage::GetDate(Data::DateTime *dt)
{
	Text::String *hdr = GetHeader(UTF8STRC("Date"));
	if (hdr == 0)
		return false;
	dt->SetValue(hdr->v, hdr->leng);
	return true;
}

UTF8Char *Text::MIMEObj::MailMessage::GetFromAddr(UTF8Char *sbuff)
{
	Text::String *hdr = GetHeader(UTF8STRC("From"));
	if (hdr == 0)
		return 0;
	return ParseHeaderStr(sbuff, hdr->v);
}

UTF8Char *Text::MIMEObj::MailMessage::GetSubject(UTF8Char *sbuff)
{
	Text::String *hdr = GetHeader(UTF8STRC("Subject"));
	if (hdr == 0)
		return 0;
	return ParseHeaderStr(sbuff, hdr->v);
}

UTF8Char *Text::MIMEObj::MailMessage::GetReplyTo(UTF8Char *sbuff)
{
	Text::String *hdr = GetHeader(UTF8STRC("Reply-To"));
	if (hdr == 0)
		return 0;
	return ParseHeaderStr(sbuff, hdr->v);
}

UOSInt Text::MIMEObj::MailMessage::GetRecpList(Data::ArrayList<MailAddress*> *recpList)
{
	UOSInt i = 0;
	Text::String *hdr = GetHeader(UTF8STRC("To"));
	if (hdr)
		i += ParseAddrList(hdr->v, hdr->leng, recpList, AT_TO);
	hdr = GetHeader(UTF8STRC("CC"));
	if (hdr)
		i += ParseAddrList(hdr->v, hdr->leng, recpList, AT_CC);
	return i;
}

void Text::MIMEObj::MailMessage::FreeRecpList(Data::ArrayList<MailAddress*> *recpList)
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

Text::MIMEObj::TextMIMEObj *Text::MIMEObj::MailMessage::GetContentText()
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
			Text::IMIMEObj *obj = mpart->GetPartObj(0);
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

Text::IMIMEObj *Text::MIMEObj::MailMessage::GetContentMajor()
{
	if (this->content == 0)
		return 0;
	Text::IMIMEObj *obj = content;
	Text::CString contType = obj->GetContentType();
	Text::CString clsName = obj->GetClassName();
	if (clsName.Equals(UTF8STRC("MultipartMIMEObj")) && contType.StartsWith(UTF8STRC("multipart/mixed")))
	{
		obj = ((Text::MIMEObj::MultipartMIMEObj*)obj)->GetPartObj(0);
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
			return mpart->GetPartObj(mpart->GetPartCount() - 1);
		}
	}
	return 0;
}

Text::IMIMEObj *Text::MIMEObj::MailMessage::GetAttachment(OSInt index, Text::StringBuilderUTF8 *name)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Text::String *s;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Text::MIMEObj::MultipartMIMEObj::PartInfo *part;
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
				s = part->GetHeader(UTF8STRC("Content-Disposition"));
				if (s && s->StartsWith(UTF8STRC("attachment")))
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
						return part->GetObject();
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

Text::IMIMEObj *Text::MIMEObj::MailMessage::GetRAWContent()
{
	return this->content;
}

Text::MIMEObj::MailMessage *Text::MIMEObj::MailMessage::ParseFile(IO::IStreamData *fd)
{
	UInt8 *buff;
	UOSInt buffSize;
	UOSInt readSize;
	UInt64 fileOfst;
	Text::MIMEObj::MailMessage *mail;
	Text::StringBuilderUTF8 sb;
	Text::PString sarr[2];
	Bool isFirst = true;
	Bool found = false;
	UOSInt i;
	UOSInt lineStart;

	buff = MemAlloc(UInt8, BUFFSIZE + 1);
	NEW_CLASS(mail, Text::MIMEObj::MailMessage());
	fileOfst = 0;
	buffSize = 0;
	while (true)
	{
		readSize = fd->GetRealData(fileOfst, BUFFSIZE - buffSize, &buff[buffSize]);
		if (readSize == 0)
		{
			if (buffSize > 0 && (buff[0] == '\r' || buff[0] == '\n'))
			{
				if (isFirst)
				{
				}
				else
				{
					found = true;
					lineStart = i + 1;
					break;
				}
			}
			break;
		}
		buffSize += readSize;
		fileOfst += readSize;
		lineStart = 0;
		i = 0;
		while (i < buffSize - 1)
		{
			if (buff[i] == '\r' && buff[i + 1] == '\n')
			{
				if (i == lineStart)
				{
					if (isFirst)
					{
					}
					else
					{
						found = true;
						lineStart = i + 2;
						break;
					}
				}
				else
				{
					isFirst = false;
					buff[i] = 0;
					if (buff[lineStart] == '\t')
					{
						sb.AppendC((UTF8Char*)&buff[lineStart + 1], i - lineStart - 1);
					}
					else if (buff[lineStart] == ' ')
					{
						sb.AppendC((UTF8Char*)&buff[lineStart + 1], i - lineStart - 1);
					}
					else
					{
						sb.AppendC((UTF8Char*)&buff[lineStart], i - lineStart);
					}
					if (buff[i - 1] != ';' || (buff[i + 2] != '\t' && buff[i + 2] != ' '))
					{
						if (Text::StrSplitTrimP(sarr, 2, sb, ':') == 1)
						{
							lineStart = 0;
							break;
						}
						mail->AddHeader(sarr[0].v, sarr[0].leng, sarr[1].v, sarr[1].leng);
						sb.ClearStr();
					}
				}
				i += 2;
				lineStart = i;
			}
			else if (buff[i] == '\r' || buff[i] == '\n')
			{
				if (i == lineStart)
				{
					if (isFirst)
					{
					}
					else
					{
						found = true;
						lineStart = i + 2;
						break;
					}
				}
				else
				{
					isFirst = false;
					buff[i] = 0;
					if (buff[lineStart] == '\t')
					{
						sb.AppendC((UTF8Char*)&buff[lineStart + 1], i - lineStart - 1);
					}
					else if (buff[lineStart] == ' ')
					{
						sb.AppendC((UTF8Char*)&buff[lineStart + 1], i - lineStart - 1);
					}
					else
					{
						sb.AppendC((UTF8Char*)&buff[lineStart], i - lineStart);
					}
					if (buff[i - 1] != ';' || (buff[i + 2] != '\t' && buff[i + 2] != ' '))
					{
						if (Text::StrSplitTrimP(sarr, 2, sb, ':') == 1)
						{
							lineStart = 0;
							break;
						}
						mail->AddHeader(sarr[0].v, sarr[0].leng, sarr[1].v, sarr[1].leng);
						sb.ClearStr();
					}
				}
				i += 1;
				lineStart = i;
			}
			else
			{
				i++;
			}
		}
		if (lineStart == 0)
			break;
		if (found)
			break;
		if (lineStart < buffSize)
		{
			MemCopyO(buff, &buff[lineStart], buffSize - lineStart);
			buffSize -= lineStart;
		}
		else
		{
			buffSize = 0;
		}
	}

	MemFree(buff);
	if (!found)
	{
		DEL_CLASS(mail);
		mail = 0;
		return 0;
	}

	UInt64 contentOfst = fileOfst - buffSize + lineStart;
	Text::String *contentLen = mail->GetHeader(UTF8STRC("Content-Length"));
	if (contentLen)
	{
		if (Text::StrToUInt64(contentLen->v) == fd->GetDataSize() - contentOfst)
		{
			IO::IStreamData *data = fd->GetPartialData(contentOfst, Text::StrToUInt64(contentLen->v));
			Text::String *contType = mail->GetHeader(UTF8STRC("Content-Type"));
			Text::IMIMEObj *obj = Text::IMIMEObj::ParseFromData(data, STR_CSTR(contType));
			DEL_CLASS(data);
			if (obj)
			{
				mail->SetContent(obj);
			}
		}
	}
	else
	{
		IO::IStreamData *data = fd->GetPartialData(contentOfst, fd->GetDataSize() - contentOfst);
		Text::String *contType = mail->GetHeader(UTF8STRC("Content-Type"));
		Text::IMIMEObj *obj = Text::IMIMEObj::ParseFromData(data, STR_CSTR(contType));
		DEL_CLASS(data);
		if (obj)
		{
			mail->SetContent(obj);
		}
	}
	return mail;
}

UOSInt Text::MIMEObj::MailMessage::ParseAddrList(const UTF8Char *hdr, UOSInt hdrLen, Data::ArrayList<MailAddress*> *recpList, AddressType type)
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
				addr->name = Text::String::New(ptr1, (UOSInt)(ptr1End - ptr1));
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
