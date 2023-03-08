#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Cert/CurlCert.h"
#include "Data/ArrayList.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "IO/Stream.h"
#include "Net/HTTPClient.h"
#include "Net/HTTPOSClient.h"
#include "Net/SocketFactory.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextBinEnc/URIEncoding.h"
#include <curl/curl.h>

#define BUFFSIZE 2048

struct Net::HTTPOSClient::ClassData
{
	CURL *curl;
	struct curl_slist *headers;
	Data::ArrayList<Text::String*> *respHeaders;
	Text::String *userAgent;
	IO::MemoryStream *respData;
	UInt64 contLen;
	Data::ArrayList<Crypto::Cert::Certificate *> *certs;
};

size_t HTTPOSClient_HeaderFunc(char *buffer, size_t size, size_t nitems, void *userdata)
{
	Net::HTTPOSClient::ClassData *data = (Net::HTTPOSClient::ClassData*)userdata;
	UOSInt len = (UOSInt)(size * nitems);
	if (Text::StrStartsWithC((const UTF8Char*)buffer, len, UTF8STRC("HTTP/")))
	{
		return len;
	}
	Text::String *hdr = Text::String::New(len);
	MemCopyNO(hdr->v, buffer, len);
	hdr->v[len] = 0;
	UOSInt i = len;
	while (i > 0)
	{
		if (hdr->v[i - 1] == 13 || hdr->v[i - 1] == 10)
		{
			i--;
			hdr->v[i] = 0;
			hdr->leng = i;
		}
		else
		{
			break;
		}
	}
	if (hdr->StartsWithICase(UTF8STRC("Content-Length: ")))
	{
		data->contLen = Text::StrToUInt64(&hdr->v[16]);
	}
	data->respHeaders->Add(hdr);
	return len;
}

size_t HTTPOSClient_WriteFunc(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	Net::HTTPOSClient::ClassData *data = (Net::HTTPOSClient::ClassData*)userdata;
	data->respData->Write((const UInt8*)ptr, size * nmemb);
	return size * nmemb;
}

Net::HTTPOSClient::HTTPOSClient(Net::SocketFactory *sockf, Text::CString userAgent, Bool kaConn) : Net::HTTPClient(sockf, kaConn)
{
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->curl = curl_easy_init();
	this->clsData->headers = 0;
	this->clsData->respHeaders = &this->headers;
	NEW_CLASS(this->clsData->respData, IO::MemoryStream());
	this->clsData->contLen = 0x7fffffff;
	this->clsData->certs = 0;
	this->cliHost = 0;
	this->writing = false;
	this->dataBuff = 0;
	this->buffSize = 0;
//	this->timeOutMS = 5000;
	this->dataBuff = MemAlloc(UInt8, BUFFSIZE);
	NEW_CLASS(this->reqMstm, IO::MemoryStream(1024));
	if (userAgent.v == 0)
	{
		userAgent = CSTR("sswr/1.0");
	}
	this->clsData->userAgent = Text::String::New(userAgent.v, userAgent.leng);
}

Net::HTTPOSClient::~HTTPOSClient()
{
	SDEL_TEXT(this->cliHost);
	if (this->dataBuff)
	{
		MemFree(this->dataBuff);
		this->dataBuff = 0;
	}
	if (this->clsData->headers)
	{
		curl_slist_free_all(this->clsData->headers);
		this->clsData->headers = 0;
	}
	if (this->clsData->curl)
		curl_easy_cleanup(this->clsData->curl);
	this->clsData->userAgent->Release();
	if (this->clsData->certs)
	{
		UOSInt i = this->clsData->certs->GetCount();
		Crypto::Cert::Certificate *cert;
		while (i-- > 0)
		{
			cert = this->clsData->certs->GetItem(i);
			DEL_CLASS(cert);
		}
		DEL_CLASS(this->clsData->certs);
	}
	DEL_CLASS(this->clsData->respData);
	MemFree(this->clsData);
	DEL_CLASS(this->reqMstm);
}

Bool Net::HTTPOSClient::IsError() const
{
	return this->clsData->curl == 0;
}

UOSInt Net::HTTPOSClient::Read(UInt8 *buff, UOSInt size)
{
	this->EndRequest(0, 0);
	if (this->clsData->curl == 0)
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
		this->buffSize = this->clsData->respData->Read(this->dataBuff, size);
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

Bool Net::HTTPOSClient::Connect(Text::CString url, Net::WebUtil::RequestMethod method, Double *timeDNS, Double *timeConn, Bool defHeaders)
{
	UTF8Char urltmp[256];
	UTF8Char svrname[256];
	UTF8Char *svrnameEnd;

	UOSInt i;
	const UTF8Char *ptr1;
	UTF8Char *ptrs[2];
	UInt16 port;
	UInt16 defPort;
	Double t1;
	if (this->clsData->curl == 0)
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

	SDEL_STRING(this->url);
	this->url = Text::String::New(url.v, url.leng);
	if (url.StartsWith(UTF8STRC("http://")))
	{
		ptr1 = &url.v[7];
		i = Text::StrIndexOfChar(ptr1, '/');
		if (i != INVALID_INDEX)
		{
			MemCopyNO(urltmp, ptr1, i * sizeof(UTF8Char));
			urltmp[i] = 0;
		}
		else
		{
			i = url.leng - 7;
			MemCopyNO(urltmp, ptr1, i * sizeof(UTF8Char));
			urltmp[i] = 0;
		}
		Text::TextBinEnc::URIEncoding::URIDecode(urltmp, urltmp);
		defPort = 80;
	}
	else if (url.StartsWith(UTF8STRC("https://")))
	{
		ptr1 = &url.v[8];
		i = Text::StrIndexOfChar(ptr1, '/');
		if (i != INVALID_INDEX)
		{
			MemCopyNO(urltmp, ptr1, i * sizeof(UTF8Char));
			urltmp[i] = 0;
		}
		else
		{
			i = url.leng - 8;
			MemCopyNO(urltmp, ptr1, i * sizeof(UTF8Char));
			urltmp[i] = 0;
		}
		Text::TextBinEnc::URIEncoding::URIDecode(urltmp, urltmp);
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
		i = Text::StrIndexOfChar(urltmp, ']');
		if (i == INVALID_INDEX)
		{
			this->writing = true;
			this->canWrite = false;
			return false;
		}
		svrnameEnd = Text::StrConcatC(svrname, &urltmp[1], i - 1);
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
			svrnameEnd = Text::StrConcat(svrname, ptrs[0]);
		}
		else
		{
			port = defPort;
			svrnameEnd = Text::StrConcat(svrname, ptrs[0]);
		}
	}

	this->clk.Start();
	if (this->cliHost == 0)
	{
		this->cliHost = Text::StrCopyNew(urltmp);
		if (Text::StrEqualsICaseC(svrname, (UOSInt)(svrnameEnd- svrname), UTF8STRC("localhost")))
		{
			this->svrAddr.addrType = Net::AddrType::IPv4;
			WriteNUInt32(this->svrAddr.addr, Net::SocketUtil::GetIPAddr(CSTR("127.0.0.1")));
		}
		else if (!sockf->DNSResolveIP(CSTRP(svrname, svrnameEnd), &this->svrAddr))
		{
			this->writing = true;
			this->canWrite = false;
			return false;
		}
		if (timeDNS)
		{
			*timeDNS = this->clk.GetTimeDiff();
		}

		t1 = this->clk.GetTimeDiff();
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
		i = this->headers.GetCount();
		while (i-- > 0)
		{
			this->headers.RemoveAt(i)->Release();
		}
		this->headers.Clear();
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

	switch (method)
	{
		case Net::WebUtil::RequestMethod::HTTP_POST:
			this->canWrite = true;
			this->writing = false;
			break;
		case Net::WebUtil::RequestMethod::HTTP_PUT:
			this->canWrite = true;
			this->writing = false;
			curl_easy_setopt(this->clsData->curl, CURLOPT_CUSTOMREQUEST, "PUT");
			break;
		case Net::WebUtil::RequestMethod::HTTP_PATCH:
			this->canWrite = true;
			this->writing = false;
			curl_easy_setopt(this->clsData->curl, CURLOPT_CUSTOMREQUEST, UTF8STRC("PATCH"));
			break;
		case Net::WebUtil::RequestMethod::HTTP_DELETE:
			curl_easy_setopt(this->clsData->curl, CURLOPT_CUSTOMREQUEST, UTF8STRC("DELETE"));
			this->canWrite = false;
			this->writing = false;
			break;
		case Net::WebUtil::RequestMethod::Unknown:
		case Net::WebUtil::RequestMethod::HTTP_GET:
		case Net::WebUtil::RequestMethod::HTTP_CONNECT:
		case Net::WebUtil::RequestMethod::RTSP_DESCRIBE:
		case Net::WebUtil::RequestMethod::RTSP_ANNOUNCE:
		case Net::WebUtil::RequestMethod::RTSP_GET_PARAMETER:
		case Net::WebUtil::RequestMethod::RTSP_OPTIONS:
		case Net::WebUtil::RequestMethod::RTSP_PAUSE:
		case Net::WebUtil::RequestMethod::RTSP_PLAY:
		case Net::WebUtil::RequestMethod::RTSP_RECORD:
		case Net::WebUtil::RequestMethod::RTSP_REDIRECT:
		case Net::WebUtil::RequestMethod::RTSP_SETUP:
		case Net::WebUtil::RequestMethod::RTSP_SET_PARAMETER:
		case Net::WebUtil::RequestMethod::RTSP_TEARDOWN:
		default:
			this->canWrite = false;
			this->writing = false;
			break;
	}
	//curl_easy_setopt(data->curl, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(this->clsData->curl, CURLOPT_URL, url);
	curl_easy_setopt(this->clsData->curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(this->clsData->curl, CURLOPT_SSL_VERIFYHOST, 0);
	curl_easy_setopt(this->clsData->curl, CURLOPT_CERTINFO, 1L);

	this->AddHeaderC(CSTR("User-Agent"), this->clsData->userAgent->ToCString());
	if (defHeaders)
	{
		this->AddHeaderC(CSTR("Accept"), CSTR("*/*"));
		this->AddHeaderC(CSTR("Accept-Charset"), CSTR("*"));
		if (this->kaConn)
		{
			this->AddHeaderC(CSTR("Connection"), CSTR("keep-alive"));
		}
		else
		{
			this->AddHeaderC(CSTR("Connection"), CSTR("close"));
		}
	}
	return true;
}

void Net::HTTPOSClient::AddHeaderC(Text::CString name, Text::CString value)
{
	ClassData *data = this->clsData;
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
			sb.AppendC(UTF8STRC(": "));
			sb.Append(value);
			data->headers = curl_slist_append(data->headers, (const Char*)sb.ToString());
//		}
	}
}

void Net::HTTPOSClient::EndRequest(Double *timeReq, Double *timeResp)
{
	if (this->clsData->curl == 0 || (this->writing && !this->canWrite))
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

		if (this->clsData->headers)
		{
			curl_easy_setopt(this->clsData->curl, CURLOPT_HTTPHEADER, this->clsData->headers);
		}
		if (reqSize > 0)
		{
			curl_easy_setopt(this->clsData->curl, CURLOPT_POSTFIELDSIZE_LARGE, reqSize);
			curl_easy_setopt(this->clsData->curl, CURLOPT_POSTFIELDS, reqBuff);
		}
		curl_easy_setopt(this->clsData->curl, CURLOPT_HEADERFUNCTION, HTTPOSClient_HeaderFunc);
		curl_easy_setopt(this->clsData->curl, CURLOPT_HEADERDATA, this->clsData);
		curl_easy_setopt(this->clsData->curl, CURLOPT_WRITEFUNCTION, HTTPOSClient_WriteFunc);
		curl_easy_setopt(this->clsData->curl, CURLOPT_WRITEDATA, this->clsData);
		CURLcode res = curl_easy_perform(this->clsData->curl);
		this->reqMstm->Clear();
		this->clsData->respData->SeekFromBeginning(0);
		this->contLeng = this->clsData->contLen;
		t1 = this->clk.GetTimeDiff();
		if (timeReq)
		{
			*timeReq = t1;
		}

		if (res == CURLE_OK)
		{
			this->contRead = 0;

			long statusCode = 0;
			curl_easy_getinfo(this->clsData->curl, CURLINFO_RESPONSE_CODE, &statusCode);
			this->respStatus = (Net::WebStatus::StatusCode)statusCode;

		}
		else
		{
			printf("CURL error: %d\r\n", res);
		}
		t1 = this->clk.GetTimeDiff();
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

Bool Net::HTTPOSClient::IsSecureConn()
{
	if (this->url && this->url->StartsWith(UTF8STRC("https://")))
	{
		return true;
	}
	return false;
}

Bool Net::HTTPOSClient::SetClientCert(Crypto::Cert::X509Cert *cert, Crypto::Cert::X509File *key)
{
#if defined(CURLOPTTYPE_BLOB)
	if (this->clsData->curl)
	{
		struct curl_blob blob;
		blob.data = (void*)cert->GetASN1Buff();
		blob.len = (size_t)cert->GetASN1BuffSize();
		blob.flags = CURL_BLOB_COPY;
		curl_easy_setopt(this->clsData->curl, CURLOPT_SSLCERTTYPE, "DER");
		curl_easy_setopt(this->clsData->curl, CURLOPT_SSLCERT_BLOB, &blob);
		blob.data = (void*)key->GetASN1Buff();
		blob.len = (size_t)key->GetASN1BuffSize();
		blob.flags = CURL_BLOB_COPY;
		curl_easy_setopt(this->clsData->curl, CURLOPT_SSLKEYTYPE, "DER");
		curl_easy_setopt(this->clsData->curl, CURLOPT_SSLKEY_BLOB, &blob);
		return true;
	}
#endif
	return false;
}


const Data::ReadingList<Crypto::Cert::Certificate *> *Net::HTTPOSClient::GetServerCerts()
{
	if (this->IsSecureConn() && this->clsData->curl)
	{
		if (this->clsData->certs)
			return this->clsData->certs;
		struct curl_certinfo *ci;
		if (!curl_easy_getinfo(this->clsData->curl, CURLINFO_CERTINFO, &ci))
		{
			if (ci->num_of_certs > 0)
			{
				NEW_CLASS(this->clsData->certs, Data::ArrayList<Crypto::Cert::Certificate*>());
				Crypto::Cert::Certificate *cert;
				int i = 0;
				while (i < ci->num_of_certs)
				{
					NEW_CLASS(cert, Crypto::Cert::CurlCert(ci->certinfo[i]));
					this->clsData->certs->Add(cert);
					i++;
				}
				return this->clsData->certs;
			}
		}
	}
	return 0;
}
