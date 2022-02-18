#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListString.h"
#include "IO/StmData/MemoryData.h"
#include "Text/MailBase64Stream.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/MIMEObj/MultipartMIMEObj.h"
#include "Text/TextBinEnc/Base64Enc.h"
#include "Text/TextBinEnc/QuotedPrintableEnc.h"
#include <stdio.h>

Text::MIMEObj::MultipartMIMEObj::PartInfo::PartInfo(Text::IMIMEObj *obj)
{
	this->obj = obj;
}

Text::MIMEObj::MultipartMIMEObj::PartInfo::~PartInfo()
{
	DEL_CLASS(this->obj);
}

Text::IMIMEObj *Text::MIMEObj::MultipartMIMEObj::PartInfo::GetObject()
{
	return this->obj;
}

Text::MIMEObj::MultipartMIMEObj::PartInfo *Text::MIMEObj::MultipartMIMEObj::PartInfo::Clone()
{
	Text::MIMEObj::MultipartMIMEObj::PartInfo *obj;
	UOSInt i;
	UOSInt j;
	NEW_CLASS(obj, Text::MIMEObj::MultipartMIMEObj::PartInfo(this->obj->Clone()));
	i = 0;
	j = this->headerName->GetCount();
	while (i < j)
	{
		obj->AddHeader(this->headerName->GetItem(i), this->headerValue->GetItem(i));
		i++;
	}
	return obj;
}

void Text::MIMEObj::MultipartMIMEObj::ParsePart(UInt8 *buff, UOSInt buffSize)
{
	UOSInt lineStart;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	OSInt si;
	Data::ArrayListString hdrNames;
	Data::ArrayListString hdrValues;
	Text::StringBuilderUTF8 sb;
	Text::PString sarr[2];
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
					j = hdrNames.SortedInsert(Text::String::New(sarr[0].v, sarr[0].leng));
					hdrValues.Insert(j, Text::String::New(sarr[1].v, sarr[1].leng));
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
			hdrNames.GetItem(i)->Release();
			hdrValues.GetItem(i)->Release();
		}
		return;
	}

	si = hdrNames.SortedIndexOfPtr(UTF8STRC("Content-Type"));
	if (si >= 0)
	{
		Text::String *contType = hdrValues.GetItem((UOSInt)si);
		Text::IMIMEObj *obj = 0;
		IO::StmData::MemoryData *mdata;
		si = hdrNames.SortedIndexOfPtr(UTF8STRC("Content-Transfer-Encoding"));
		if (si >= 0)
		{
			Text::String *tenc = hdrValues.GetItem((UOSInt)si);
			if (tenc->Equals(UTF8STRC("base64")))
			{
				Text::TextBinEnc::Base64Enc b64;
				UInt8 *tmpBuff = MemAlloc(UInt8, buffSize - lineStart);
				j = b64.DecodeBin(&buff[lineStart], buffSize - lineStart, tmpBuff);

				NEW_CLASS(mdata, IO::StmData::MemoryData(tmpBuff, j));
				obj = Text::IMIMEObj::ParseFromData(mdata, contType->ToCString());
				DEL_CLASS(mdata);

				MemFree(tmpBuff);
			}
			else if (tenc->Equals(UTF8STRC("quoted-printable")))
			{
				Text::TextBinEnc::QuotedPrintableEnc qpenc;
				UInt8 *tmpBuff = MemAlloc(UInt8, buffSize - lineStart);
				j = qpenc.DecodeBin(&buff[lineStart], buffSize - lineStart, tmpBuff);

				NEW_CLASS(mdata, IO::StmData::MemoryData(tmpBuff, j));
				obj = Text::IMIMEObj::ParseFromData(mdata, contType->ToCString());
				DEL_CLASS(mdata);

				MemFree(tmpBuff);
			}
			else
			{
				//////////////////////////////////////
				printf("MultipartMIMEObj: Unknown Content-Transfer-Encoding\r\n");
			}
		}
		else
		{
			NEW_CLASS(mdata, IO::StmData::MemoryData(&buff[lineStart], buffSize - lineStart));
			obj = Text::IMIMEObj::ParseFromData(mdata, contType->ToCString());
			DEL_CLASS(mdata);
		}

		if (obj)
		{
			i = this->AddPart(obj);
			j = 0;
			k = hdrNames.GetCount();
			while (j < k)
			{
				Text::String *name = hdrNames.GetItem(j);
				Text::String *value = hdrValues.GetItem(j);
				this->AddPartHeader(i, name->v, name->leng, value->v, value->leng);
				j++;
			}
		}
	}

	i = hdrNames.GetCount();
	while (i-- > 0)
	{
		hdrNames.GetItem(i)->Release();
		hdrValues.GetItem(i)->Release();
	}
}

Text::MIMEObj::MultipartMIMEObj::MultipartMIMEObj(Text::String *contentType, Text::String *defMsg, Text::String *boundary) : Text::IMIMEObj(CSTR("multipart/mixed"))
{
	this->contentType = contentType->Clone();
	if (defMsg)
	{
		this->defMsg = defMsg->Clone();
	}
	else
	{
		this->defMsg = 0;
	}
	this->boundary = boundary->Clone();
	NEW_CLASS(parts, Data::ArrayList<PartInfo*>());
}

Text::MIMEObj::MultipartMIMEObj::MultipartMIMEObj(Text::CString contentType, Text::CString defMsg, Text::CString boundary) : Text::IMIMEObj(CSTR("multipart/mixed"))
{
	this->contentType = Text::String::New(contentType);
	if (defMsg.leng > 0)
	{
		this->defMsg = Text::String::New(defMsg);
	}
	else
	{
		this->defMsg = 0;
	}
	this->boundary = Text::String::New(boundary);
	NEW_CLASS(parts, Data::ArrayList<PartInfo*>());
}

Text::MIMEObj::MultipartMIMEObj::MultipartMIMEObj(const UTF8Char *contentType, const UTF8Char *defMsg) : Text::IMIMEObj(CSTR("multipart/mixed"))
{
	Text::StringBuilderUTF8 sbc;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	sbc.AppendC(UTF8STRC("----------"));
	sbc.AppendI64(dt.ToTicks());
	sbc.AppendOSInt((0x7fffffff & (OSInt)this));
	this->boundary = Text::String::New(sbc.ToString(), sbc.GetLength());
	sbc.ClearStr();
	sbc.AppendSlow(contentType);
	sbc.AppendC(UTF8STRC(";\r\n\tboundary=\""));
	sbc.Append(this->boundary);
	sbc.AppendC(UTF8STRC("\""));
	this->contentType = Text::String::New(sbc.ToString(), sbc.GetLength());
	if (defMsg)
	{
		this->defMsg = Text::String::NewNotNull(defMsg);
	}
	else
	{
		this->defMsg = 0;
	}
	NEW_CLASS(this->parts, Data::ArrayList<PartInfo*>());
}

Text::MIMEObj::MultipartMIMEObj::~MultipartMIMEObj()
{
	UOSInt i;
	PartInfo *part;
	this->contentType->Release();
	SDEL_STRING(this->defMsg);
	this->boundary->Release();
	i = this->parts->GetCount();
	while (i-- > 0)
	{
		part = this->parts->GetItem(i);
		DEL_CLASS(part);
	}
	DEL_CLASS(this->parts);
}

Text::CString Text::MIMEObj::MultipartMIMEObj::GetClassName()
{
	return {UTF8STRC("MultipartMIMEObj")};
}

Text::CString Text::MIMEObj::MultipartMIMEObj::GetContentType()
{
	return {this->contentType->v, this->contentType->leng};
}

UOSInt Text::MIMEObj::MultipartMIMEObj::WriteStream(IO::Stream *stm)
{
	UOSInt ret = 0;
	UOSInt len;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Int32 encType;
	PartInfo *part;
	Text::String *hdrName;
	Text::String *hdrValue;
	Text::StringBuilderUTF8 sbc;
	if (this->defMsg)
	{
		len = this->defMsg->leng;
		stm->Write(this->defMsg->v, len);
		ret += len;
	}
	i = 0;
	j = this->parts->GetCount();
	while (i < j)
	{
		part = this->parts->GetItem(i);
		encType = 0;
		sbc.ClearStr();
		sbc.AppendC(UTF8STRC("\r\n--"));
		sbc.Append(this->boundary);
		sbc.AppendC(UTF8STRC("\r\n"));
		stm->Write(sbc.ToString(), sbc.GetLength());
		ret += sbc.GetLength();

		k = 0;
		l = part->GetHeaderCount();
		while (k < l)
		{
			hdrName = part->GetHeaderName(k);
			hdrValue = part->GetHeaderValue(k);
			sbc.ClearStr();
			sbc.Append(hdrName);
			sbc.AppendC(UTF8STRC(": "));
			sbc.Append(hdrValue);
			sbc.AppendC(UTF8STRC("\r\n"));
			stm->Write((const UInt8*)sbc.ToString(), sbc.GetLength());
			ret += sbc.GetLength();
			if (hdrName->Equals(UTF8STRC("Content-Transfer-Encoding")))
			{
				if (hdrValue->Equals(UTF8STRC("base64")))
				{
					encType = 1;
				}
			}
			k++;
		}
		stm->Write((UInt8*)"\r\n", 2);
		ret += 2;

		if (encType == 1)
		{
			Text::MailBase64Stream *b64Stm;
			NEW_CLASS(b64Stm, Text::MailBase64Stream(stm));
			part->GetObject()->WriteStream(b64Stm);
			ret += b64Stm->GetWriteCount();
			DEL_CLASS(b64Stm);
		}
		else
		{
			ret += part->GetObject()->WriteStream(stm);
		}

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

Text::IMIMEObj *Text::MIMEObj::MultipartMIMEObj::Clone()
{
	PartInfo *part;
	Text::MIMEObj::MultipartMIMEObj *obj;
	UOSInt i;
	UOSInt j;
	NEW_CLASS(obj, Text::MIMEObj::MultipartMIMEObj(this->contentType, this->defMsg, this->boundary));
	i = 0;
	j = this->parts->GetCount();
	while (i < j)
	{
		part = this->parts->GetItem(i);
		obj->parts->Add(part->Clone());
		i++;
	}
	return obj;
}

Text::String *Text::MIMEObj::MultipartMIMEObj::GetDefMsg()
{
	return this->defMsg;
}

UOSInt Text::MIMEObj::MultipartMIMEObj::AddPart(Text::IMIMEObj *obj)
{
	PartInfo *part;
	NEW_CLASS(part, PartInfo(obj));
	return this->parts->Add(part);
}

Bool Text::MIMEObj::MultipartMIMEObj::AddPartHeader(UOSInt partIndex, const UTF8Char *name, UOSInt nameLen, const UTF8Char *value, UOSInt valueLen)
{
	PartInfo *part = this->parts->GetItem(partIndex);
	if (part == 0)
		return false;
	part->AddHeader(name, nameLen, value, valueLen);
	return true;
}

Text::IMIMEObj *Text::MIMEObj::MultipartMIMEObj::GetPartObj(UOSInt partIndex)
{
	PartInfo *part = this->parts->GetItem(partIndex);
	if (part == 0)
		return 0;
	return part->GetObject();
}

Text::MIMEObj::MultipartMIMEObj::PartInfo *Text::MIMEObj::MultipartMIMEObj::GetPart(UOSInt partIndex)
{
	return this->parts->GetItem(partIndex);
}

UOSInt Text::MIMEObj::MultipartMIMEObj::GetPartCount()
{
	return this->parts->GetCount();
}

Text::MIMEObj::MultipartMIMEObj *Text::MIMEObj::MultipartMIMEObj::ParseFile(Text::CString contentType, IO::IStreamData *data)
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
	else
	{
		return 0;
	}

	UOSInt i = contentType.leng;
	UOSInt k;
	while (contentType.v[j] == '\r' || contentType.v[j] == '\n' || contentType.v[j] == '\t' || contentType.v[j] == ' ')
	{
		j++;
	}
	if (Text::StrStartsWithC(&contentType.v[j], contentType.leng - j, UTF8STRC("boundary=")))
	{
		Text::StringBuilderUTF8 boundary;
		Text::MIMEObj::MultipartMIMEObj *obj;
		UOSInt buffSize;
		UInt8 *buff;
		k = Text::StrIndexOfCharC(&contentType.v[j], contentType.leng - j, ';');
		if (k != INVALID_INDEX)
		{
			i = j + k;
		}

		boundary.AppendC(UTF8STRC("--"));
		if (contentType.v[j + 9] == '"' && contentType.v[i - 1] == '"')
		{
			boundary.AppendC(&contentType.v[j + 10], i - j - 11);
		}
		else
		{
			boundary.AppendC(&contentType.v[j + 9], i - j - 9);
		}

		buffSize = (UOSInt)data->GetDataSize();
		buff = MemAlloc(UInt8, buffSize + 1);
		data->GetRealData(0, buffSize, buff);
		buff[buffSize] = 0;

		k = Text::StrIndexOfC(buff, buffSize, boundary.ToString(), boundary.GetLength());
		if (k == INVALID_INDEX)
		{
			NEW_CLASS(obj, Text::MIMEObj::MultipartMIMEObj(contentType, CSTR_NULL, boundary.ToCString()));
			i = 0;
		}
		else
		{
			buff[k] = 0;
			NEW_CLASS(obj, Text::MIMEObj::MultipartMIMEObj(contentType, {buff, k}, boundary.ToCString()));
			i = k + boundary.GetLength();
			if (buff[i] == '\r' && buff[i + 1] == '\n')
			{
				i += 2;
			}
		}
		while (true)
		{
			k = Text::StrIndexOfC(&buff[i], buffSize - i, boundary.ToString(), boundary.GetLength());
			if (k == INVALID_INDEX)
				break;
			k += i;

			obj->ParsePart(&buff[i], k - i);
			i = k + boundary.GetLength();
			if (buff[i] == '\r' && buff[i + 1] == '\n')
			{
				i += 2;
			}
		}
		obj->ParsePart(&buff[i], buffSize - i);
		MemFree(buff);
		return obj;
	}
	return 0;
}
