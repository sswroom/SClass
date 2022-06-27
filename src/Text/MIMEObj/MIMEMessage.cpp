#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Encrypt/Base64.h"
#include "Text/Encoding.h"
#include "Text/EncodingFactory.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/MIMEObj/MIMEMessage.h"

#define BUFFSIZE 4096

Text::MIMEObj::MIMEMessage::MIMEMessage() : Text::IMIMEObj(CSTR("message/rfc822"))
{
	this->content = 0;
	this->transferData = 0;
	this->transferSize = 0;
}

Text::MIMEObj::MIMEMessage::MIMEMessage(Text::IMIMEObj *content) : Text::IMIMEObj(CSTR("message/rfc822"))
{
	this->content = content;
	this->transferData = 0;
	this->transferSize = 0;
}

Text::MIMEObj::MIMEMessage::~MIMEMessage()
{
	UOSInt i;
	i = this->headerName.GetCount();
	while (i-- > 0)
	{
		this->headerName.GetItem(i)->Release();
		this->headerValue.GetItem(i)->Release();
	}
	SDEL_CLASS(this->content);
	if (this->transferData)
	{
		MemFree(this->transferData);
	}
}

Text::CString Text::MIMEObj::MIMEMessage::GetClassName() const
{
	return CSTR("MIMEMessage");
}

Text::CString Text::MIMEObj::MIMEMessage::GetContentType() const
{
	Text::String *contType = this->GetHeader(UTF8STRC("Content-Type"));
	return STR_CSTR(contType);
}

UOSInt Text::MIMEObj::MIMEMessage::WriteStream(IO::Stream *stm) const
{
	UOSInt i;
	UOSInt j;
	Text::String *s;
	Text::StringBuilderUTF8 sbc;
	i = 0;
	j = this->headerName.GetCount();
	while (i < j)
	{
		sbc.Append(this->headerName.GetItem(i));
		sbc.AppendC(UTF8STRC(": "));
		s = this->headerValue.GetItem(i);
		sbc.Append(s);
		sbc.AppendC(UTF8STRC("\r\n"));
		i++;
	}
	sbc.AppendC(UTF8STRC("\r\n"));
	stm->Write((UInt8*)sbc.ToString(), sbc.GetLength());
	i = sbc.GetLength();
	if (this->transferData)
	{
		stm->Write(this->transferData, this->transferSize);
		i += this->transferSize;
	}
	else if (this->content)
	{
		i += this->content->WriteStream(stm);
	}
	return i;
}

Text::IMIMEObj *Text::MIMEObj::MIMEMessage::Clone() const
{
	Text::MIMEObj::MIMEMessage *msg;
	UOSInt i;
	UOSInt j;
	NEW_CLASS(msg, Text::MIMEObj::MIMEMessage());
	i = 0;
	j = this->headerName.GetCount();
	while (i < j)
	{
		msg->AddHeader(this->headerName.GetItem(i), this->headerValue.GetItem(i));
		i++;
	}
	if (this->content)
	{
		msg->SetContent(this->content->Clone());
	}
	return msg;
}

void Text::MIMEObj::MIMEMessage::SetContent(Text::IMIMEObj *content)
{
	SDEL_CLASS(this->content);
	this->content = content;
}

Text::IMIMEObj *Text::MIMEObj::MIMEMessage::GetContent() const
{
	return this->content;
}

void Text::MIMEObj::MIMEMessage::SetTransferData(const UInt8 *data, UOSInt dataSize)
{
	if (this->transferData)
	{
		MemFree(this->transferData);
	}
	this->transferSize = dataSize;
	this->transferData = MemAlloc(UInt8, dataSize);
	MemCopyNO(this->transferData, data, dataSize);
}

void Text::MIMEObj::MIMEMessage::AddHeader(const UTF8Char *name, UOSInt nameLen, const UTF8Char *value, UOSInt valueLen)
{
	this->headerName.Add(Text::String::New(name, nameLen));
	this->headerValue.Add(Text::String::New(value, valueLen));
}

void Text::MIMEObj::MIMEMessage::AddHeader(Text::String *name, Text::String *value)
{
	this->headerName.Add(name->Clone());
	this->headerValue.Add(value->Clone());
}

Text::String *Text::MIMEObj::MIMEMessage::GetHeader(const UTF8Char *name, UOSInt nameLen) const
{
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->headerName.GetCount();
	while (i < j)
	{
		if (this->headerName.GetItem(i)->EqualsICase(name, nameLen))
			return this->headerValue.GetItem(i);
		i++;
	}
	return 0;
}

UOSInt Text::MIMEObj::MIMEMessage::GetHeaderCount() const
{
	return this->headerName.GetCount();
}

Text::String *Text::MIMEObj::MIMEMessage::GetHeaderName(UOSInt index) const
{
	return this->headerName.GetItem(index);
}

Text::String *Text::MIMEObj::MIMEMessage::GetHeaderValue(UOSInt index) const
{
	return this->headerValue.GetItem(index);
}

Bool Text::MIMEObj::MIMEMessage::ParseFromData(IO::IStreamData *fd)
{
	UInt8 *buff;
	UOSInt buffSize;
	UOSInt readSize;
	UInt64 fileOfst;
	Text::StringBuilderUTF8 sb;
	Text::PString sarr[2];
	Bool isFirst = true;
	Bool found = false;
	UOSInt i;
	UOSInt lineStart;

	buff = MemAlloc(UInt8, BUFFSIZE + 1);
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
						sb.AppendUTF8Char(' ');
						sb.AppendC((UTF8Char*)&buff[lineStart + 1], i - lineStart - 1);
					}
					else if (buff[lineStart] == ' ')
					{
						sb.AppendC((UTF8Char*)&buff[lineStart], i - lineStart);
					}
					else
					{
						sb.AppendC((UTF8Char*)&buff[lineStart], i - lineStart);
					}
					if (buff[i - 1] != ';' && (buff[i + 2] != '\t' && buff[i + 2] != ' '))
					{
						if (Text::StrSplitTrimP(sarr, 2, sb, ':') == 1)
						{
							lineStart = 0;
							break;
						}
						this->AddHeader(sarr[0].v, sarr[0].leng, sarr[1].v, sarr[1].leng);
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
						sb.AppendUTF8Char(' ');
						sb.AppendC((UTF8Char*)&buff[lineStart + 1], i - lineStart - 1);
					}
					else if (buff[lineStart] == ' ')
					{
						sb.AppendC((UTF8Char*)&buff[lineStart], i - lineStart);
					}
					else
					{
						sb.AppendC((UTF8Char*)&buff[lineStart], i - lineStart);
					}
					if (buff[i - 1] != ';' && (buff[i + 2] != '\t' && buff[i + 2] != ' '))
					{
						if (Text::StrSplitTrimP(sarr, 2, sb, ':') == 1)
						{
							lineStart = 0;
							break;
						}
						this->AddHeader(sarr[0].v, sarr[0].leng, sarr[1].v, sarr[1].leng);
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
		return false;
	}

	UInt64 contentOfst = fileOfst - buffSize + lineStart;
	Text::String *contentLen = this->GetHeader(UTF8STRC("Content-Length"));
	if (contentLen)
	{
		if (Text::StrToUInt64(contentLen->v) == fd->GetDataSize() - contentOfst)
		{
			IO::IStreamData *data = fd->GetPartialData(contentOfst, Text::StrToUInt64(contentLen->v));
			Text::String *contType = this->GetHeader(UTF8STRC("Content-Type"));
			Text::IMIMEObj *obj = Text::IMIMEObj::ParseFromData(data, STR_CSTR(contType));
			DEL_CLASS(data);
			if (obj)
			{
				this->SetContent(obj);
			}
		}
	}
	else
	{
		IO::IStreamData *data = fd->GetPartialData(contentOfst, fd->GetDataSize() - contentOfst);
		Text::String *contType = this->GetHeader(UTF8STRC("Content-Type"));
		Text::IMIMEObj *obj = Text::IMIMEObj::ParseFromData(data, STR_CSTR(contType));
		DEL_CLASS(data);
		if (obj)
		{
			this->SetContent(obj);
		}
	}
	return true;
}

UTF8Char *Text::MIMEObj::MIMEMessage::ParseHeaderStr(UTF8Char *sbuff, const UTF8Char *value)
{
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbc;
	UTF8Char c;
	Bool err;
	while (true)
	{
		c = *value++;
		if (c == '=')
		{
			if (*value == '?')
			{
				err = false;
				value++;
				sb.ClearStr();
				while (true)
				{
					c = *value++;
					if (c == '?')
					{
						if (*value == 'B' && value[1] == '?')
						{
							value += 2;
							break;
						}
						else if (*value == 'b' && value[1] == '?')
						{
							value += 2;
							break;
						}
						else
						{
							err = true;
							break;
						}
					}
					else if (c == 0)
					{
						err = true;
						break;
					}
					else
					{
						sb.AppendUTF8Char(c);
					}
				}
				if (!err)
				{
					sbc.ClearStr();
					while (true)
					{
						c = *value++;
						if (c == '?')
						{
							if (*value == '=')
							{
								value++;
								break;
							}
							else
							{
								err = true;
								break;
							}
						}
						else if (c == 0)
						{
							err = true;
							break;
						}
						else
						{
							sbc.AppendUTF8Char(c);
						}
					}
				}
				if (!err)
				{
					Text::EncodingFactory encFact;
					UInt32 cp = encFact.GetCodePage(sb.ToCString());
					if (cp == 0)
					{
						err = true;
					}
					else
					{
						Text::Encoding enc(cp);
						Crypto::Encrypt::Base64 b64;
						UOSInt buffSize = (sbc.GetLength() >> 2) * 3;
						UOSInt outSize;
						UInt8 *tmpBuff = MemAlloc(UInt8, buffSize);
						outSize = b64.Decrypt((const UInt8*)sbc.ToString(), sbc.GetLength(), tmpBuff, 0);
						sbuff = enc.UTF8FromBytes(sbuff, tmpBuff, outSize, 0);
						MemFree(tmpBuff);
					}
				}
			}
			else
			{
				*sbuff++ = c;
			}
		}
		else
		{
			if (c == 0)
			{
				*sbuff = 0;
				break;
			}
			*sbuff++ = c;
		}
	}
	return sbuff;
}

