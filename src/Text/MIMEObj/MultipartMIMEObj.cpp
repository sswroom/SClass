#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListString.h"
#include "Data/ArrayListStringNN.h"
#include "Data/FastStringMap.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Text/MailBase64Stream.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/MIMEObj/MultipartMIMEObj.h"
#include "Text/TextBinEnc/Base64Enc.h"
#include "Text/TextBinEnc/QuotedPrintableEnc.h"
#include <stdio.h>

void Text::MIMEObj::MultipartMIMEObj::ParsePart(UInt8 *buff, UOSInt buffSize)
{
	UOSInt lineStart;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Data::ArrayListStringNN hdrNames;
	Data::ArrayListStringNN hdrValues;
	Data::FastStringMap<Text::String*> hdrMap;
	Text::StringBuilderUTF8 sb;
	Text::PString sarr[2];
	NotNullPtr<Text::String> hdrName;
	NotNullPtr<Text::String> hdrValue;
	Bool found = false;
	lineStart = 0;
	i = 0;
	while (i < buffSize - 1)
	{
		if (buff[i] == '\r' && buff[i + 1] == '\n')
		{
			if (i == lineStart)
			{
				found = true;
				lineStart = i + 2;
				break;
			}
			else
			{
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
					hdrName = Text::String::New(sarr[0].v, sarr[0].leng);
					hdrValue = Text::String::New(sarr[1].v, sarr[1].leng);
					hdrNames.Add(hdrName);
					hdrValues.Add(hdrValue);
					hdrMap.PutNN(hdrName, hdrValue.Ptr());
					sb.ClearStr();
				}
			}
			i += 2;
			lineStart = i;
		}
		else
		{
			i++;
		}
	}
	if (!found)
	{
		i = hdrNames.GetCount();
		while (i-- > 0)
		{
			OPTSTR_DEL(hdrNames.GetItem(i));
			OPTSTR_DEL(hdrValues.GetItem(i));
		}
		return;
	}

	Text::String *contType = hdrMap.GetC(CSTR("Content-Type"));
	if (contType)
	{
		Text::IMIMEObj *obj = 0;
		Text::String *tenc = hdrMap.GetC(CSTR("Content-Transfer-Encoding"));
		if (tenc)
		{
			if (tenc->Equals(UTF8STRC("base64")))
			{
				Text::TextBinEnc::Base64Enc b64;
				UInt8 *tmpBuff = MemAlloc(UInt8, buffSize - lineStart);
				j = b64.DecodeBin(&buff[lineStart], buffSize - lineStart, tmpBuff);

				IO::StmData::MemoryDataRef mdata(tmpBuff, j);
				obj = Text::IMIMEObj::ParseFromData(mdata, contType->ToCString());
				MemFree(tmpBuff);
			}
			else if (tenc->Equals(UTF8STRC("quoted-printable")))
			{
				Text::TextBinEnc::QuotedPrintableEnc qpenc;
				UInt8 *tmpBuff = MemAlloc(UInt8, buffSize - lineStart);
				j = qpenc.DecodeBin(&buff[lineStart], buffSize - lineStart, tmpBuff);

				IO::StmData::MemoryDataRef mdata(tmpBuff, j);
				obj = Text::IMIMEObj::ParseFromData(mdata, contType->ToCString());
				MemFree(tmpBuff);
			}
			else if (tenc->Equals(UTF8STRC("7bit")))
			{
				IO::StmData::MemoryDataRef mdata(&buff[lineStart], buffSize - lineStart);
				obj = Text::IMIMEObj::ParseFromData(mdata, contType->ToCString());
			}
			else
			{
				//////////////////////////////////////
				printf("MultipartMIMEObj: Unknown Content-Transfer-Encoding\r\n");
			}
		}
		else
		{
			IO::StmData::MemoryDataRef mdata(&buff[lineStart], buffSize - lineStart);
			obj = Text::IMIMEObj::ParseFromData(mdata, contType->ToCString());
		}

		if (obj)
		{
			i = this->AddPart(obj);
			this->SetPartTransferData(i, &buff[lineStart], buffSize - lineStart);
			j = 0;
			k = hdrNames.GetCount();
			while (j < k)
			{
				NotNullPtr<Text::String> name;
				NotNullPtr<Text::String> value;
				if (hdrNames.GetItem(j).SetTo(name) && hdrValues.GetItem(j).SetTo(value))
					this->AddPartHeader(i, name->v, name->leng, value->v, value->leng);
				j++;
			}
		}
	}

	i = hdrNames.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(hdrNames.GetItem(i));
		OPTSTR_DEL(hdrValues.GetItem(i));
	}
}

Text::MIMEObj::MultipartMIMEObj::MultipartMIMEObj(NotNullPtr<Text::String> contentType, Text::String *defMsg, NotNullPtr<Text::String> boundary) : Text::IMIMEObj(CSTR("multipart/mixed"))
{
	this->contentType = contentType->Clone();
	if (defMsg)
	{
		this->defMsg = defMsg->Clone().Ptr();
	}
	else
	{
		this->defMsg = 0;
	}
	this->boundary = boundary->Clone();
}

Text::MIMEObj::MultipartMIMEObj::MultipartMIMEObj(Text::CString contentType, Text::CString defMsg, Text::CString boundary) : Text::IMIMEObj(CSTR("multipart/mixed"))
{
	this->contentType = Text::String::New(contentType);
	if (defMsg.leng > 0)
	{
		this->defMsg = Text::String::New(defMsg).Ptr();
	}
	else
	{
		this->defMsg = 0;
	}
	this->boundary = Text::String::New(boundary);
}

Text::MIMEObj::MultipartMIMEObj::MultipartMIMEObj(Text::CString contentType, Text::CString defMsg) : Text::IMIMEObj(CSTR("multipart/mixed"))
{
	Text::StringBuilderUTF8 sbc;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	sbc.AppendC(UTF8STRC("----------"));
	sbc.AppendI64(dt.ToTicks());
	sbc.AppendOSInt((0x7fffffff & (OSInt)this));
	this->boundary = Text::String::New(sbc.ToCString());
	sbc.ClearStr();
	sbc.Append(contentType);
	sbc.AppendC(UTF8STRC(";\r\n\tboundary=\""));
	sbc.Append(this->boundary);
	sbc.AppendC(UTF8STRC("\""));
	this->contentType = Text::String::New(sbc.ToCString());
	if (defMsg.leng > 0)
	{
		this->defMsg = Text::String::New(defMsg).Ptr();
	}
	else
	{
		this->defMsg = 0;
	}
}

Text::MIMEObj::MultipartMIMEObj::~MultipartMIMEObj()
{
	UOSInt i;
	MIMEMessage *part;
	this->contentType->Release();
	SDEL_STRING(this->defMsg);
	this->boundary->Release();
	i = this->parts.GetCount();
	while (i-- > 0)
	{
		part = this->parts.GetItem(i);
		DEL_CLASS(part);
	}
}

Text::CStringNN Text::MIMEObj::MultipartMIMEObj::GetClassName() const
{
	return CSTR("MultipartMIMEObj");
}

Text::CStringNN Text::MIMEObj::MultipartMIMEObj::GetContentType() const
{
	return this->contentType->ToCString();
}

UOSInt Text::MIMEObj::MultipartMIMEObj::WriteStream(IO::Stream *stm) const
{
	UOSInt ret = 0;
	UOSInt len;
	UOSInt i;
	UOSInt j;
	MIMEMessage *part;
	Text::StringBuilderUTF8 sbc;
	if (this->defMsg)
	{
		len = this->defMsg->leng;
		stm->Write(this->defMsg->v, len);
		ret += len;
	}
	i = 0;
	j = this->parts.GetCount();
	while (i < j)
	{
		part = this->parts.GetItem(i);
		sbc.ClearStr();
		sbc.AppendC(UTF8STRC("\r\n--"));
		sbc.Append(this->boundary);
		sbc.AppendC(UTF8STRC("\r\n"));
		stm->Write(sbc.ToString(), sbc.GetLength());
		ret += sbc.GetLength();
		ret += part->WriteStream(stm);

		i++;
	}
	sbc.ClearStr();
	sbc.AppendC(UTF8STRC("\r\n--"));
	sbc.Append(this->boundary);
	sbc.AppendC(UTF8STRC("--"));
	stm->Write((const UInt8*)sbc.ToString(), sbc.GetLength());
	ret += sbc.GetLength();
	return ret;
}

Text::IMIMEObj *Text::MIMEObj::MultipartMIMEObj::Clone() const
{
	MIMEMessage *part;
	Text::MIMEObj::MultipartMIMEObj *obj;
	UOSInt i;
	UOSInt j;
	NEW_CLASS(obj, Text::MIMEObj::MultipartMIMEObj(this->contentType, this->defMsg, this->boundary));
	i = 0;
	j = this->parts.GetCount();
	while (i < j)
	{
		part = this->parts.GetItem(i);
		obj->parts.Add((MIMEMessage*)part->Clone());
		i++;
	}
	return obj;
}

Text::String *Text::MIMEObj::MultipartMIMEObj::GetDefMsg() const
{
	return this->defMsg;
}

UOSInt Text::MIMEObj::MultipartMIMEObj::AddPart(Text::IMIMEObj *obj)
{
	MIMEMessage *part;
	NEW_CLASS(part, MIMEMessage(obj));
	return this->parts.Add(part);
}

void Text::MIMEObj::MultipartMIMEObj::SetPartTransferData(UOSInt partIndex, const UInt8 *data, UOSInt dataSize)
{
	MIMEMessage *part = this->parts.GetItem(partIndex);
	if (part == 0)
		return;
	part->SetTransferData(data, dataSize);
}

Bool Text::MIMEObj::MultipartMIMEObj::AddPartHeader(UOSInt partIndex, const UTF8Char *name, UOSInt nameLen, const UTF8Char *value, UOSInt valueLen)
{
	MIMEMessage *part = this->parts.GetItem(partIndex);
	if (part == 0)
		return false;
	part->AddHeader(name, nameLen, value, valueLen);
	return true;
}

Text::IMIMEObj *Text::MIMEObj::MultipartMIMEObj::GetPartContent(UOSInt partIndex) const
{
	MIMEMessage *part = this->parts.GetItem(partIndex);
	if (part == 0)
		return 0;
	return part->GetContent();
}

Text::MIMEObj::MIMEMessage *Text::MIMEObj::MultipartMIMEObj::GetPart(UOSInt partIndex) const
{
	return this->parts.GetItem(partIndex);
}

UOSInt Text::MIMEObj::MultipartMIMEObj::GetPartCount() const
{
	return this->parts.GetCount();
}

Text::MIMEObj::MultipartMIMEObj *Text::MIMEObj::MultipartMIMEObj::ParseFile(Text::CString contentType, NotNullPtr<IO::StreamData> data)
{
	UOSInt j;
	if (contentType.StartsWith(UTF8STRC("multipart/mixed;")))
	{
		j = 16;
	}
	else if (contentType.StartsWith(UTF8STRC("multipart/related;")))
	{
		j = 18;
	}
	else if (contentType.StartsWith(UTF8STRC("multipart/alternative;")))
	{
		j = 22;
	}
	else if (contentType.StartsWith(UTF8STRC("multipart/signed;")))
	{
		j = 17;
	}
	else
	{
		return 0;
	}

	Text::CString currPart = contentType;
	while (true)
	{
		UOSInt i;
		while (currPart.v[j] == '\r' || currPart.v[j] == '\n' || currPart.v[j] == '\t' || currPart.v[j] == ' ')
		{
			j++;
		}
		currPart = currPart.Substring(j);
		if (currPart.StartsWith(UTF8STRC("boundary=")))
		{
			Text::StringBuilderUTF8 boundary;
			Text::MIMEObj::MultipartMIMEObj *obj;
			UOSInt buffSize;
			UInt8 *buff;
			i = currPart.leng;
			j = currPart.IndexOf(';');
			if (j != INVALID_INDEX)
			{
				i = j;
			}

			boundary.AppendC(UTF8STRC("--"));
			if (currPart.v[9] == '"' && currPart.v[i - 1] == '"')
			{
				boundary.AppendC(&currPart.v[10], i - 11);
			}
			else
			{
				boundary.AppendC(&currPart.v[9], i - 9);
			}

			buffSize = (UOSInt)data->GetDataSize();
			buff = MemAlloc(UInt8, buffSize + 1);
			data->GetRealData(0, buffSize, Data::ByteArray(buff, buffSize + 1));
			buff[buffSize] = 0;

			j = Text::StrIndexOfC(buff, buffSize, boundary.ToString(), boundary.GetLength());
			if (j == INVALID_INDEX)
			{
				NEW_CLASS(obj, Text::MIMEObj::MultipartMIMEObj(contentType, CSTR_NULL, boundary.ToCString()));
				i = 0;
			}
			else
			{
				buff[j] = 0;
				NEW_CLASS(obj, Text::MIMEObj::MultipartMIMEObj(contentType, {buff, j}, boundary.ToCString()));
				i = j + boundary.GetLength();
				if (buff[i] == '\r' && buff[i + 1] == '\n')
				{
					i += 2;
				}
			}
			while (true)
			{
				j = Text::StrIndexOfC(&buff[i], buffSize - i, boundary.ToString(), boundary.GetLength());
				if (j == INVALID_INDEX)
					break;
				j += i;

				obj->ParsePart(&buff[i], j - i);
				i = j + boundary.GetLength();
				if (buff[i] == '\r' && buff[i + 1] == '\n')
				{
					i += 2;
				}
			}
			obj->ParsePart(&buff[i], buffSize - i);
			MemFree(buff);
			return obj;
		}	
		else
		{
			j = currPart.IndexOf(';');
			if (j == INVALID_INDEX)
				return 0;
			j++;
		}
	}
}
