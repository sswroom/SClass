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

Text::MIMEObj::MailMessage::MailMessage() : Text::IMIMEObj((const UTF8Char*)"message/rfc822")
{
	this->content = 0;
}

Text::MIMEObj::MailMessage::~MailMessage()
{
	SDEL_CLASS(this->content);
}

const UTF8Char *Text::MIMEObj::MailMessage::GetClassName()
{
	return (const UTF8Char*)"MailMessage";
}

const UTF8Char *Text::MIMEObj::MailMessage::GetContentType()
{
	return (const UTF8Char *)"message/rfc822";
}

OSInt Text::MIMEObj::MailMessage::WriteStream(IO::Stream *stm)
{
	OSInt i;
	OSInt j;
	const UTF8Char *csptr;
	Text::StringBuilderUTF8 sbc;
	i = 0;
	j = this->headerName->GetCount();
	while (i < j)
	{
		sbc.Append(this->headerName->GetItem(i));
		sbc.Append((const UTF8Char*)": ");
		csptr = this->headerValue->GetItem(i);
		sbc.Append(csptr);
		sbc.Append((const UTF8Char*)"\r\n");
		i++;
	}
	sbc.Append((const UTF8Char*)"\r\n");
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
	OSInt i;
	OSInt j;
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
	const UTF8Char *hdr = GetHeader((const UTF8Char*)"Date");
	if (hdr == 0)
		return false;
	dt->SetValue(hdr);
	return true;
}

UTF8Char *Text::MIMEObj::MailMessage::GetFromAddr(UTF8Char *sbuff)
{
	const UTF8Char *hdr = GetHeader((const UTF8Char*)"From");
	if (hdr == 0)
		return 0;
	return ParseHeaderStr(sbuff, hdr);
}

UTF8Char *Text::MIMEObj::MailMessage::GetSubject(UTF8Char *sbuff)
{
	const UTF8Char *hdr = GetHeader((const UTF8Char*)"Subject");
	if (hdr == 0)
		return 0;
	return ParseHeaderStr(sbuff, hdr);
}

UTF8Char *Text::MIMEObj::MailMessage::GetReplyTo(UTF8Char *sbuff)
{
	const UTF8Char *hdr = GetHeader((const UTF8Char*)"Reply-To");
	if (hdr == 0)
		return 0;
	return ParseHeaderStr(sbuff, hdr);
}

OSInt Text::MIMEObj::MailMessage::GetRecpList(Data::ArrayList<MailAddress*> *recpList)
{
	OSInt i = 0;
	const UTF8Char *hdr = GetHeader((const UTF8Char*)"To");
	if (hdr)
		i += ParseAddrList(hdr, recpList, AT_TO);
	hdr = GetHeader((const UTF8Char*)"CC");
	if (hdr)
		i += ParseAddrList(hdr, recpList, AT_CC);
	return i;
}

void Text::MIMEObj::MailMessage::FreeRecpList(Data::ArrayList<MailAddress*> *recpList)
{
	MailAddress *addr;
	OSInt i;
	i = recpList->GetCount();
	while (i-- > 0)
	{
		addr = recpList->RemoveAt(i);
		if (addr->name)
			Text::StrDelNew(addr->name);
		Text::StrDelNew(addr->address);
		MemFree(addr);
	}
}

Text::MIMEObj::TextMIMEObj *Text::MIMEObj::MailMessage::GetContentText()
{
	if (this->content == 0)
		return 0;
	if (Text::StrEquals(this->content->GetClassName(), (const UTF8Char*)"TextMIMEObj"))
		return (Text::MIMEObj::TextMIMEObj*)this->content;
	if (Text::StrEquals(this->content->GetClassName(), (const UTF8Char*)"MultipartMIMEObj"))
	{
		if (Text::StrStartsWith(this->content->GetContentType(), (const UTF8Char*)"multipart/mixed"))
		{
			Text::MIMEObj::MultipartMIMEObj *mpart = (Text::MIMEObj::MultipartMIMEObj*)this->content;
			Text::IMIMEObj *obj = mpart->GetPartObj(0);
			if (obj)
			{
				if (Text::StrEquals(obj->GetClassName(), (const UTF8Char*)"TextMIMEObj"))
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
	if (Text::StrEquals(obj->GetClassName(), (const UTF8Char*)"MultipartMIMEObj") && Text::StrStartsWith(obj->GetContentType(), (const UTF8Char*)"multipart/mixed"))
	{
		obj = ((Text::MIMEObj::MultipartMIMEObj*)obj)->GetPartObj(0);
	}
	if (Text::StrEquals(obj->GetClassName(), (const UTF8Char*)"TextMIMEObj"))
		return obj;
	if (Text::StrEquals(obj->GetClassName(), (const UTF8Char*)"HTMLMIMEObj"))
		return obj;
	if (Text::StrEquals(obj->GetClassName(), (const UTF8Char*)"MultipartMIMEObj"))
	{
		if (Text::StrStartsWith(obj->GetContentType(), (const UTF8Char*)"multipart/related"))
			return obj;
		if (Text::StrStartsWith(obj->GetContentType(), (const UTF8Char*)"multipart/alternative"))
		{
			Text::MIMEObj::MultipartMIMEObj *mpart = (Text::MIMEObj::MultipartMIMEObj*)obj;
			return mpart->GetPartObj(mpart->GetPartCount() - 1);
		}
	}
	return 0;
}

Text::IMIMEObj *Text::MIMEObj::MailMessage::GetAttachment(OSInt index, Text::StringBuilderUTF *name)
{
	OSInt i;
	OSInt j;
	const UTF8Char *cptr;
	UTF8Char sbuff[512];
	Text::MIMEObj::MultipartMIMEObj::PartInfo *part;
	if (this->content == 0)
		return 0;
	if (Text::StrEquals(this->content->GetClassName(), (const UTF8Char*)"MultipartMIMEObj"))
	{
		if (Text::StrStartsWith(this->content->GetContentType(), (const UTF8Char*)"multipart/mixed"))
		{
			Text::MIMEObj::MultipartMIMEObj *mpart = (Text::MIMEObj::MultipartMIMEObj*)this->content;
			i = 0;
			j = mpart->GetPartCount();
			while (i < j)
			{
				part = mpart->GetPart(i);
				cptr = part->GetHeader((const UTF8Char*)"Content-Disposition");
				if (cptr && Text::StrStartsWith(cptr, (const UTF8Char*)"attachment"))
				{
					if (index == 0)
					{
						ParseHeaderStr(sbuff, cptr);
						i = Text::StrIndexOf(sbuff, (const UTF8Char*)"filename=");
						if (i > 0)
						{
							if (sbuff[i + 9] == '\"')
							{
								j = Text::StrIndexOf(&sbuff[i + 10], '\"');
								if (j >= 0)
								{
									name->AppendC(&sbuff[i + 10], j);
								}
								else
								{
									name->Append(&sbuff[i + 10]);
								}
							}
							else
							{
								j = Text::StrIndexOf(&sbuff[i + 9], ' ');
								if (j >= 0)
								{
									name->AppendC(&sbuff[i + 9], j);
								}
								else
								{
									name->Append(&sbuff[i + 9]);
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
	OSInt buffSize;
	OSInt readSize;
	Int64 fileOfst;
	Text::MIMEObj::MailMessage *mail;
	Text::StringBuilderUTF8 sb;
	UTF8Char *sptr;
	UTF8Char *sarr[2];
	Bool isFirst = true;
	Bool found = false;
	OSInt i;
	OSInt lineStart;

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
						sb.Append((UTF8Char*)&buff[lineStart + 1]);
					}
					else if (buff[lineStart] == ' ')
					{
						sb.Append((UTF8Char*)&buff[lineStart + 1]);
					}
					else
					{
						sb.Append((UTF8Char*)&buff[lineStart]);
					}
					if (buff[i - 1] != ';' || (buff[i + 2] != '\t' && buff[i + 2] != ' '))
					{
						sptr = sb.ToString();
						if (Text::StrSplitTrim(sarr, 2, sptr, ':') == 1)
						{
							lineStart = 0;
							break;
						}
						mail->AddHeader(sarr[0], sarr[1]);
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
						sb.Append((UTF8Char*)&buff[lineStart + 1]);
					}
					else if (buff[lineStart] == ' ')
					{
						sb.Append((UTF8Char*)&buff[lineStart + 1]);
					}
					else
					{
						sb.Append((UTF8Char*)&buff[lineStart]);
					}
					if (buff[i - 1] != ';' || (buff[i + 2] != '\t' && buff[i + 2] != ' '))
					{
						sptr = sb.ToString();
						if (Text::StrSplitTrim(sarr, 2, sptr, ':') == 1)
						{
							lineStart = 0;
							break;
						}
						mail->AddHeader(sarr[0], sarr[1]);
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

	Int64 contentOfst = fileOfst - buffSize + lineStart;
	const UTF8Char *contentLen = mail->GetHeader((const UTF8Char*)"Content-Length");
	if (contentLen)
	{
		if ((UInt64)Text::StrToInt64(contentLen) == fd->GetDataSize() - contentOfst)
		{
			IO::IStreamData *data = fd->GetPartialData(contentOfst, Text::StrToInt64(contentLen));
			Text::IMIMEObj *obj = Text::IMIMEObj::ParseFromData(data, mail->GetHeader((const UTF8Char*)"Content-Type"));
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
		Text::IMIMEObj *obj = Text::IMIMEObj::ParseFromData(data, mail->GetHeader((const UTF8Char*)"Content-Type"));
		DEL_CLASS(data);
		if (obj)
		{
			mail->SetContent(obj);
		}
	}
	return mail;
}

OSInt Text::MIMEObj::MailMessage::ParseAddrList(const UTF8Char *hdr, Data::ArrayList<MailAddress*> *recpList, AddressType type)
{
	UTF8Char *sbuff;
	OSInt hdrLen;
	UTF8Char *sptr;
	UTF8Char *ptr1;
	UTF8Char *ptr2;
	OSInt ret = 0;
	UTF8Char c;
	Bool isEnd;
	Bool quoted;
	MailAddress *addr;
	hdrLen = Text::StrCharCnt(hdr);
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
				Text::StrTrim(ptr1);
				addr->name = Text::StrCopyNew(ptr1);
				ptr1 = ptr2;
				while (true)
				{
					c = *ptr2++;
					if (c == '0' || c == '>')
					{
						ptr2[-1] = 0;
						addr->address = Text::StrCopyNew(ptr1);
						break;
					}
				}
				break;
			}
			else if (c == 0)
			{
				addr->address = Text::StrCopyNew(ptr1);
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
