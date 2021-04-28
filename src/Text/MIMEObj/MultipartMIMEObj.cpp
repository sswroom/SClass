#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListStrUTF8.h"
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
	Data::ArrayListStrUTF8 hdrNames;
	Data::ArrayListStrUTF8 hdrValues;
	Text::StringBuilderUTF8 sb;
	UTF8Char *sptr;
	UTF8Char *sarr[2];
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
					j = hdrNames.SortedInsert(Text::StrCopyNew(sarr[0]));
					hdrValues.Insert(j, Text::StrCopyNew(sarr[1]));
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
			Text::StrDelNew(hdrNames.GetItem(i));
			Text::StrDelNew(hdrValues.GetItem(i));
		}
		return;
	}

	si = hdrNames.SortedIndexOf((const UTF8Char*)"Content-Type");
	if (si >= 0)
	{
		const UTF8Char *contType = hdrValues.GetItem((UOSInt)si);
		Text::IMIMEObj *obj = 0;
		IO::StmData::MemoryData *mdata;
		si = hdrNames.SortedIndexOf((const UTF8Char*)"Content-Transfer-Encoding");
		if (si >= 0)
		{
			const UTF8Char *tenc = hdrValues.GetItem((UOSInt)si);
			if (Text::StrEquals(tenc, (const UTF8Char*)"base64"))
			{
				Text::TextBinEnc::Base64Enc b64;
				UInt8 *tmpBuff = MemAlloc(UInt8, buffSize - lineStart);
				j = b64.DecodeBin(&buff[lineStart], buffSize - lineStart, tmpBuff);

				NEW_CLASS(mdata, IO::StmData::MemoryData(tmpBuff, j));
				obj = Text::IMIMEObj::ParseFromData(mdata, contType);
				DEL_CLASS(mdata);

				MemFree(tmpBuff);
			}
			else if (Text::StrEquals(tenc, (const UTF8Char*)"quoted-printable"))
			{
				Text::TextBinEnc::QuotedPrintableEnc qpenc;
				UInt8 *tmpBuff = MemAlloc(UInt8, buffSize - lineStart);
				j = qpenc.DecodeBin(&buff[lineStart], buffSize - lineStart, tmpBuff);

				NEW_CLASS(mdata, IO::StmData::MemoryData(tmpBuff, j));
				obj = Text::IMIMEObj::ParseFromData(mdata, contType);
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
			obj = Text::IMIMEObj::ParseFromData(mdata, contType);
			DEL_CLASS(mdata);
		}

		if (obj)
		{
			i = this->AddPart(obj);
			j = 0;
			k = hdrNames.GetCount();
			while (j < k)
			{
				this->AddPartHeader(i, hdrNames.GetItem(j), hdrValues.GetItem(j));
				j++;
			}
		}
	}

	i = hdrNames.GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(hdrNames.GetItem(i));
		Text::StrDelNew(hdrValues.GetItem(i));
	}
}

Text::MIMEObj::MultipartMIMEObj::MultipartMIMEObj(const UTF8Char *contentType, const UTF8Char *defMsg, const UTF8Char *boundary) : Text::IMIMEObj((const UTF8Char*)"multipart/mixed")
{
	this->contentType = Text::StrCopyNew(contentType);
	if (defMsg)
	{
		this->defMsg = Text::StrCopyNew(defMsg);
	}
	else
	{
		this->defMsg = 0;
	}
	this->boundary = Text::StrCopyNew(boundary);
	NEW_CLASS(parts, Data::ArrayList<PartInfo*>());
}

Text::MIMEObj::MultipartMIMEObj::MultipartMIMEObj(const UTF8Char *contentType, const UTF8Char *defMsg) : Text::IMIMEObj((const UTF8Char*)"multipart/mixed")
{
	Text::StringBuilderUTF8 sbc;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	sbc.Append((const UTF8Char*)"----------");
	sbc.AppendI64(dt.ToTicks());
	sbc.AppendOSInt((0x7fffffff & (OSInt)this));
	this->boundary = Text::StrCopyNew(sbc.ToString());
	sbc.ClearStr();
	sbc.Append(contentType);
	sbc.Append((const UTF8Char*)";\r\n\tboundary=\"");
	sbc.Append(this->boundary);
	sbc.Append((const UTF8Char*)"\"");
	this->contentType = Text::StrCopyNew(sbc.ToString());
	if (defMsg)
	{
		this->defMsg = Text::StrCopyNew(defMsg);
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
	Text::StrDelNew(this->contentType);
	SDEL_TEXT(this->defMsg);
	Text::StrDelNew(this->boundary);
	i = this->parts->GetCount();
	while (i-- > 0)
	{
		part = this->parts->GetItem(i);
		DEL_CLASS(part);
	}
	DEL_CLASS(this->parts);
}

const UTF8Char *Text::MIMEObj::MultipartMIMEObj::GetClassName()
{
	return (const UTF8Char*)"MultipartMIMEObj";
}

const UTF8Char *Text::MIMEObj::MultipartMIMEObj::GetContentType()
{
	return this->contentType;
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
	const UTF8Char *hdrName;
	const UTF8Char *hdrValue;
	Text::StringBuilderUTF8 sbc;
	if (this->defMsg)
	{
		len = Text::StrCharCnt(this->defMsg);
		stm->Write((const UInt8*)this->defMsg, len);
		ret += len;
	}
	i = 0;
	j = this->parts->GetCount();
	while (i < j)
	{
		part = this->parts->GetItem(i);
		encType = 0;
		sbc.ClearStr();
		sbc.Append((const UTF8Char*)"\r\n--");
		sbc.Append(this->boundary);
		sbc.Append((const UTF8Char*)"\r\n");
		stm->Write((const UInt8*)sbc.ToString(), sbc.GetLength());
		ret += sbc.GetLength();

		k = 0;
		l = part->GetHeaderCount();
		while (k < l)
		{
			hdrName = part->GetHeaderName(k);
			hdrValue = part->GetHeaderValue(k);
			sbc.ClearStr();
			sbc.Append(hdrName);
			sbc.Append((const UTF8Char*)": ");
			sbc.Append(hdrValue);
			sbc.Append((const UTF8Char*)"\r\n");
			stm->Write((const UInt8*)sbc.ToString(), sbc.GetLength());
			ret += sbc.GetLength();
			if (Text::StrEquals(hdrName, (const UTF8Char*)"Content-Transfer-Encoding"))
			{
				if (Text::StrEquals(hdrValue, (const UTF8Char*)"base64"))
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
	sbc.Append((const UTF8Char*)"\r\n--");
	sbc.Append(this->boundary);
	sbc.Append((const UTF8Char*)"--");
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

const UTF8Char *Text::MIMEObj::MultipartMIMEObj::GetDefMsg()
{
	return this->defMsg;
}

UOSInt Text::MIMEObj::MultipartMIMEObj::AddPart(Text::IMIMEObj *obj)
{
	PartInfo *part;
	NEW_CLASS(part, PartInfo(obj));
	return this->parts->Add(part);
}

Bool Text::MIMEObj::MultipartMIMEObj::AddPartHeader(UOSInt partIndex, const UTF8Char *name, const UTF8Char *value)
{
	PartInfo *part = this->parts->GetItem(partIndex);
	if (part == 0)
		return false;
	part->AddHeader(name, value);
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

Text::MIMEObj::MultipartMIMEObj *Text::MIMEObj::MultipartMIMEObj::ParseFile(const UTF8Char *contentType, IO::IStreamData *data)
{
	UOSInt j;
	if (Text::StrStartsWith(contentType, (const UTF8Char*)"multipart/mixed;"))
	{
		j = 16;
	}
	else if (Text::StrStartsWith(contentType, (const UTF8Char*)"multipart/related;"))
	{
		j = 18;
	}
	else if (Text::StrStartsWith(contentType, (const UTF8Char*)"multipart/alternative;"))
	{
		j = 22;
	}
	else
	{
		return 0;
	}

	UOSInt i = Text::StrCharCnt(contentType);
	OSInt k;
	while (contentType[j] == '\r' || contentType[j] == '\n' || contentType[j] == '\t' || contentType[j] == ' ')
	{
		j++;
	}
	if (Text::StrStartsWith(&contentType[j], (const UTF8Char*)"boundary="))
	{
		Text::StringBuilderUTF8 boundary;
		Text::MIMEObj::MultipartMIMEObj *obj;
		UOSInt buffSize;
		UInt8 *buff;
		k = Text::StrIndexOf(&contentType[j], ';');
		if (k >= 0)
		{
			i = j + (UOSInt)k;
		}

		boundary.Append((const UTF8Char*)"--");
		if (contentType[j + 9] == '"' && contentType[i - 1] == '"')
		{
			boundary.AppendC(&contentType[j + 10], i - j - 11);
		}
		else
		{
			boundary.AppendC(&contentType[j + 9], i - j - 9);
		}

		buffSize = (UOSInt)data->GetDataSize();
		buff = MemAlloc(UInt8, buffSize + 1);
		data->GetRealData(0, buffSize, buff);
		buff[buffSize] = 0;

		k = Text::StrIndexOf(buff, boundary.ToString());
		if (k < 0)
		{
			NEW_CLASS(obj, Text::MIMEObj::MultipartMIMEObj(contentType, 0, boundary.ToString()));
			i = 0;
		}
		else
		{
			buff[k] = 0;
			NEW_CLASS(obj, Text::MIMEObj::MultipartMIMEObj(contentType, buff, boundary.ToString()));
			i = (UOSInt)k + boundary.GetLength();
			if (buff[i] == '\r' && buff[i + 1] == '\n')
			{
				i += 2;
			}
		}
		while (true)
		{
			k = Text::StrIndexOf(&buff[i], boundary.ToString());
			if (k < 0)
				break;
			k += (OSInt)i;

			obj->ParsePart(&buff[i], (UOSInt)k - i);
			i = (UOSInt)k + boundary.GetLength();
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
