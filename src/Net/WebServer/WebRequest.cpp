#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/WebServer/WebRequest.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextEnc/FormEncoding.h"
#include "Text/TextEnc/URIEncoding.h"

#define MAX_DATA_SIZE 104857600

void Net::WebServer::WebRequest::ParseQuery()
{
	UTF8Char *sbuff;
	UTF8Char *sbuff2;
	const UTF8Char *url = this->GetRequestURI();
	UOSInt urlLen = Text::StrCharCnt(url);
	UTF8Char *strs1[2];
	UTF8Char *strs2[2];
	const UTF8Char *sptr;

	sbuff = MemAlloc(UTF8Char, urlLen);
	sbuff2 = MemAlloc(UTF8Char, urlLen);
	NEW_CLASS(this->queryMap, Data::StringUTF8Map<const UTF8Char *>());
	if (this->GetQueryString(sbuff, urlLen))
	{
		UOSInt scnt;
		Bool hasMore;
		strs1[1] = sbuff;
		hasMore = true;
		while (hasMore)
		{
			hasMore = (Text::StrSplit(strs1, 2, strs1[1], '&') == 2);

			scnt = Text::StrSplit(strs2, 2, strs1[0], '=');
			if (scnt == 2)
			{
				Text::TextEnc::URIEncoding::URIDecode(sbuff2, strs2[1]);
			}
			else
			{
				sbuff2[0] = 0;
			}
			sptr = this->queryMap->Get(strs2[0]);
			if (sptr)
			{
				Text::StringBuilderUTF8 sb;
				sb.Append(sptr);
				sb.AppendChar(PARAM_SEPERATOR, 1);
				sb.Append(sbuff2);
				sptr = this->queryMap->Put(strs2[0], Text::StrCopyNew(sb.ToString()));
			}
			else
			{
				sptr = this->queryMap->Put(strs2[0], Text::StrCopyNew(sbuff2));
			}
			if (sptr)
			{
				Text::StrDelNew(sptr);
			}
		}
	}
	MemFree(sbuff2);
	MemFree(sbuff);
}

void Net::WebServer::WebRequest::ParseFormStr(Data::StringUTF8Map<const UTF8Char *> *formMap, const UInt8 *buff, UOSInt buffSize)
{
	UInt8 *tmpBuff;
	UInt8 b;
	UInt8 b2;
	UTF8Char *tmpStr;
	UTF8Char *tmpName;
	const UTF8Char *csptr;
	UOSInt buffPos;
	UOSInt charCnt;

	tmpBuff = MemAlloc(UInt8, buffSize + 1);
	buffPos = 0;
	tmpName = 0;
	while (buffSize-- > 0)
	{
		b = *buff++;
		if (b == '&')
		{
			if (tmpName)
			{
				tmpBuff[buffPos] = 0;
				csptr = formMap->Get(tmpBuff);
				if (csptr)
				{
					charCnt = Text::StrCharCnt(csptr) + 1;
					charCnt += Text::StrCharCnt(tmpName);
					tmpStr = MemAlloc(UTF8Char, charCnt + 1);
					Text::StrConcat(Text::StrConcat(Text::StrConcat(tmpStr, csptr), (const UTF8Char*)","), tmpName);
					formMap->Put(tmpBuff, tmpStr);
					MemFree((void*)csptr);
				}
				else
				{
					charCnt = Text::StrCharCnt(tmpName);
					tmpStr = MemAlloc(UTF8Char, charCnt + 1);
					Text::StrConcatC(tmpStr, tmpName, charCnt);
					formMap->Put(tmpBuff, tmpStr);
				}
				tmpName = 0;
				buffPos = 0;
			}
		}
		else if (b == '=' && tmpName == 0 && buffPos > 0)
		{
			tmpBuff[buffPos] = 0;
			buffPos++;
			tmpName = &tmpBuff[buffPos];
		}
		else if (b == '%' && buffSize >= 2)
		{
			b = (UInt8)((*buff++) - 0x30);
			b2 = (UInt8)((*buff++) - 0x30);
			if (b >= 0x2a)
			{
				b = (UInt8)(b - 0x27);
			}
			else if (b >= 0xa)
			{
				b = (UInt8)(b - 7);
			}
			if (b2 >= 0x2a)
			{
				b2 = (UInt8)(b2 - 0x27);
			}
			else if (b2 >= 0xa)
			{
				b2 = (UInt8)(b2 - 7);
			}
			tmpBuff[buffPos] = (UInt8)(((b << 4) | b2) & 0xff);
			buffPos++;
			buffSize -= 2;
		}
		else if (b == '+')
		{
			tmpBuff[buffPos] = ' ';
			buffPos++;
		}
		else
		{
			tmpBuff[buffPos] = b;
			buffPos++;
		}
	}

	if (tmpName)
	{
		tmpBuff[buffPos] = 0;
		csptr = formMap->Get(tmpBuff);
		if (csptr)
		{
			charCnt = Text::StrCharCnt(csptr) + 1;
			charCnt += Text::StrCharCnt(tmpName);
			tmpStr = MemAlloc(UTF8Char, charCnt + 1);
			Text::StrConcat(Text::StrConcat(Text::StrConcat(tmpStr, csptr), (const UTF8Char*)","), tmpName);
			formMap->Put(tmpBuff, tmpStr);
			MemFree((void*)csptr);
		}
		else
		{
			charCnt = Text::StrCharCnt(tmpName);
			tmpStr = MemAlloc(UTF8Char, charCnt + 1);
			Text::StrConcatC(tmpStr, tmpName, charCnt);
			formMap->Put(tmpBuff, tmpStr);
		}
		tmpName = 0;
		buffPos = 0;
	}

	MemFree(tmpBuff);
}

void Net::WebServer::WebRequest::ParseFormPart(UInt8 *data, UOSInt dataSize, UOSInt startOfst)
{
	if (dataSize < 4)
		return;
	if (data[dataSize - 1] == '-' && data[dataSize - 2] == '-')
		dataSize -= 2;
	if (data[dataSize - 2] == 13 && data[dataSize - 1] == 10)
		dataSize -= 2;
	if (data[0] == 13 && data[1] == 10)
	{
		data += 2;
		dataSize -= 2;
		startOfst += 2;
	}
	UOSInt i;
	UOSInt j;
	UOSInt lineStart;
	Int32 contType = 0;
	const UTF8Char *formName = 0;
	const UTF8Char *fileName = 0;
	i = 0;
	lineStart = 0;
	while (i < dataSize)
	{
		if (data[i] == 13 && data[i + 1] == 10)
		{
			if (lineStart == i)
			{
				i += 2;
				break;
			}
			
			if (Text::StrStartsWith((Char*)&data[lineStart], "Content-Disposition: form-data;"))
			{
				Char *line;
				Char *lineStrs[10];
				UOSInt strCnt;
				contType = 1;

				line = MemAlloc(Char, i - lineStart - 30);
				MemCopyNO(line, &data[lineStart + 31], i - lineStart - 31);
				line[i - lineStart - 31] = 0;

				strCnt = Text::StrSplitTrim(lineStrs, 10, line, ';');
				j = strCnt;
				while (j-- > 0)
				{
					if (Text::StrStartsWith(lineStrs[j], "name="))
					{
						SDEL_TEXT(formName);
						formName = ParseHeaderVal(&lineStrs[j][5]);
					}
					else if (Text::StrStartsWith(lineStrs[j], "filename="))
					{
						SDEL_TEXT(fileName);
						contType = 2;
						fileName = ParseHeaderVal(&lineStrs[j][9]);
					}
				}
				MemFree(line);
			}
			i++;
			lineStart = i + 1;
		}
		i++;
	}

	if (contType == 1)
	{
		UTF8Char *tmpStr;
		const UTF8Char *csptr;
		if (formName && dataSize > i)
		{
			csptr = this->formMap->Get(formName);
			if (csptr)
			{
				MemFree((void*)csptr);
				tmpStr = MemAlloc(UTF8Char, dataSize - i + 1);
				Text::StrConcatC(tmpStr, &data[i], dataSize - i);
				formMap->Put(formName, tmpStr);
			}
			else
			{
				tmpStr = MemAlloc(UTF8Char, dataSize - i + 1);
				Text::StrConcatC(tmpStr, &data[i], dataSize - i);
				formMap->Put(formName, tmpStr);
			}
		}
	}
	else if (contType == 2)
	{
		if (formName)
		{
			FormFileInfo *info = MemAlloc(FormFileInfo, 1);
			info->ofst = startOfst + i;
			info->leng = dataSize - i;
			info->formName = Text::StrCopyNew(formName);
			if (fileName)
			{
				info->fileName = Text::StrCopyNew(fileName);
			}
			else
			{
				info->fileName = 0;
			}
			this->formFileList->Add(info);
		}
	}
	SDEL_TEXT(formName);
	SDEL_TEXT(fileName);
}

const UTF8Char *Net::WebServer::WebRequest::ParseHeaderVal(Char *headerData)
{
	UTF8Char *outStr;
	UOSInt charCnt = Text::StrCharCnt(headerData);
	if (headerData[0] == '"' && headerData[charCnt-1] == '"')
	{
		outStr = MemAlloc(UTF8Char, charCnt - 1);
		MemCopyNO(outStr, &headerData[1], charCnt - 2);
		outStr[charCnt - 2] = 0;
	}
	else
	{
		outStr = MemAlloc(UTF8Char, charCnt + 1);
		MemCopyNO(outStr, &headerData[1], charCnt);
		outStr[charCnt] = 0;
	}
	return outStr;
}

const UTF8Char *Net::WebServer::WebRequest::GetSHeader(const UTF8Char *name)
{
	OSInt i = this->headerNames->SortedIndexOf(name);
	if (i >= 0)
	{
		return this->headerVals->GetItem((UOSInt)i);
	}
	else
	{
		return 0;
	}
}

Net::WebServer::WebRequest::WebRequest(const UTF8Char *requestURI, RequestMethod reqMeth, RequestProtocol reqProto, Bool secureConn, const Net::SocketUtil::AddressInfo *cliAddr, UInt16 cliPort, UInt16 svrPort)
{
	this->requestURI = Text::StrCopyNew(requestURI);
	this->reqMeth = reqMeth;
	this->reqProto = reqProto;
	this->secureConn = secureConn;
	this->cliAddr = *cliAddr;
	this->cliPort = cliPort;
	this->svrPort = svrPort;
	NEW_CLASS(this->headerNames, Data::ArrayListStrUTF8());
	NEW_CLASS(this->headerVals, Data::ArrayListStrUTF8());
	this->queryMap = 0;
	this->formMap = 0;
	this->formFileList = 0;
	this->reqData = 0;
	this->reqDataSize = 0;
	this->chunkMStm = 0;
}

Net::WebServer::WebRequest::~WebRequest()
{
	UOSInt i;
	Text::StrDelNew(this->requestURI);

	i = this->headerNames->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->headerNames->RemoveAt(i));
		Text::StrDelNew(this->headerVals->RemoveAt(i));
	}
	DEL_CLASS(this->headerNames);
	DEL_CLASS(this->headerVals);
	if (this->queryMap)
	{
		Data::ArrayList<const UTF8Char *> *strs = this->queryMap->GetValues();
		i = strs->GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew(strs->GetItem(i));
		}
		DEL_CLASS(this->queryMap);
	}
	if (this->formMap)
	{
		Data::ArrayList<const UTF8Char *> *strs = this->formMap->GetValues();
		i = strs->GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew(strs->GetItem(i));
		}
		DEL_CLASS(this->formMap);
	}
	if (this->formFileList)
	{
		FormFileInfo *fileInfo;
		i = this->formFileList->GetCount();
		while (i-- > 0)
		{
			fileInfo = this->formFileList->GetItem(i);
			SDEL_TEXT(fileInfo->fileName);
			MemFree(fileInfo);
		}
		DEL_CLASS(this->formFileList);
	}
	if (this->reqData)
	{
		MemFree(this->reqData);
		this->reqData = 0;
	}
	SDEL_CLASS(this->chunkMStm);
}

void Net::WebServer::WebRequest::AddHeader(const UTF8Char *name, const UTF8Char *value)
{
	UOSInt i = this->headerNames->SortedInsert(Text::StrCopyNew(name));
	this->headerVals->Insert(i, Text::StrCopyNew(value));
}

UTF8Char *Net::WebServer::WebRequest::GetHeader(UTF8Char *sbuff, const UTF8Char *name, UOSInt buffLen)
{
	OSInt i = this->headerNames->SortedIndexOf(name);
	if (i >= 0)
	{
		return Text::StrConcatS(sbuff, this->headerVals->GetItem((UOSInt)i), buffLen);
	}
	else
	{
		return 0;
	}
}

Bool Net::WebServer::WebRequest::GetHeader(Text::StringBuilderUTF *sb, const UTF8Char *name)
{
	const UTF8Char *hdr = this->GetSHeader(name);
	if (hdr == 0)
		return false;
	sb->Append(hdr);
	return true;
}

UOSInt Net::WebServer::WebRequest::GetHeaderNames(Data::ArrayList<const UTF8Char*> *names)
{
	names->AddAll(this->headerNames);
	return this->headerNames->GetCount();
}

UOSInt Net::WebServer::WebRequest::GetHeaderCnt()
{
	return this->headerNames->GetCount();
}

const UTF8Char *Net::WebServer::WebRequest::GetHeaderName(UOSInt index)
{
	return this->headerNames->GetItem(index);
}

const UTF8Char *Net::WebServer::WebRequest::GetHeaderValue(UOSInt index)
{
	return this->headerVals->GetItem(index);
}

const UTF8Char *Net::WebServer::WebRequest::GetRequestURI()
{
	return this->requestURI;
}

Net::WebServer::IWebRequest::RequestProtocol Net::WebServer::WebRequest::GetProtocol()
{
	return this->reqProto;
}

const UTF8Char *Net::WebServer::WebRequest::GetQueryValue(const UTF8Char *name)
{
	if (this->queryMap == 0)
	{
		this->ParseQuery();
	}
	return this->queryMap->Get(name);
}

Bool Net::WebServer::WebRequest::HasQuery(const UTF8Char *name)
{
	if (this->queryMap == 0)
	{
		this->ParseQuery();
	}
	const UTF8Char *sptr = this->queryMap->Get(name);
	return sptr != 0;
}

Net::WebServer::IWebRequest::RequestMethod Net::WebServer::WebRequest::GetReqMethod()
{
	return this->reqMeth;
}

void Net::WebServer::WebRequest::ParseHTTPForm()
{
	if (this->formMap)
	{
		return;
	}
	if ((this->reqData == 0) || (this->reqDataSize != this->reqCurrSize))
	{
		return;
	}

	Text::StringBuilderUTF8 *sb;
	NEW_CLASS(sb, Text::StringBuilderUTF8());
	if (this->GetHeader(sb, (const UTF8Char*)"Content-Type"))
	{
		if (Text::StrEquals(sb->ToString(), (const UTF8Char*)"application/x-www-form-urlencoded"))
		{
			NEW_CLASS(this->formMap, Data::StringUTF8Map<const UTF8Char *>());
			ParseFormStr(this->formMap, this->reqData, this->reqDataSize);
		}
		else if (Text::StrStartsWith(sb->ToString(), (const UTF8Char*)"multipart/form-data"))
		{
			UTF8Char *sptr = sb->ToString();
			UOSInt i = Text::StrIndexOf(sptr, (const UTF8Char*)"boundary=");
			if (i != INVALID_INDEX)
			{
				UInt8 *boundary = &sptr[i + 9];
				UOSInt boundSize = Text::StrCharCnt(&sptr[i + 9]);
				NEW_CLASS(this->formMap, Data::StringUTF8Map<const UTF8Char *>());
				NEW_CLASS(this->formFileList, Data::ArrayList<FormFileInfo *>());

				UOSInt formStart;
				UOSInt formCurr;
				Bool eq;
				formStart = 0;
				formCurr = 0;
				while (formCurr <= this->reqDataSize - boundSize)
				{
					eq = true;
					i = boundSize;
					while (i-- > 0)
					{
						if (boundary[i] != this->reqData[formCurr + i])
						{
							eq = false;
							break;
						}
					}
					if (eq)
					{
						if (formCurr > formStart)
						{
							ParseFormPart(&this->reqData[formStart], formCurr - formStart, formStart);
						}
						formCurr += boundSize;
						if (formCurr + 2 <= this->reqDataSize && this->reqData[formCurr] == 13 && this->reqData[formCurr] == 10)
						{
							formCurr += 2;
						}
						formStart = formCurr;
					}
					else
					{
						formCurr++;
					}
				}
				if (formStart < this->reqDataSize)
				{
					ParseFormPart(&this->reqData[formStart], this->reqDataSize - formStart, formStart);
				}
			}
		}
	}
	DEL_CLASS(sb);
}

const UTF8Char *Net::WebServer::WebRequest::GetHTTPFormStr(const UTF8Char *name)
{
	if (this->formMap == 0)
		return 0;
	return this->formMap->Get(name);
}

const UInt8 *Net::WebServer::WebRequest::GetHTTPFormFile(const UTF8Char *formName, UOSInt index, UTF8Char *fileName, UOSInt fileNameBuffSize, UOSInt *fileSize)
{
	if (this->formFileList == 0)
		return 0;
	UOSInt i = 0;
	UOSInt j = this->formFileList->GetCount();
	while (i < j)
	{
		FormFileInfo *info = this->formFileList->GetItem(i);
		if (Text::StrEquals(info->formName, formName))
		{
			if (index == 0)
			{
				if (fileSize)
				{
					*fileSize = info->leng;
				}
				if (fileName && info->fileName)
				{
					Text::StrConcatS(fileName, info->fileName, fileNameBuffSize);
				}
				return &this->reqData[info->ofst];
			}
			index--;
		}
		i++;
	}
	return 0;
}

void Net::WebServer::WebRequest::GetRequestURLBase(Text::StringBuilderUTF *sb)
{
	UInt16 defPort;
	const UTF8Char *csptr;
	switch (this->reqProto)
	{
	case RequestProtocol::HTTP1_0:
	case RequestProtocol::HTTP1_1:
		if (this->secureConn)
		{
			sb->Append((const UTF8Char*)"https://");
			defPort=443;
		}
		else
		{
			sb->Append((const UTF8Char*)"http://");
			defPort=80;
		}
		csptr = this->GetSHeader((const UTF8Char*)"Host");
		if (csptr)
		{
			sb->Append(csptr);
		}
		if (this->svrPort != defPort)
		{
//			sb->AppendChar(':', 1);
//			sb->AppendU16(this->svrPort);
		}
		break;
	case RequestProtocol::RTSP1_0:
		sb->Append((const UTF8Char*)"rtsp://");
		defPort=554;
		break;
	}
}

const Net::SocketUtil::AddressInfo *Net::WebServer::WebRequest::GetClientAddr()
{
	return &this->cliAddr;
}

UInt16 Net::WebServer::WebRequest::GetClientPort()
{
	return this->cliPort;
}

const UInt8 *Net::WebServer::WebRequest::GetReqData(UOSInt *dataSize)
{
	if (this->reqData == 0)
	{
		*dataSize = 0;
		return 0;
	}
	else
	{
		*dataSize = this->reqCurrSize;
		return this->reqData;
	}
}

Bool Net::WebServer::WebRequest::HasData()
{
	const UTF8Char *contLeng;
	contLeng = this->GetSHeader((const UTF8Char*)"Content-Length");
	if (contLeng == 0)
	{
		if (this->GetReqMethod() != Net::WebServer::IWebRequest::RequestMethod::HTTP_POST)
		{
			return false;
		}
		contLeng = this->GetSHeader((const UTF8Char*)"Connection");
		if (contLeng == 0 || !Text::StrEquals(contLeng, (const UTF8Char*)"close"))
		{
			return false;
		}
		contLeng = this->GetSHeader((const UTF8Char*)"Transfer-Encoding");
		if (contLeng == 0 || !Text::StrEquals(contLeng, (const UTF8Char*)"chunked"))
		{
			return false;
		}
		this->reqDataSize = (UOSInt)-1;
		return true;
	}
	this->reqDataSize = (UOSInt)Text::StrToUInt64(contLeng);
	if (this->reqDataSize > 0 && this->reqDataSize <= MAX_DATA_SIZE)
		return true;
	return false;
}

void Net::WebServer::WebRequest::DataStart()
{
	if (this->reqData == 0 && this->HasData())
	{
		if ((OSInt)this->reqDataSize == -1)
		{
			this->reqData = MemAlloc(UInt8, 65536);
			NEW_CLASS(this->chunkMStm, IO::MemoryStream((const UTF8Char*)"Net.WebServer.WebReqeust.chunkMStm"));
		}
		else
		{
			this->reqData = MemAlloc(UInt8, this->reqDataSize);
		}
		this->reqCurrSize = 0;
	}
}

Bool Net::WebServer::WebRequest::DataStarted()
{
	return this->reqData != 0;
}

Bool Net::WebServer::WebRequest::DataFull()
{
	if (this->reqData == 0 || ((OSInt)this->reqDataSize != -1 && this->reqCurrSize >= this->reqDataSize))
		return true;
	return false;
}
UOSInt Net::WebServer::WebRequest::DataPut(const UInt8 *data, UOSInt dataSize)
{
	if (this->reqData == 0)
		return 0;
	if ((OSInt)this->reqDataSize == -1)
	{
		if (this->reqCurrSize + dataSize > 65536)
		{
			MemCopyNO(&this->reqData[this->reqCurrSize], data, 65536 - this->reqCurrSize);
			dataSize = 65536 - this->reqCurrSize;
			this->reqCurrSize = 65536;
			this->reqDataSize = 65536;
		}
		else
		{
			MemCopyNO(&this->reqData[this->reqCurrSize], data, dataSize);
			this->reqCurrSize += dataSize;
		}

		UOSInt i;
		UOSInt j;
		UInt32 leng;
		i = 0;
		while (i <= this->reqCurrSize - 2)
		{
			if (this->reqData[i] == 13 && this->reqData[i + 1] == 10)
			{
				UInt8 *buff = this->chunkMStm->GetBuff(&this->reqDataSize);
				MemFree(this->reqData);
				this->reqData = MemAlloc(UInt8, this->reqDataSize);
				MemCopyNO(this->reqData, buff, this->reqDataSize);
				this->reqCurrSize = this->reqDataSize;
				i = 0;
				break;
			}
			j = i;
			while (j <= this->reqCurrSize - 2)
			{
				if (this->reqData[j] == 13 && this->reqData[j + 1] == 10)
				{
					this->reqData[j] = 0;
					leng = Text::StrHex2UInt32C(&this->reqData[i]);
					this->reqData[j] = 13;
					if (j + 4 + leng <= this->reqCurrSize)
					{
						if (this->reqData[j + 2 + leng] != 13 || this->reqData[j + 3 + leng] != 10)
						{
							i = 0;
							this->reqDataSize = 0;
							MemFree(this->reqData);
							this->reqData = 0;
							this->reqCurrSize = 0;
							break;
						}

						if (leng > 0)
						{
							this->chunkMStm->Write(&this->reqData[j + 2], leng);
							i = j + 4 + leng;
						}
						else
						{
							UInt8 *buff = this->chunkMStm->GetBuff(&this->reqDataSize);
							MemFree(this->reqData);
							this->reqData = MemAlloc(UInt8, this->reqDataSize);
							MemCopyNO(this->reqData, buff, this->reqDataSize);
							this->reqCurrSize = this->reqDataSize;
							i = 0;
							j = 0;
						}
						break;
					}
					else
					{
						j = 0;
						break;
					}
				}
				j++;

				if (j - i > 8)
				{
					i = 0;
					this->reqDataSize = 0;
					MemFree(this->reqData);
					this->reqData = 0;
					this->reqCurrSize = 0;
					break;
				}
			}
			if (j == 0 || j > this->reqCurrSize - 2)
				break;
		}
		if (i >= this->reqCurrSize)
		{
			this->reqCurrSize = 0;
		}
		else if (i > 0)
		{
			MemCopyO(this->reqData, &this->reqData[i], this->reqCurrSize - i);
			this->reqCurrSize -= i;
		}
		return dataSize;
	}
	else
	{
		UOSInt sizeLeft = this->reqDataSize - this->reqCurrSize;
		if (sizeLeft > dataSize)
		{
			MemCopyNO(&this->reqData[this->reqCurrSize], data, dataSize);
			this->reqCurrSize += dataSize;
			return dataSize;
		}
		else if (sizeLeft > 0)
		{
			MemCopyNO(&this->reqData[this->reqCurrSize], data, sizeLeft);
			this->reqCurrSize += sizeLeft;
			return sizeLeft;
		}
	}
	return 0;
}

UOSInt Net::WebServer::WebRequest::GetMaxDataSize()
{
	return MAX_DATA_SIZE;
}
