#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/SSLClient.h"
#include "Net/WebServer/WebRequest.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextBinEnc/URIEncoding.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

#define MAX_DATA_SIZE 104857600

void Net::WebServer::WebRequest::ParseQuery()
{
	UnsafeArray<UTF8Char> sbuff;
	UnsafeArray<UTF8Char> sbuff2;
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> url = this->GetRequestURI();
	UOSInt urlLen = url->leng;
	Text::PString strs1[2];
	Text::PString strs2[2];
	NN<Text::String> s;
	Optional<Text::String> opts;

	sbuff = MemAllocArr(UTF8Char, urlLen);
	NEW_CLASS(this->queryMap, Data::FastStringMapNN<Text::String>());
	if (this->GetQueryString(sbuff, urlLen).SetTo(sptr))
	{
		sbuff2 = MemAllocArr(UTF8Char, urlLen);
		UOSInt scnt;
		Bool hasMore;
		strs1[1].v = sbuff;
		strs1[1].leng = (UOSInt)(sptr - sbuff);
		hasMore = true;
		while (hasMore)
		{
			hasMore = (Text::StrSplitP(strs1, 2, strs1[1], '&') == 2);

			scnt = Text::StrSplitP(strs2, 2, strs1[0], '=');
			if (scnt == 2)
			{
				sptr = Text::TextBinEnc::URIEncoding::URIDecode(sbuff2, strs2[1].v);
			}
			else
			{
				sbuff2[0] = 0;
				sptr = sbuff2;
			}
			if (this->queryMap->GetC(strs2[0].ToCString()).SetTo(s))
			{
				Text::StringBuilderUTF8 sb;
				sb.Append(s);
				sb.AppendChar(PARAM_SEPERATOR, 1);
				sb.AppendC(sbuff2, (UOSInt)(sptr - sbuff2));
				opts = this->queryMap->PutC(strs2[0].ToCString(), Text::String::New(sb.ToString(), sb.GetLength()));
			}
			else
			{
				opts = this->queryMap->PutC(strs2[0].ToCString(), Text::String::New(sbuff2, (UOSInt)(sptr - sbuff2)));
			}
			if (opts.SetTo(s))
			{
				s->Release();
			}
		}
		MemFreeArr(sbuff2);
	}
	MemFreeArr(sbuff);
}

void Net::WebServer::WebRequest::ParseFormStr(NN<Data::FastStringMapNN<Text::String>> formMap, const UInt8 *buff, UOSInt buffSize)
{
	UInt8 *tmpBuff;
	UInt8 b;
	UInt8 b2;
	NN<Text::String> tmpStr;
	UnsafeArrayOpt<UTF8Char> tmpName;
	UnsafeArray<UTF8Char> nns;
	UOSInt tmpNameLen;
	UOSInt nameLen = 0;
	NN<Text::String> s;
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
			if (tmpName.SetTo(nns))
			{
				tmpBuff[buffPos] = 0;
#if defined(VERBOSE)
				printf("WebRequest: Form: %s = %s\r\n", tmpBuff, nns.Ptr());
#endif
				if (formMap->GetC({tmpBuff, nameLen}).SetTo(s))
				{
					charCnt = s->leng + 1;
					tmpNameLen = (UOSInt)(&tmpBuff[buffPos] - nns);
					charCnt += tmpNameLen;
					tmpStr = Text::String::New(charCnt);
					Text::StrConcatC(Text::StrConcatC(s->ConcatTo(tmpStr->v), UTF8STRC(",")), nns, tmpNameLen);
					formMap->PutC({tmpBuff, nameLen}, tmpStr);
					s->Release();
				}
				else
				{
					formMap->PutC({tmpBuff, nameLen}, Text::String::New(nns, (UOSInt)(&tmpBuff[buffPos] - nns)));
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

	if (tmpName.SetTo(nns))
	{
		tmpBuff[buffPos] = 0;
#if defined(VERBOSE)
		printf("WebRequest: Form: %s = %s\r\n", tmpBuff, nns.Ptr());
#endif
		if (formMap->GetC({tmpBuff, nameLen}).SetTo(s))
		{
			charCnt = s->leng + 1;
			tmpNameLen = (UOSInt)(&tmpBuff[buffPos] - nns);
			charCnt += tmpNameLen;
			tmpStr = Text::String::New(charCnt);
			Text::StrConcatC(Text::StrConcatC(s->ConcatTo(tmpStr->v), UTF8STRC(",")), nns, tmpNameLen);
			formMap->PutC({tmpBuff, nameLen}, tmpStr);
			s->Release();
		}
		else
		{
			formMap->PutC({tmpBuff, nameLen}, Text::String::New(nns, (UOSInt)(&tmpBuff[buffPos] - nns)));
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
	Text::CString formName = CSTR_NULL;
	Text::CString fileName = CSTR_NULL;
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
				UnsafeArray<UTF8Char> line;
				Text::PString lineStrs[10];
				UOSInt strCnt;
				contType = 1;

				line = MemAllocArr(UTF8Char, i - lineStart - 30);
				MemCopyNO(line.Ptr(), &data[lineStart + 31], i - lineStart - 31);
				line[i - lineStart - 31] = 0;

				strCnt = Text::StrSplitTrimP(lineStrs, 10, {line, i - lineStart - 31}, ';');
				j = strCnt;
				while (j-- > 0)
				{
					if (Text::StrStartsWithC(lineStrs[j].v, lineStrs[j].leng, UTF8STRC("name=")))
					{
						SDEL_TEXT(formName.v);
						formName = ParseHeaderVal(&lineStrs[j].v[5], lineStrs[j].leng - 5);
					}
					else if (Text::StrStartsWithC(lineStrs[j].v, lineStrs[j].leng, UTF8STRC("filename=")))
					{
						SDEL_TEXT(fileName.v);
						contType = 2;
						fileName = ParseHeaderVal(&lineStrs[j].v[9], lineStrs[j].leng - 9);
					}
				}
				MemFreeArr(line);
			}
			i++;
			lineStart = i + 1;
		}
		i++;
	}

	Text::CStringNN formNameNN;
	Text::CStringNN fileNameNN;
	if (contType == 1)
	{
		NN<Text::String> s;
		if (formName.SetTo(formNameNN) && dataSize >= i)
		{
			if (this->formMap->GetC(formNameNN).SetTo(s))
			{
				s->Release();
				formMap->PutC(formNameNN, Text::String::New(&data[i], dataSize - i));
			}
			else
			{
				formMap->PutC(formNameNN, Text::String::New(&data[i], dataSize - i));
			}
		}
	}
	else if (contType == 2)
	{
		if (formName.SetTo(formNameNN) && fileName.SetTo(fileNameNN))
		{
			NN<FormFileInfo> info = MemAllocNN(FormFileInfo);
			info->ofst = startOfst + i;
			info->leng = dataSize - i;
			info->formName = Text::String::New(formNameNN);
			info->fileName = Text::String::New(fileNameNN);
			this->formFileList->Add(info);
		}
	}
	SDEL_TEXT(formName.v);
	SDEL_TEXT(fileName.v);
}

Text::CStringNN Net::WebServer::WebRequest::ParseHeaderVal(UnsafeArray<UTF8Char> headerData, UOSInt dataLen)
{
	UnsafeArray<UTF8Char> outStr;
	if (headerData[0] == '"' && headerData[dataLen-1] == '"')
	{
		outStr = MemAllocArr(UTF8Char, dataLen - 1);
		MemCopyNO(outStr.Ptr(), &headerData[1], dataLen - 2);
		outStr[dataLen - 2] = 0;
		dataLen -= 2;
	}
	else
	{
		outStr = MemAllocArr(UTF8Char, dataLen + 1);
		MemCopyNO(outStr.Ptr(), &headerData[1], dataLen);
		outStr[dataLen] = 0;
	}
	return {outStr, dataLen};
}

Net::WebServer::WebRequest::WebRequest(Text::CStringNN requestURI, Net::WebUtil::RequestMethod reqMeth, RequestProtocol reqProto, NN<Net::TCPClient> cli, const Net::SocketUtil::AddressInfo *cliAddr, UInt16 cliPort, UInt16 svrPort)
{
	this->requestURI = Text::String::New(requestURI);
	this->reqMeth = reqMeth;
	this->reqProto = reqProto;
	this->cli = cli;
	this->cliAddr = *cliAddr;
	this->cliPort = cliPort;
	this->svrPort = svrPort;
	this->queryMap = 0;
	this->formMap = 0;
	this->formFileList = 0;
	this->reqData = 0;
	this->reqDataSize = 0;
	this->chunkMStm = 0;
	this->remoteCert = 0;
}

Net::WebServer::WebRequest::~WebRequest()
{
	UOSInt i;
	this->requestURI->Release();
	NNLIST_FREE_STRING(&this->headers);
	if (this->queryMap)
	{
		NNLIST_FREE_STRING(this->queryMap);
		DEL_CLASS(this->queryMap);
	}
	if (this->formMap)
	{
		NNLIST_FREE_STRING(this->formMap);
		DEL_CLASS(this->formMap);
	}
	if (this->formFileList)
	{
		NN<FormFileInfo> fileInfo;
		i = this->formFileList->GetCount();
		while (i-- > 0)
		{
			fileInfo = this->formFileList->GetItemNoCheck(i);
			fileInfo->formName->Release();
			fileInfo->fileName->Release();
			MemFreeNN(fileInfo);
		}
		DEL_CLASS(this->formFileList);
	}
	if (this->reqData)
	{
		MemFree(this->reqData);
		this->reqData = 0;
	}
	SDEL_CLASS(this->chunkMStm);
	this->remoteCert.Delete();
}

void Net::WebServer::WebRequest::AddHeader(Text::CStringNN name, Text::CStringNN value)
{
	Optional<Text::String> s = this->headers.PutC(name, Text::String::New(value));
	OPTSTR_DEL(s);
}

Optional<Text::String> Net::WebServer::WebRequest::GetSHeader(Text::CStringNN name) const
{
	return this->headers.GetC(name);
}

UnsafeArrayOpt<UTF8Char> Net::WebServer::WebRequest::GetHeader(UnsafeArray<UTF8Char> sbuff, Text::CStringNN name, UOSInt buffLen) const
{
	NN<Text::String> s;
	if (this->headers.GetC(name).SetTo(s))
	{
		return s->ConcatToS(sbuff, buffLen);
	}
	else
	{
		return 0;
	}
}

Bool Net::WebServer::WebRequest::GetHeaderC(NN<Text::StringBuilderUTF8> sb, Text::CStringNN name) const
{
	NN<Text::String> hdr;
	if (!this->headers.GetC(name).SetTo(hdr))
		return false;
	sb->Append(hdr);
	return true;
}

UOSInt Net::WebServer::WebRequest::GetHeaderNames(NN<Data::ArrayListStringNN> names) const
{
	NN<Text::String> s;
	UOSInt i = 0;
	UOSInt j = this->headers.GetCount();
	names->EnsureCapacity(j);
	while (i < j)
	{
		if (this->headers.GetKey(i).SetTo(s))
			names->Add(s);
		i++;
	}
	return j;
}

UOSInt Net::WebServer::WebRequest::GetHeaderCnt() const
{
	return this->headers.GetCount();
}

Optional<Text::String> Net::WebServer::WebRequest::GetHeaderName(UOSInt index) const
{
	return this->headers.GetKey(index);
}

Optional<Text::String> Net::WebServer::WebRequest::GetHeaderValue(UOSInt index) const
{
	return this->headers.GetItem(index);
}

NN<Text::String> Net::WebServer::WebRequest::GetRequestURI() const
{
	return this->requestURI;
}

Net::WebServer::IWebRequest::RequestProtocol Net::WebServer::WebRequest::GetProtocol() const
{
	return this->reqProto;
}

Optional<Text::String> Net::WebServer::WebRequest::GetQueryValue(Text::CStringNN name)
{
	if (this->queryMap == 0)
	{
		this->ParseQuery();
	}
	return this->queryMap->GetC(name);
}

Bool Net::WebServer::WebRequest::HasQuery(Text::CStringNN name)
{
	if (this->queryMap == 0)
	{
		this->ParseQuery();
	}
	return this->queryMap->GetC(name).NotNull();
}

Net::WebUtil::RequestMethod Net::WebServer::WebRequest::GetReqMethod() const
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

	Text::StringBuilderUTF8 sb;
	if (this->GetHeaderC(sb, CSTR("Content-Type")))
	{
		if (sb.StartsWith(UTF8STRC("application/x-www-form-urlencoded")))
		{
			NN<Data::FastStringMapNN<Text::String>> formMap;
			NEW_CLASSNN(formMap, Data::FastStringMapNN<Text::String>());
			this->formMap = formMap.Ptr();
			ParseFormStr(formMap, this->reqData, this->reqDataSize);
		}
		else if (sb.StartsWith(UTF8STRC("multipart/form-data")))
		{
			UnsafeArray<UTF8Char> sptr = sb.v;
			UOSInt i = Text::StrIndexOfC(sptr, sb.GetLength(), UTF8STRC("boundary="));
			if (i != INVALID_INDEX)
			{
				UInt8 *boundary = &sptr[i + 9];
				UOSInt boundSize = sb.GetLength() - i - 9;
				NEW_CLASS(this->formMap, Data::FastStringMapNN<Text::String>());
				NEW_CLASS(this->formFileList, Data::ArrayListNN<FormFileInfo>());

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
}

Optional<Text::String> Net::WebServer::WebRequest::GetHTTPFormStr(Text::CStringNN name)
{
	if (this->formMap == 0)
		return 0;
	return this->formMap->GetC(name);
}

UnsafeArrayOpt<const UInt8> Net::WebServer::WebRequest::GetHTTPFormFile(Text::CStringNN formName, UOSInt index, UnsafeArrayOpt<UTF8Char> fileName, UOSInt fileNameBuffSize, OptOut<UnsafeArray<UTF8Char>> fileNameEnd, OptOut<UOSInt> fileSize)
{
	if (this->formFileList == 0)
		return 0;
	UOSInt i = 0;
	UOSInt j = this->formFileList->GetCount();
	while (i < j)
	{
		NN<FormFileInfo> info = this->formFileList->GetItemNoCheck(i);
		if (info->formName->Equals(formName.v, formName.leng))
		{
			if (index == 0)
			{
				fileSize.Set(info->leng);
				UnsafeArray<UTF8Char> nnfileName;
				if (fileName.SetTo(nnfileName))
				{
					fileNameEnd.Set(Text::StrConcatCS(nnfileName, info->fileName->v, info->fileName->leng, fileNameBuffSize));
				}
				return &this->reqData[info->ofst];
			}
			index--;
		}
		i++;
	}
	return 0;
}

void Net::WebServer::WebRequest::GetRequestURLBase(NN<Text::StringBuilderUTF8> sb)
{
	UInt16 defPort;
	NN<Text::String> s;
	switch (this->reqProto)
	{
	case RequestProtocol::HTTP1_0:
	case RequestProtocol::HTTP1_1:
	case RequestProtocol::HTTP2_0:
		if (this->cli->IsSSL())
		{
			sb->AppendC(UTF8STRC("https://"));
			defPort=443;
		}
		else
		{
			sb->AppendC(UTF8STRC("http://"));
			defPort=80;
		}
		if (this->GetSHeader(CSTR("Host")).SetTo(s))
		{
			sb->Append(s);
		}
		if (this->svrPort != defPort)
		{
//			sb->AppendUTF8Char(':');
//			sb->AppendU16(this->svrPort);
		}
		break;
	case RequestProtocol::RTSP1_0:
		sb->AppendC(UTF8STRC("rtsp://"));
		defPort=554;
		break;
	}
}

NN<const Net::SocketUtil::AddressInfo> Net::WebServer::WebRequest::GetClientAddr() const
{
	return this->cliAddr;
}

NN<Net::NetConnection> Net::WebServer::WebRequest::GetNetConn() const
{
	return this->cli;
}

UInt16 Net::WebServer::WebRequest::GetClientPort() const
{
	return this->cliPort;
}

Bool Net::WebServer::WebRequest::IsSecure() const
{
	return this->cli->IsSSL();
}

Optional<Crypto::Cert::X509Cert> Net::WebServer::WebRequest::GetClientCert()
{
	if (this->remoteCert.NotNull())
	{
		return this->remoteCert;
	}
	if (!this->cli->IsSSL())
	{
		return 0;
	}
	Net::SSLClient *cli = (Net::SSLClient*)this->cli.Ptr();
	NN<Crypto::Cert::Certificate> cert;
	if (cli->GetRemoteCert().SetTo(cert))
	{
		this->remoteCert = cert->CreateX509Cert();
		return this->remoteCert;
	}
	return 0;
}

UnsafeArrayOpt<const UInt8> Net::WebServer::WebRequest::GetReqData(OutParam<UOSInt> dataSize)
{
	if (this->reqData == 0)
	{
		dataSize.Set(0);
		return 0;
	}
	else
	{
		dataSize.Set(this->reqCurrSize);
		return this->reqData;
	}
}

Bool Net::WebServer::WebRequest::HasData()
{
	NN<Text::String> contLeng;
	if (!this->GetSHeader(CSTR("Content-Length")).SetTo(contLeng))
	{
		if (this->GetReqMethod() != Net::WebUtil::RequestMethod::HTTP_POST)
		{
			return false;
		}
		if (!this->GetSHeader(CSTR("Connection")).SetTo(contLeng) || !contLeng->Equals(UTF8STRC("close")))
		{
			return false;
		}
		if (!this->GetSHeader(CSTR("Transfer-Encoding")).SetTo(contLeng) || !contLeng->Equals(UTF8STRC("chunked")))
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
			NEW_CLASS(this->chunkMStm, IO::MemoryStream());
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
				UnsafeArray<UInt8> buff = this->chunkMStm->GetBuff(this->reqDataSize);
				MemFree(this->reqData);
				this->reqData = MemAlloc(UInt8, this->reqDataSize);
				MemCopyNO(this->reqData, buff.Ptr(), this->reqDataSize);
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
							this->chunkMStm->Write(Data::ByteArrayR(&this->reqData[j + 2], leng));
							i = j + 4 + leng;
						}
						else
						{
							UnsafeArray<UInt8> buff = this->chunkMStm->GetBuff(this->reqDataSize);
							MemFree(this->reqData);
							this->reqData = MemAlloc(UInt8, this->reqDataSize);
							MemCopyNO(this->reqData, buff.Ptr(), this->reqDataSize);
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
			if (j == 0 || j + 2 > this->reqCurrSize)
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
