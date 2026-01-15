#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Encrypt/Base64.h"
#include "Data/ByteBuffer.h"
#include "Text/Encoding.h"
#include "Text/EncodingFactory.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/MIMEObj/MIMEMessage.h"

#define BUFFSIZE 4096

Text::MIMEObj::MIMEMessage::MIMEMessage() : Text::MIMEObject(CSTR("message/rfc822"))
{
	this->content = nullptr;
	this->transferData = 0;
	this->transferSize = 0;
}

Text::MIMEObj::MIMEMessage::MIMEMessage(Optional<Text::MIMEObject> content) : Text::MIMEObject(CSTR("message/rfc822"))
{
	this->content = content;
	this->transferData = 0;
	this->transferSize = 0;
}

Text::MIMEObj::MIMEMessage::~MIMEMessage()
{
	UIntOS i;
	i = this->headerName.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(this->headerName.GetItem(i));
		OPTSTR_DEL(this->headerValue.GetItem(i));
	}
	this->content.Delete();
	if (this->transferData)
	{
		MemFree(this->transferData);
	}
}

Text::CStringNN Text::MIMEObj::MIMEMessage::GetClassName() const
{
	return CSTR("MIMEMessage");
}

Text::CStringNN Text::MIMEObj::MIMEMessage::GetContentType() const
{
	NN<Text::String> contType;
	if (!this->GetHeader(UTF8STRC("Content-Type")).SetTo(contType))
		return CSTR("application/octet-stream");
	else
		return contType->ToCString();
}

UIntOS Text::MIMEObj::MIMEMessage::WriteStream(NN<IO::Stream> stm) const
{
	UIntOS i;
	UIntOS j;
	NN<Text::String> s;
	NN<MIMEObject> content;
	Text::StringBuilderUTF8 sbc;
	i = 0;
	j = this->headerName.GetCount();
	while (i < j)
	{
		if (this->headerName.GetItem(i).SetTo(s))
			sbc.Append(s);
		sbc.AppendC(UTF8STRC(": "));
		if (this->headerValue.GetItem(i).SetTo(s))
			sbc.Append(s);
		sbc.AppendC(UTF8STRC("\r\n"));
		i++;
	}
	sbc.AppendC(UTF8STRC("\r\n"));
	stm->Write(sbc.ToByteArray());
	i = sbc.GetLength();
	if (this->transferData)
	{
		stm->Write(Data::ByteArrayR(this->transferData, this->transferSize));
		i += this->transferSize;
	}
	else if (this->content.SetTo(content))
	{
		i += content->WriteStream(stm);
	}
	return i;
}

NN<Text::MIMEObject> Text::MIMEObj::MIMEMessage::Clone() const
{
	NN<Text::MIMEObj::MIMEMessage> msg;
	NN<MIMEObject> content;
	UIntOS i;
	UIntOS j;
	NEW_CLASSNN(msg, Text::MIMEObj::MIMEMessage());
	i = 0;
	j = this->headerName.GetCount();
	while (i < j)
	{
		NN<Text::String> name;
		NN<Text::String> value;
		if (this->headerName.GetItem(i).SetTo(name) && this->headerValue.GetItem(i).SetTo(value))
		msg->AddHeader(name, value);
		i++;
	}
	if (this->content.SetTo(content))
	{
		msg->SetContent(content->Clone());
	}
	return msg;
}

void Text::MIMEObj::MIMEMessage::SetContent(Optional<Text::MIMEObject> content)
{
	this->content.Delete();
	this->content = content;
}

Optional<Text::MIMEObject> Text::MIMEObj::MIMEMessage::GetContent() const
{
	return this->content;
}

void Text::MIMEObj::MIMEMessage::SetTransferData(const UInt8 *data, UIntOS dataSize)
{
	if (this->transferData)
	{
		MemFree(this->transferData);
	}
	this->transferSize = dataSize;
	this->transferData = MemAlloc(UInt8, dataSize);
	MemCopyNO(this->transferData, data, dataSize);
}

void Text::MIMEObj::MIMEMessage::AddHeader(Text::CStringNN name, Text::CStringNN value)
{
	this->headerName.Add(Text::String::New(name));
	this->headerValue.Add(Text::String::New(value));
}

void Text::MIMEObj::MIMEMessage::AddHeader(NN<Text::String> name, NN<Text::String> value)
{
	this->headerName.Add(name->Clone());
	this->headerValue.Add(value->Clone());
}

Optional<Text::String> Text::MIMEObj::MIMEMessage::GetHeader(UnsafeArray<const UTF8Char> name, UIntOS nameLen) const
{
	UIntOS i;
	UIntOS j;
	NN<Text::String> s;
	i = 0;
	j = this->headerName.GetCount();
	while (i < j)
	{
		if (this->headerName.GetItem(i).SetTo(s) && s->EqualsICase(name, nameLen))
			return this->headerValue.GetItem(i);
		i++;
	}
	return nullptr;
}

UIntOS Text::MIMEObj::MIMEMessage::GetHeaderCount() const
{
	return this->headerName.GetCount();
}

Optional<Text::String> Text::MIMEObj::MIMEMessage::GetHeaderName(UIntOS index) const
{
	return this->headerName.GetItem(index);
}

Optional<Text::String> Text::MIMEObj::MIMEMessage::GetHeaderValue(UIntOS index) const
{
	return this->headerValue.GetItem(index);
}

Bool Text::MIMEObj::MIMEMessage::ParseFromData(NN<IO::StreamData> fd)
{
	UIntOS buffSize;
	UIntOS readSize;
	UInt64 fileOfst;
	Text::StringBuilderUTF8 sb;
	Text::PString sarr[2];
	Bool isFirst = true;
	Bool found = false;
	UIntOS i = 0;
	UIntOS lineStart;

	Data::ByteBuffer buff(BUFFSIZE + 1);
	fileOfst = 0;
	buffSize = 0;
	while (true)
	{
		readSize = fd->GetRealData(fileOfst, BUFFSIZE - buffSize, buff.SubArray(buffSize));
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
						this->AddHeader(sarr[0].ToCString(), sarr[1].ToCString());
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
						this->AddHeader(sarr[0].ToCString(), sarr[1].ToCString());
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
			buff.CopyInner(0, lineStart, buffSize - lineStart);
			buffSize -= lineStart;
		}
		else
		{
			buffSize = 0;
		}
	}

	if (!found)
	{
		return false;
	}

	UInt64 contentOfst = fileOfst - buffSize + lineStart;
	NN<Text::String> s;
	if (this->GetHeader(UTF8STRC("Content-Length")).SetTo(s))
	{
		if (Text::StrToUInt64(s->v) == fd->GetDataSize() - contentOfst)
		{
			NN<IO::StreamData> data = fd->GetPartialData(contentOfst, fd->GetDataSize() - contentOfst);
			Optional<Text::String> contType = this->GetHeader(UTF8STRC("Content-Type"));
			NN<Text::MIMEObject> obj;
			if (Text::MIMEObject::ParseFromData(data, contType.SetTo(s)?s->ToCString():CSTR("application/octet-stream")).SetTo(obj))
			{
				this->SetContent(obj);
			}
			data.Delete();
		}
	}
	else
	{
		NN<IO::StreamData> data = fd->GetPartialData(contentOfst, fd->GetDataSize() - contentOfst);
		Optional<Text::String> contType = this->GetHeader(UTF8STRC("Content-Type"));
		NN<Text::MIMEObject> obj;
		if (Text::MIMEObject::ParseFromData(data, contType.SetTo(s)?s->ToCString():CSTR("application/octet-stream")).SetTo(obj))
		{
			this->SetContent(obj);
		}
		data.Delete();
	}
	return true;
}

UnsafeArray<UTF8Char> Text::MIMEObj::MIMEMessage::ParseHeaderStr(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UTF8Char> value)
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
						UIntOS buffSize = (sbc.GetLength() >> 2) * 3;
						UIntOS outSize;
						UInt8 *tmpBuff = MemAlloc(UInt8, buffSize);
						outSize = b64.Decrypt((const UInt8*)sbc.ToPtr(), sbc.GetLength(), tmpBuff);
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

