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
	NN<Text::String> hdrName;
	NN<Text::String> hdrValue;
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
		Optional<Text::IMIMEObj> obj = 0;
		NN<Text::IMIMEObj> nnobj;
		Text::String *tenc = hdrMap.GetC(CSTR("Content-Transfer-Encoding"));
		if (tenc)
		{
			if (tenc->Equals(UTF8STRC("base64")))
			{
				Text::TextBinEnc::Base64Enc b64;
				UInt8 *tmpBuff = MemAlloc(UInt8, buffSize - lineStart);
				j = b64.DecodeBin(Text::CStringNN(&buff[lineStart], buffSize - lineStart), tmpBuff);

				IO::StmData::MemoryDataRef mdata(tmpBuff, j);
				obj = Text::IMIMEObj::ParseFromData(mdata, contType->ToCString());
				MemFree(tmpBuff);
			}
			else if (tenc->Equals(UTF8STRC("quoted-printable")))
			{
				Text::TextBinEnc::QuotedPrintableEnc qpenc;
				UInt8 *tmpBuff = MemAlloc(UInt8, buffSize - lineStart);
				j = qpenc.DecodeBin(Text::CStringNN(&buff[lineStart], buffSize - lineStart), tmpBuff);

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

		if (obj.SetTo(nnobj))
		{
			i = this->AddPart(nnobj);
			this->SetPartTransferData(i, &buff[lineStart], buffSize - lineStart);
			j = 0;
			k = hdrNames.GetCount();
			while (j < k)
			{
				NN<Text::String> name;
				NN<Text::String> value;
				if (hdrNames.GetItem(j).SetTo(name) && hdrValues.GetItem(j).SetTo(value))
					this->AddPartHeader(i, name->ToCString(), value->ToCString());
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

Text::MIMEObj::MultipartMIMEObj::MultipartMIMEObj(NN<Text::String> contentType, Optional<Text::String> defMsg, NN<Text::String> boundary) : Text::IMIMEObj(CSTR("multipart/mixed"))
{
	this->contentType = contentType->Clone();
	this->defMsg = Text::String::CopyOrNull(defMsg);
	this->boundary = boundary->Clone();
}

Text::MIMEObj::MultipartMIMEObj::MultipartMIMEObj(Text::CStringNN contentType, Text::CString defMsg, Text::CStringNN boundary) : Text::IMIMEObj(CSTR("multipart/mixed"))
{
	this->contentType = Text::String::New(contentType);
	this->defMsg = Text::String::NewOrNull(defMsg);
	this->boundary = Text::String::New(boundary);
}

Text::MIMEObj::MultipartMIMEObj::MultipartMIMEObj(Text::CStringNN contentType, Text::CString defMsg) : Text::IMIMEObj(CSTR("multipart/mixed"))
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
	this->defMsg = Text::String::NewOrNull(defMsg);
}

Text::MIMEObj::MultipartMIMEObj::~MultipartMIMEObj()
{
	this->contentType->Release();
	OPTSTR_DEL(this->defMsg);
	this->boundary->Release();
	this->parts.DeleteAll();
}

Text::CStringNN Text::MIMEObj::MultipartMIMEObj::GetClassName() const
{
	return CSTR("MultipartMIMEObj");
}

Text::CStringNN Text::MIMEObj::MultipartMIMEObj::GetContentType() const
{
	return this->contentType->ToCString();
}

UOSInt Text::MIMEObj::MultipartMIMEObj::WriteStream(NN<IO::Stream> stm) const
{
	UOSInt ret = 0;
	UOSInt len;
	UOSInt i;
	UOSInt j;
	NN<MIMEMessage> part;
	Text::StringBuilderUTF8 sbc;
	NN<Text::String> defMsg;
	if (this->defMsg.SetTo(defMsg))
	{
		len = defMsg->leng;
		stm->Write(defMsg->ToByteArray());
		ret += len;
	}
	i = 0;
	j = this->parts.GetCount();
	while (i < j)
	{
		part = this->parts.GetItemNoCheck(i);
		sbc.ClearStr();
		sbc.AppendC(UTF8STRC("\r\n--"));
		sbc.Append(this->boundary);
		sbc.AppendC(UTF8STRC("\r\n"));
		stm->Write(sbc.ToByteArray());
		ret += sbc.GetLength();
		ret += part->WriteStream(stm);

		i++;
	}
	sbc.ClearStr();
	sbc.AppendC(UTF8STRC("\r\n--"));
	sbc.Append(this->boundary);
	sbc.AppendC(UTF8STRC("--"));
	stm->Write(sbc.ToByteArray());
	ret += sbc.GetLength();
	return ret;
}

NN<Text::IMIMEObj> Text::MIMEObj::MultipartMIMEObj::Clone() const
{
	NN<MIMEMessage> part;
	NN<Text::MIMEObj::MultipartMIMEObj> obj;
	UOSInt i;
	UOSInt j;
	NEW_CLASSNN(obj, Text::MIMEObj::MultipartMIMEObj(this->contentType, this->defMsg, this->boundary));
	i = 0;
	j = this->parts.GetCount();
	while (i < j)
	{
		part = this->parts.GetItemNoCheck(i);
		obj->parts.Add(NN<MIMEMessage>::ConvertFrom(part->Clone()));
		i++;
	}
	return obj;
}

Optional<Text::String> Text::MIMEObj::MultipartMIMEObj::GetDefMsg() const
{
	return this->defMsg;
}

UOSInt Text::MIMEObj::MultipartMIMEObj::AddPart(NN<Text::IMIMEObj> obj)
{
	NN<MIMEMessage> part;
	NEW_CLASSNN(part, MIMEMessage(obj));
	return this->parts.Add(part);
}

void Text::MIMEObj::MultipartMIMEObj::SetPartTransferData(UOSInt partIndex, const UInt8 *data, UOSInt dataSize)
{
	NN<MIMEMessage> part;
	if (!this->parts.GetItem(partIndex).SetTo(part))
		return;
	part->SetTransferData(data, dataSize);
}

Bool Text::MIMEObj::MultipartMIMEObj::AddPartHeader(UOSInt partIndex, Text::CStringNN name, Text::CStringNN value)
{
	NN<MIMEMessage> part;
	if (!this->parts.GetItem(partIndex).SetTo(part))
		return false;
	part->AddHeader(name, value);
	return true;
}

Optional<Text::IMIMEObj> Text::MIMEObj::MultipartMIMEObj::GetPartContent(UOSInt partIndex) const
{
	NN<MIMEMessage> part;
	if (!this->parts.GetItem(partIndex).SetTo(part))
		return 0;
	return part->GetContent();
}

Optional<Text::MIMEObj::MIMEMessage> Text::MIMEObj::MultipartMIMEObj::GetPart(UOSInt partIndex) const
{
	return this->parts.GetItem(partIndex);
}

UOSInt Text::MIMEObj::MultipartMIMEObj::GetPartCount() const
{
	return this->parts.GetCount();
}

Optional<Text::MIMEObj::MultipartMIMEObj> Text::MIMEObj::MultipartMIMEObj::ParseFile(Text::CStringNN contentType, NN<IO::StreamData> data)
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

	Text::CStringNN currPart = contentType;
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
