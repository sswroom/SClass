#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/DateTime.h"
#include "IO/Stream.h"
#include "Net/HTTPClient.h"
#include "Net/HTTPOSClient.h"
#include "Net/SocketFactory.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextEnc/URIEncoding.h"
#include <curl/curl.h>

#define BUFFSIZE 2048

typedef struct
{
	CURL *curl;
	struct curl_slist *headers;
	Data::ArrayList<UTF8Char*> *respHeaders;
	const UTF8Char *userAgent;
	IO::MemoryStream *respData;
	Int64 contLen;
} ClassData;

size_t HTTPOSClient_HeaderFunc(char *buffer, size_t size, size_t nitems, void *userdata)
{
	ClassData *data = (ClassData*)userdata;
	if (Text::StrStartsWith(buffer, "HTTP/"))
	{
		return size * nitems;
	}
	UTF8Char *hdr = MemAlloc(UTF8Char, size * nitems + 1);
	MemCopyNO(hdr, buffer, size * nitems);
	hdr[size * nitems] = 0;
	OSInt i = size * nitems;
	while (i > 0)
	{
		if (hdr[i - 1] == 13 || hdr[i - 1] == 10)
		{
			i--;
			hdr[i] = 0;
		}
		else
		{
			break;
		}
	}
	if (Text::StrStartsWithICase(hdr, (const UTF8Char*)"Content-Length: "))
	{
		data->contLen = Text::StrToInt64(&hdr[16]);
	}
	data->respHeaders->Add(hdr);
	return size * nitems;
}

size_t HTTPOSClient_WriteFunc(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	ClassData *data = (ClassData*)userdata;
	data->respData->Write((const UInt8*)ptr, size * nmemb);
	return size * nmemb;
}

Net::HTTPOSClient::HTTPOSClient(Net::SocketFactory *sockf, const UTF8Char *userAgent, Bool kaConn) : Net::HTTPClient(sockf, kaConn)
{
	ClassData *data = MemAlloc(ClassData, 1);
	this->clsData = data;
	data->curl = curl_easy_init();
	data->headers = 0;
	data->respHeaders = this->headers;
	NEW_CLASS(data->respData, IO::MemoryStream((const UTF8Char*)"Net.HTTPOSClient.respData"));
	data->contLen = 0x7fffffff;
	this->cliHost = 0;
	this->writing = false;
	this->dataBuff = 0;
	this->buffSize = 0;
//	this->timeOutMS = 5000;
	this->dataBuff = MemAlloc(UInt8, BUFFSIZE);
	NEW_CLASS(this->reqMstm, IO::MemoryStream(1024, (const UTF8Char*)"Net.HTTPMyClient.reqMstm"));
	if (userAgent == 0)
	{
		userAgent = (const UTF8Char*)"sswr/1.0";
	}
	data->userAgent = Text::StrCopyNew(userAgent);
}

Net::HTTPOSClient::~HTTPOSClient()
{
	SDEL_TEXT(this->cliHost);
	if (this->dataBuff)
	{
		MemFree(this->dataBuff);
		this->dataBuff = 0;
	}
	ClassData *data = (ClassData*)this->clsData;
	if (data->headers)
	{
		curl_slist_free_all(data->headers);
		data->headers = 0;
	}
	if (data->curl)
		curl_easy_cleanup(data->curl);
	Text::StrDelNew(data->userAgent);
	DEL_CLASS(data->respData);
	MemFree(data);
	DEL_CLASS(this->reqMstm);
}

Bool Net::HTTPOSClient::IsError()
{
	ClassData *data = (ClassData*)this->clsData;
	return data->curl == 0;
}

UOSInt Net::HTTPOSClient::Read(UInt8 *buff, UOSInt size)
{
	ClassData *data = (ClassData*)this->clsData;
	this->EndRequest(0, 0);
	if (data->curl == 0)
	{
		return 0;
	}

	if (size > BUFFSIZE)
	{
		size = BUFFSIZE;
	}

	if (size > (this->contLeng - this->contRead))
	{
		if ((size = (UOSInt)(this->contLeng - this->contRead)) <= 0)
		{
			return 0;
		}
	}

	if (this->buffSize == 0)
	{
		this->buffSize = data->respData->Read(this->dataBuff, size);
	}
	if (this->buffSize >= size)
	{
		MemCopyNO(buff, this->dataBuff, size);
		MemCopyO(this->dataBuff, &this->dataBuff[size], this->buffSize - size);
		this->buffSize -= size;
		this->contRead += size;
		return size;
	}
	else
	{
		MemCopyNO(buff, this->dataBuff, this->buffSize);
		size = this->buffSize;
		this->contRead += this->buffSize;
		this->buffSize = 0;
		return size;
	}
	return 0;
}

UOSInt Net::HTTPOSClient::Write(const UInt8 *buff, UOSInt size)
{
	if (this->canWrite && !this->hasForm)
	{
		if (!this->writing)
		{
//			this->reqMstm->Write((UInt8*)"\r\n", 2);
		}
		writing = true;
		return this->reqMstm->Write(buff, size);
	}
	return 0;
}

Int32 Net::HTTPOSClient::Flush()
{
	return 0;
}

void Net::HTTPOSClient::Close()
{
/*	cli->ShutdownSend();
	cli->Close();*/
}

Bool Net::HTTPOSClient::Recover()
{
	//////////////////////////////////////////
	return false;
}

Bool Net::HTTPOSClient::Connect(const UTF8Char *url, const Char *method, Double *timeDNS, Double *timeConn, Bool defHeaders)
{
	UTF8Char urltmp[256];
	UTF8Char svrname[256];

	OSInt i;
	const UTF8Char *ptr1;
	UTF8Char *ptrs[2];
	UInt16 port;
	UInt16 defPort;
	Double t1;
	ClassData *data = (ClassData*)this->clsData;
	if (data->curl == 0)
	{
		if (timeDNS)
		{
			*timeDNS = -1;
		}
		if (timeConn)
		{
			*timeConn = -1;
		}
		return false;
	}

	SDEL_TEXT(this->url);
	this->url = Text::StrCopyNew(url);
	if (Text::StrStartsWith(url, (const UTF8Char*)"http://"))
	{
		ptr1 = &url[7];
		i = Text::StrIndexOf(ptr1, '/');
		if (i >= 0)
		{
			MemCopyNO(urltmp, ptr1, i * sizeof(UTF8Char));
			urltmp[i] = 0;
		}
		else
		{
			i = Text::StrCharCnt(ptr1);
			MemCopyNO(urltmp, ptr1, i * sizeof(UTF8Char));
			urltmp[i] = 0;
		}
		Text::TextEnc::URIEncoding::URIDecode(urltmp, urltmp);
		defPort = 80;
	}
	else if (Text::StrStartsWith(url, (const UTF8Char*)"https://"))
	{
		ptr1 = &url[8];
		i = Text::StrIndexOf(ptr1, '/');
		if (i >= 0)
		{
			MemCopyNO(urltmp, ptr1, i * sizeof(UTF8Char));
			urltmp[i] = 0;
		}
		else
		{
			i = Text::StrCharCnt(ptr1);
			MemCopyNO(urltmp, ptr1, i * sizeof(UTF8Char));
			urltmp[i] = 0;
		}
		Text::TextEnc::URIEncoding::URIDecode(urltmp, urltmp);
		defPort = 443;
	}
	else
	{
		if (timeDNS)
		{
			*timeDNS = -1;
		}
		if (timeConn)
		{
			*timeConn = -1;
		}
		return false;
	}

	if (urltmp[0] == '[')
	{
		i = Text::StrIndexOf(urltmp, ']');
		if (i < 0)
		{
			this->writing = true;
			this->canWrite = false;
			return false;
		}
		Text::StrConcatC(svrname, &urltmp[1], i - 1);
		if (urltmp[i + 1] == ':')
		{
			port = 0;
			Text::StrToUInt16(&urltmp[i + 2], &port);
			urltmp[i + 1] = 0;
		}
		else
		{
			port = defPort;
		}
	}
	else
	{
		i = Text::StrSplit(ptrs, 2, urltmp, ':');
		if (i == 2)
		{
			port = 0;
			Text::StrToUInt16(ptrs[1], &port);
			Text::StrConcat(svrname, ptrs[0]);
		}
		else
		{
			port = defPort;
			Text::StrConcat(svrname, ptrs[0]);
		}
	}

	this->clk->Start();
	if (this->cliHost == 0)
	{
		this->cliHost = Text::StrCopyNew(urltmp);
		if (Text::StrEqualsICase(svrname, (const UTF8Char*)"localhost"))
		{
			this->svrAddr.addrType = Net::SocketUtil::AT_IPV4;
			*(Int32*)this->svrAddr.addr = Net::SocketUtil::GetIPAddr((const UTF8Char*)"127.0.0.1");
		}
		else if (!sockf->DNSResolveIP(svrname, &this->svrAddr))
		{
			this->writing = true;
			this->canWrite = false;
			return false;
		}
		if (timeDNS)
		{
			*timeDNS = clk->GetTimeDiff();
		}

		t1 = clk->GetTimeDiff();
		if (timeConn)
		{
			*timeConn = t1;
		}
//		this->sockf->SetLinger(cli->GetSocket(), 0);
//		this->sockf->SetNoDelay(cli->GetSocket(), true);
	}
	else if (Text::StrEquals(this->cliHost, urltmp))
	{
		if (this->buffSize > 0)
		{
			this->contRead += this->buffSize;
			this->buffSize = 0;
		}
		if (timeDNS)
		{
			*timeDNS = 0;
		}
		if (timeConn)
		{
			*timeConn = 0;
		}
		this->contRead = 0;
		i = this->headers->GetCount();
		while (i-- > 0)
		{
			MemFree(this->headers->RemoveAt(i));
		}
		this->headers->Clear();
	}
	else
	{
		if (timeDNS)
		{
			*timeDNS = -1;
		}
		if (timeConn)
		{
			*timeConn = -1;
		}
		return false;
	}

	if (method && (Text::StrEqualsICase(method, "POST") || Text::StrEqualsICase(method, "PUT") || Text::StrEqualsICase(method, "PATCH")))
	{
		this->canWrite = true;
		this->writing = false;
	}
	else
	{
		this->canWrite = false;
		this->writing = false;
	}
	if (method)
	{
		if (Text::StrEqualsICase(method, "POST"))
		{

		}
		else if (Text::StrEqualsICase(method, "PUT"))
		{
			curl_easy_setopt(data->curl, CURLOPT_CUSTOMREQUEST, "PUT");
		}
		else if (Text::StrEqualsICase(method, "PATCH"))
		{
			curl_easy_setopt(data->curl, CURLOPT_CUSTOMREQUEST, "PATCH");
		}
		else if (Text::StrEqualsICase(method, "DELETE"))
		{
			curl_easy_setopt(data->curl, CURLOPT_CUSTOMREQUEST, "DELETE");
		}
	}
	//curl_easy_setopt(data->curl, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(data->curl, CURLOPT_URL, url);
	curl_easy_setopt(data->curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(data->curl, CURLOPT_SSL_VERIFYHOST, 0);

	this->AddHeader((const UTF8Char*)"User-Agent", data->userAgent);
	if (defHeaders)
	{
		this->AddHeader((const UTF8Char*)"Accept", (const UTF8Char*)"*/*");
		this->AddHeader((const UTF8Char*)"Accept-Charset", (const UTF8Char*)"*");
		if (this->kaConn)
		{
			this->AddHeader((const UTF8Char*)"Connection", (const UTF8Char*)"keep-alive");
		}
		else
		{
			this->AddHeader((const UTF8Char*)"Connection", (const UTF8Char*)"close");
		}
	}
	return true;
}

void Net::HTTPOSClient::AddHeader(const UTF8Char *name, const UTF8Char *value)
{
	ClassData *data = (ClassData*)this->clsData;
	if (data->curl && !writing)
	{
/*		if (Text::StrEquals(name, (const UTF8Char*)"User-Agent"))
		{
		}
		else if (Text::StrEquals(name, (const UTF8Char*)"Connection"))
		{
		}
		else
		{*/
			Text::StringBuilderUTF8 sb;
			sb.Append(name);
			sb.Append((const UTF8Char*)": ");
			sb.Append(value);
			data->headers = curl_slist_append(data->headers, (const Char*)sb.ToString());
//		}
	}
}

void Net::HTTPOSClient::EndRequest(Double *timeReq, Double *timeResp)
{
	ClassData *data = (ClassData*)this->clsData;
	if (data->curl == 0 || (this->writing && !this->canWrite))
	{
		if (timeReq)
		{
			*timeReq = -1;
		}
		if (timeResp)
		{
			*timeResp = -1;
		}
		return;
	}
	else
	{
		Double t1;
		if (this->hasForm)
		{
			UOSInt len = this->formSb->GetLength();
			this->AddContentLength(len);
			this->hasForm = false;
			this->Write((UInt8*)this->formSb->ToString(), len);
			DEL_CLASS(this->formSb);
			this->formSb = 0;
		}
		this->canWrite = false;
		this->writing = true;

		UOSInt reqSize;
		UInt8 *reqBuff = this->reqMstm->GetBuff(&reqSize);

		if (data->headers)
		{
			curl_easy_setopt(data->curl, CURLOPT_HTTPHEADER, data->headers);
		}
		if (reqSize > 0)
		{
			curl_easy_setopt(data->curl, CURLOPT_POSTFIELDSIZE_LARGE, reqSize);
			curl_easy_setopt(data->curl, CURLOPT_POSTFIELDS, reqBuff);
		}
		curl_easy_setopt(data->curl, CURLOPT_HEADERFUNCTION, HTTPOSClient_HeaderFunc);
		curl_easy_setopt(data->curl, CURLOPT_HEADERDATA, data);
		curl_easy_setopt(data->curl, CURLOPT_WRITEFUNCTION, HTTPOSClient_WriteFunc);
		curl_easy_setopt(data->curl, CURLOPT_WRITEDATA, data);
		CURLcode res = curl_easy_perform(data->curl);
		this->reqMstm->Clear();
		data->respData->Seek(IO::SeekableStream::ST_BEGIN, 0);
		this->contLeng = data->contLen;
		t1 = this->clk->GetTimeDiff();
		if (timeReq)
		{
			*timeReq = t1;
		}

		if (res == CURLE_OK)
		{
			this->contRead = 0;

			long statusCode = 0;
			curl_easy_getinfo(data->curl, CURLINFO_RESPONSE_CODE, &statusCode);
			this->respStatus = (Net::WebStatus::StatusCode)statusCode;

		}
		else
		{
			printf("CURL error: %d\r\n", res);
		}
		t1 = this->clk->GetTimeDiff();
		if (timeResp)
		{
			*timeResp = t1;
		}
	}
}

void Net::HTTPOSClient::SetTimeout(Int32 ms)
{
//	if (this->cli)
//		this->cli->SetTimeout(this->timeOutMS = ms);
}
