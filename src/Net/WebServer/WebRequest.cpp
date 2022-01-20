#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/WebServer/WebRequest.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextBinEnc/URIEncoding.h"

#define MAX_DATA_SIZE 104857600

void Net::WebServer::WebRequest::ParseQuery()
{
	UTF8Char *sbuff;
	UTF8Char *sbuff2;
	UTF8Char *sptr;
	Text::String *url = this->GetRequestURI();
	UOSInt urlLen = url->leng;
	Text::PString strs1[2];
	Text::PString strs2[2];
	Text::String *s;

	sbuff = MemAlloc(UTF8Char, urlLen);
	NEW_CLASS(this->queryMap, Data::FastStringMap<Text::String *>());
	if ((sptr = this->GetQueryString(sbuff, urlLen)) != 0)
	{
		sbuff2 = MemAlloc(UTF8Char, urlLen);
		UOSInt scnt;
		Bool hasMore;
		strs1[1].v = sbuff;
		strs1[1].len = (UOSInt)(sptr - sbuff);
		hasMore = true;
		while (hasMore)
		{
			hasMore = (Text::StrSplitP(strs1, 2, strs1[1].v, strs1[1].len, '&') == 2);

			scnt = Text::StrSplitP(strs2, 2, strs1[0].v, strs1[0].len, '=');
			if (scnt == 2)
			{
				sptr = Text::TextBinEnc::URIEncoding::URIDecode(sbuff2, strs2[1].v);
			}
			else
			{
				sbuff2[0] = 0;
				sptr = sbuff2;
			}
			s = this->queryMap->GetC(strs2[0].v, strs2[0].len);
			if (s)
			{
				Text::StringBuilderUTF8 sb;
				sb.Append(s);
				sb.AppendChar(PARAM_SEPERATOR, 1);
				sb.AppendC(sbuff2, (UOSInt)(sptr - sbuff2));
				s = this->queryMap->PutC(strs2[0].v, strs2[0].len, Text::String::New(sb.ToString(), sb.GetLength()));
			}
			else
			{
				s = this->queryMap->PutC(strs2[0].v, strs2[0].len, Text::String::New(sbuff2, (UOSInt)(sptr - sbuff2)));
			}
			if (s)
			{
				s->Release();
			}
		}
		MemFree(sbuff2);
	}
	MemFree(sbuff);
}

void Net::WebServer::WebRequest::ParseFormStr(Data::FastStringMap<Text::String *> *formMap, const UInt8 *buff, UOSInt buffSize)
{
	UInt8 *tmpBuff;
	UInt8 b;
	UInt8 b2;
	Text::String *tmpStr;
	UTF8Char *tmpName;
	UOSInt tmpNameLen;
	UOSInt nameLen = 0;
	Text::String *s;
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
				s = formMap->GetC(tmpBuff, nameLen);
				if (s)
				{
					charCnt = s->leng + 1;
					tmpNameLen = (UOSInt)(&tmpBuff[buffPos] - tmpName);
					charCnt += tmpNameLen;
					tmpStr = Text::String::New(charCnt);
					Text::StrConcatC(Text::StrConcatC(s->ConcatTo(tmpStr->v), UTF8STRC(",")), tmpName, tmpNameLen);
					formMap->PutC(tmpBuff, nameLen, tmpStr);
					s->Release();
				}
				else
				{
					formMap->PutC(tmpBuff, nameLen, Text::String::New(tmpName, (UOSInt)(&tmpBuff[buffPos] - tmpName)));
				}
				tmpName = 0;
				buffPos = 0;
			}
		}
		else if (b == '=' && tmpName == 0 && buffPos > 0)
		{
			tmpBuff[buffPos] = 0;
			nameLen = buffPos;
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
		s = formMap->GetC(tmpBuff, nameLen);
		if (s)
		{
			charCnt = s->leng + 1;
			tmpNameLen = (UOSInt)(&tmpBuff[buffPos] - tmpName);
			charCnt += tmpNameLen;
			tmpStr = Text::String::New(charCnt);
			Text::StrConcatC(Text::StrConcatC(s->ConcatTo(tmpStr->v), UTF8STRC(",")), tmpName, tmpNameLen);
			formMap->PutC(tmpBuff, nameLen, tmpStr);
			s->Release();
		}
		else
		{
			formMap->PutC(tmpBuff, nameLen, Text::String::New(tmpName, (UOSInt)(&tmpBuff[buffPos] - tmpName)));
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
			
			if (Text::StrStartsWithC(&data[lineStart], i - lineStart, UTF8STRC("Content-Disposition: form-data;")))
			{
				UTF8Char *line;
				Text::PString lineStrs[10];
				UOSInt strCnt;
				contType = 1;

				line = MemAlloc(UTF8Char, i - lineStart - 30);
				MemCopyNO(line, &data[lineStart + 31], i - lineStart - 31);
				line[i - lineStart - 31] = 0;

				strCnt = Text::StrSplitTrimP(lineStrs, 10, line, i - lineStart - 31, ';');
				j = strCnt;
				while (j-- > 0)
				{
					if (Text::StrStartsWithC(lineStrs[j].v, lineStrs[j].len, UTF8STRC("name=")))
					{
						SDEL_TEXT(formName);
						formName = ParseHeaderVal(&lineStrs[j].v[5], lineStrs[j].len - 5);
					}
					else if (Text::StrStartsWithC(lineStrs[j].v, lineStrs[j].len, UTF8STRC("filename=")))
					{
						SDEL_TEXT(fileName);
						contType = 2;
						fileName = ParseHeaderVal(&lineStrs[j].v[9], lineStrs[j].len - 9);
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
		Text::String *s;
		if (formName && dataSize > i)
		{
			s = this->formMap->Get(formName);
			if (s)
			{
				s->Release();
				formMap->Put(formName, Text::String::New(&data[i], dataSize - i));
			}
			else
			{
				formMap->Put(formName, Text::String::New(&data[i], dataSize - i));
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
			info->formName = Text::String::NewNotNull(formName);
			info->fileName = Text::String::NewOrNull(fileName);
			this->formFileList->Add(info);
		}
	}
	SDEL_TEXT(formName);
	SDEL_TEXT(fileName);
}

const UTF8Char *Net::WebServer::WebRequest::ParseHeaderVal(UTF8Char *headerData, UOSInt dataLen)
{
	UTF8Char *outStr;
	if (headerData[0] == '"' && headerData[dataLen-1] == '"')
	{
		outStr = MemAlloc(UTF8Char, dataLen - 1);
		MemCopyNO(outStr, &headerData[1], dataLen - 2);
		outStr[dataLen - 2] = 0;
	}
	else
	{
		outStr = MemAlloc(UTF8Char, dataLen + 1);
		MemCopyNO(outStr, &headerData[1], dataLen);
		outStr[dataLen] = 0;
	}
	return outStr;
}

Net::WebServer::WebRequest::WebRequest(const UTF8Char *requestURI, UOSInt uriLen, RequestMethod reqMeth, RequestProtocol reqProto, Bool secureConn, const Net::SocketUtil::AddressInfo *cliAddr, UInt16 cliPort, UInt16 svrPort)
{
	this->requestURI = Text::String::New(requestURI, uriLen);
	this->reqMeth = reqMeth;
	this->reqProto = reqProto;
	this->secureConn = secureConn;
	this->cliAddr = *cliAddr;
	this->cliPort = cliPort;
	this->svrPort = svrPort;
	NEW_CLASS(this->headers, Data::FastStringMap<Text::String*>());
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
	this->requestURI->Release();
	LIST_FREE_STRING(this->headers);
	DEL_CLASS(this->headers);
	if (this->queryMap)
	{
		LIST_FREE_STRING(this->queryMap);
		DEL_CLASS(this->queryMap);
	}
	if (this->formMap)
	{
		LIST_FREE_STRING(this->formMap);
		DEL_CLASS(this->formMap);
	}
	if (this->formFileList)
	{
		FormFileInfo *fileInfo;
		i = this->formFileList->GetCount();
		while (i-- > 0)
		{
			fileInfo = this->formFileList->GetItem(i);
			fileInfo->formName->Release();
			SDEL_STRING(fileInfo->fileName);
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

void Net::WebServer::WebRequest::AddHeaderC(const UTF8Char *name, UOSInt nameLen, const UTF8Char *value, UOSInt valueLen)
{
	Text::String *s = this->headers->PutC(name, nameLen, Text::String::New(value, valueLen));
	SDEL_STRING(s);
}

Text::String *Net::WebServer::WebRequest::GetSHeader(const UTF8Char *name, UOSInt nameLen)
{
	return this->headers->GetC(name, nameLen);
}

UTF8Char *Net::WebServer::WebRequest::GetHeader(UTF8Char *sbuff, const UTF8Char *name, UOSInt buffLen)
{
	Text::String *s = this->headers->Get(name);
	if (s)
	{
		return Text::StrConcatS(sbuff, s->v, buffLen);
	}
	else
	{
		return 0;
	}
}

Bool Net::WebServer::WebRequest::GetHeaderC(Text::StringBuilderUTF *sb, const UTF8Char *name, UOSInt nameLen)
{
	Text::String *hdr = this->headers->GetC(name, nameLen);
	if (hdr == 0)
		return false;
	sb->Append(hdr);
	return true;
}

UOSInt Net::WebServer::WebRequest::GetHeaderNames(Data::ArrayList<Text::String*> *names)
{
	UOSInt i = 0;
	UOSInt j = this->headers->GetCount();
	names->EnsureCapacity(j);
	while (i < j)
	{
		names->Add(this->headers->GetKey(i));
		i++;
	}
	return j;
}

UOSInt Net::WebServer::WebRequest::GetHeaderCnt()
{
	return this->headers->GetCount();
}

Text::String *Net::WebServer::WebRequest::GetHeaderName(UOSInt index)
{
	return this->headers->GetKey(index);
}

Text::String *Net::WebServer::WebRequest::GetHeaderValue(UOSInt index)
{
	return this->headers->GetItem(index);
}

Text::String *Net::WebServer::WebRequest::GetRequestURI()
{
	return this->requestURI;
}

Net::WebServer::IWebRequest::RequestProtocol Net::WebServer::WebRequest::GetProtocol()
{
	return this->reqProto;
}

Text::String *Net::WebServer::WebRequest::GetQueryValue(const UTF8Char *name, UOSInt nameLen)
{
	if (this->queryMap == 0)
	{
		this->ParseQuery();
	}
	return this->queryMap->GetC(name, nameLen);
}

Bool Net::WebServer::WebRequest::HasQuery(const UTF8Char *name, UOSInt nameLen)
{
	if (this->queryMap == 0)
	{
		this->ParseQuery();
	}
	Text::String *sptr = this->queryMap->GetC(name, nameLen);
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
	if (this->GetHeaderC(sb, UTF8STRC("Content-Type")))
	{
		if (Text::StrEqualsC(sb->ToString(), sb->GetLength(), UTF8STRC("application/x-www-form-urlencoded")))
		{
			NEW_CLASS(this->formMap, Data::FastStringMap<Text::String *>());
			ParseFormStr(this->formMap, this->reqData, this->reqDataSize);
		}
		else if (Text::StrStartsWithC(sb->ToString(), sb->GetLength(), UTF8STRC("multipart/form-data")))
		{
			UTF8Char *sptr = sb->ToString();
			UOSInt i = Text::StrIndexOf(sptr, (const UTF8Char*)"boundary=");
			if (i != INVALID_INDEX)
			{
				UInt8 *boundary = &sptr[i + 9];
				UOSInt boundSize = sb->GetLength() - i - 9;
				NEW_CLASS(this->formMap, Data::FastStringMap<Text::String *>());
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

Text::String *Net::WebServer::WebRequest::GetHTTPFormStr(const UTF8Char *name, UOSInt nameLen)
{
	if (this->formMap == 0)
		return 0;
	return this->formMap->GetC(name, nameLen);
}

const UInt8 *Net::WebServer::WebRequest::GetHTTPFormFile(const UTF8Char *formName, UOSInt index, UTF8Char *fileName, UOSInt fileNameBuffSize, UOSInt *fileSize)
{
	if (this->formFileList == 0)
		return 0;
	UOSInt formNameLen = Text::StrCharCnt(formName);
	UOSInt i = 0;
	UOSInt j = this->formFileList->GetCount();
	while (i < j)
	{
		FormFileInfo *info = this->formFileList->GetItem(i);
		if (info->formName->Equals(formName, formNameLen))
		{
			if (index == 0)
			{
				if (fileSize)
				{
					*fileSize = info->leng;
				}
				if (fileName && info->fileName)
				{
					Text::StrConcatS(fileName, info->fileName->v, fileNameBuffSize);
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
	Text::String *s;
	switch (this->reqProto)
	{
	case RequestProtocol::HTTP1_0:
	case RequestProtocol::HTTP1_1:
	case RequestProtocol::HTTP2_0:
		if (this->secureConn)
		{
			sb->AppendC(UTF8STRC("https://"));
			defPort=443;
		}
		else
		{
			sb->AppendC(UTF8STRC("http://"));
			defPort=80;
		}
		s = this->GetSHeader(UTF8STRC("Host"));
		if (s)
		{
			sb->Append(s);
		}
		if (this->svrPort != defPort)
		{
//			sb->AppendChar(':', 1);
//			sb->AppendU16(this->svrPort);
		}
		break;
	case RequestProtocol::RTSP1_0:
		sb->AppendC(UTF8STRC("rtsp://"));
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

Bool Net::WebServer::WebRequest::IsSecure()
{
	return this->secureConn;
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
	Text::String *contLeng;
	contLeng = this->GetSHeader(UTF8STRC("Content-Length"));
	if (contLeng == 0)
	{
		if (this->GetReqMethod() != Net::WebServer::IWebRequest::RequestMethod::HTTP_POST)
		{
			return false;
		}
		contLeng = this->GetSHeader(UTF8STRC("Connection"));
		if (contLeng == 0 || !contLeng->Equals(UTF8STRC("close")))
		{
			return false;
		}
		contLeng = this->GetSHeader(UTF8STRC("Transfer-Encoding"));
		if (contLeng == 0 || !contLeng->Equals(UTF8STRC("chunked")))
		{
			return false;
		}
		this->reqDataSize = (UOSInt)-1;
		return true;
	}
	this->reqDataSize = (UOSInt)contLeng->ToUInt64();
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
			NEW_CLASS(this->chunkMStm, IO::MemoryStream(UTF8STRC("Net.WebServer.WebReqeust.chunkMStm")));
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
