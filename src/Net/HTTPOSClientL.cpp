#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Cert/CurlCert.h"
#include "Data/ArrayList.hpp"
#include "Core/ByteTool_C.h"
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
//#define VERBOSE

struct Net::HTTPOSClient::ClassData
{
	CURL *curl;
	struct curl_slist *headers;
	Data::ArrayListStringNN *respHeaders;
	NN<Text::String> userAgent;
	IO::MemoryStream *respData;
	UInt64 contLen;
	Optional<Data::ArrayListNN<Crypto::Cert::Certificate>> certs;
};

size_t HTTPOSClient_HeaderFunc(char *buffer, size_t size, size_t nitems, void *userdata)
{
	Net::HTTPOSClient::ClassData *data = (Net::HTTPOSClient::ClassData*)userdata;
	UOSInt len = (UOSInt)(size * nitems);
	if (Text::StrStartsWithC((const UTF8Char*)buffer, len, UTF8STRC("HTTP/")))
	{
		return len;
	}
	NN<Text::String> hdr = Text::String::New(len);
	MemCopyNO(hdr->v.Ptr(), buffer, len);
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
	data->respData->Write(Data::ByteArrayR((const UInt8*)ptr, size * nmemb));
	return size * nmemb;
}

Net::HTTPOSClient::HTTPOSClient(NN<Net::TCPClientFactory> clif, Text::CString userAgent, Bool kaConn) : Net::HTTPClient(clif, kaConn)
{
	this->clsData = MemAllocNN(ClassData);
	this->clsData->curl = curl_easy_init();
#if defined(VERBOSE)
	curl_easy_setopt(this->clsData->curl, CURLOPT_VERBOSE, 1);
#endif
	this->clsData->headers = 0;
	this->clsData->respHeaders = &this->headers;
	NEW_CLASS(this->clsData->respData, IO::MemoryStream());
	this->clsData->contLen = 0x7fffffff;
	this->clsData->certs = 0;
	this->cliHost = 0;
	this->writing = false;
	this->buffSize = 0;
//	this->timeOutMS = 5000;
	this->dataBuff = MemAllocArr(UInt8, BUFFSIZE);
	NEW_CLASSNN(this->reqMstm, IO::MemoryStream(1024));
	Text::CStringNN nnuserAgent;
	if (!userAgent.SetTo(nnuserAgent))
	{
		nnuserAgent = CSTR("sswr/1.0");
	}
	this->clsData->userAgent = Text::String::New(nnuserAgent);
}

Net::HTTPOSClient::~HTTPOSClient()
{
	SDEL_TEXT(this->cliHost);
	MemFreeArr(this->dataBuff);
	if (this->clsData->headers)
	{
		curl_slist_free_all(this->clsData->headers);
		this->clsData->headers = 0;
	}
	if (this->clsData->curl)
		curl_easy_cleanup(this->clsData->curl);
	this->clsData->userAgent->Release();
	NN<Data::ArrayListNN<Crypto::Cert::Certificate>> certList;
	if (this->clsData->certs.SetTo(certList))
	{
		UOSInt i = certList->GetCount();
		NN<Crypto::Cert::Certificate> cert;
		while (i-- > 0)
		{
			cert = certList->GetItemNoCheck(i);
			cert.Delete();
		}
		certList.Delete();
	}
	DEL_CLASS(this->clsData->respData);
	MemFreeNN(this->clsData);
	this->reqMstm.Delete();
}

Bool Net::HTTPOSClient::IsError() const
{
	return this->clsData->curl == 0;
}

UOSInt Net::HTTPOSClient::Read(const Data::ByteArray &buff)
{
	this->EndRequest(0, 0);
	if (this->clsData->curl == 0)
	{
		return 0;
	}

	Data::ByteArray myBuff = buff;
	if (myBuff.GetSize() > BUFFSIZE)
	{
		myBuff = myBuff.WithSize(BUFFSIZE);
	}

	if (myBuff.GetSize() > (this->contLeng - this->contRead))
	{
		myBuff = myBuff.WithSize((UOSInt)(this->contLeng - this->contRead));
		if (myBuff.GetSize() <= 0)
		{
			return 0;
		}
	}

	if (this->buffSize == 0)
	{
		this->buffSize = this->clsData->respData->Read(Data::ByteArray(this->dataBuff, myBuff.GetSize()));
	}
	if (this->buffSize >= myBuff.GetSize())
	{
		myBuff.CopyFrom(Data::ByteArray(this->dataBuff, myBuff.GetSize()));
		MemCopyO(&this->dataBuff[0], &this->dataBuff[myBuff.GetSize()], this->buffSize - myBuff.GetSize());
		this->buffSize -= myBuff.GetSize();
		this->contRead += myBuff.GetSize();
		return myBuff.GetSize();
	}
	else
	{
		myBuff.CopyFrom(Data::ByteArray(this->dataBuff, this->buffSize));
		UOSInt size = this->buffSize;
		this->contRead += this->buffSize;
		this->buffSize = 0;
		return size;
	}
	return 0;
}

UOSInt Net::HTTPOSClient::Write(Data::ByteArrayR buff)
{
	if (this->canWrite && this->sbForm.IsNull())
	{
		if (!this->writing)
		{
//			this->reqMstm->Write((UInt8*)"\r\n", 2);
		}
		writing = true;
		return this->reqMstm->Write(buff);
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

Bool Net::HTTPOSClient::Connect(Text::CStringNN url, Net::WebUtil::RequestMethod method, OptOut<Double> timeDNS, OptOut<Double> timeConn, Bool defHeaders)
{
	UTF8Char urltmp[256];
	UTF8Char svrname[256];
	UnsafeArray<UTF8Char> svrnameEnd;

	UOSInt i;
	UnsafeArray<const UTF8Char> ptr1;
	UnsafeArray<UTF8Char> ptrs[2];
	UInt16 port;
	UInt16 defPort;
	Double t1;
	if (this->clsData->curl == 0)
	{
		timeDNS.Set(-1);
		timeConn.Set(-1);
		return false;
	}

	this->url->Release();
	this->url = Text::String::New(url);
	this->SetSourceName(this->url);
	if (url.StartsWith(UTF8STRC("http://")))
	{
		ptr1 = &url.v[7];
		i = Text::StrIndexOfChar(ptr1, '/');
		if (i != INVALID_INDEX)
		{
			MemCopyNO(urltmp, ptr1.Ptr(), i * sizeof(UTF8Char));
			urltmp[i] = 0;
		}
		else
		{
			i = url.leng - 7;
			MemCopyNO(urltmp, ptr1.Ptr(), i * sizeof(UTF8Char));
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
			MemCopyNO(urltmp, ptr1.Ptr(), i * sizeof(UTF8Char));
			urltmp[i] = 0;
		}
		else
		{
			i = url.leng - 8;
			MemCopyNO(urltmp, ptr1.Ptr(), i * sizeof(UTF8Char));
			urltmp[i] = 0;
		}
		Text::TextBinEnc::URIEncoding::URIDecode(urltmp, urltmp);
		defPort = 443;
	}
	else
	{
		timeDNS.Set(-1);
		timeConn.Set(-1);
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
			Text::StrToUInt16(&urltmp[i + 2], port);
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
			Text::StrToUInt16(ptrs[1], port);
			svrnameEnd = Text::StrConcat(svrname, ptrs[0]);
		}
		else
		{
			port = defPort;
			svrnameEnd = Text::StrConcat(svrname, ptrs[0]);
		}
	}

	this->clk.Start();
	UnsafeArray<const UTF8Char> nns;
	if (!this->cliHost.SetTo(nns))
	{
		this->cliHost = Text::StrCopyNew(urltmp);
		if (Text::StrEqualsICaseC(svrname, (UOSInt)(svrnameEnd- svrname), UTF8STRC("localhost")))
		{
			this->svrAddr.addrType = Net::AddrType::IPv4;
			WriteNUInt32(this->svrAddr.addr, Net::SocketUtil::GetIPAddr(CSTR("127.0.0.1")));
		}
		else if (!clif->GetSocketFactory()->DNSResolveIP(CSTRP(svrname, svrnameEnd), this->svrAddr))
		{
			this->writing = true;
			this->canWrite = false;
			return false;
		}
		timeDNS.Set(this->clk.GetTimeDiff());
		t1 = this->clk.GetTimeDiff();
		timeConn.Set(t1);
//		this->sockf->SetLinger(cli->GetSocket(), 0);
//		this->sockf->SetNoDelay(cli->GetSocket(), true);
	}
	else if (Text::StrEquals(nns, urltmp))
	{
		if (this->buffSize > 0)
		{
			this->contRead += this->buffSize;
			this->buffSize = 0;
		}
		timeDNS.Set(0);
		timeConn.Set(0);
		this->contRead = 0;
		i = this->headers.GetCount();
		while (i-- > 0)
		{
			OPTSTR_DEL(this->headers.RemoveAt(i));
		}
		this->headers.Clear();
	}
	else
	{
		timeDNS.Set(-1);
		timeConn.Set(-1);
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
		case Net::WebUtil::RequestMethod::HTTP_HEAD:
			curl_easy_setopt(this->clsData->curl, CURLOPT_CUSTOMREQUEST, UTF8STRC("HEAD"));
			this->canWrite = false;
			this->writing = false;
			break;
		case Net::WebUtil::RequestMethod::HTTP_TRACE:
			curl_easy_setopt(this->clsData->curl, CURLOPT_CUSTOMREQUEST, UTF8STRC("TRACE"));
			this->canWrite = false;
			this->writing = false;
			break;
		case Net::WebUtil::RequestMethod::Unknown:
		case Net::WebUtil::RequestMethod::HTTP_GET:
		case Net::WebUtil::RequestMethod::HTTP_CONNECT:
		case Net::WebUtil::RequestMethod::HTTP_OPTIONS:
		case Net::WebUtil::RequestMethod::RTSP_DESCRIBE:
		case Net::WebUtil::RequestMethod::RTSP_ANNOUNCE:
		case Net::WebUtil::RequestMethod::RTSP_GET_PARAMETER:
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
	curl_easy_setopt(this->clsData->curl, CURLOPT_URL, url);
	curl_easy_setopt(this->clsData->curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(this->clsData->curl, CURLOPT_SSL_VERIFYHOST, 0);
	curl_easy_setopt(this->clsData->curl, CURLOPT_CERTINFO, 1L);

	this->AddHeaderC(CSTR("User-Agent"), this->clsData->userAgent->ToCString());
	if (defHeaders)
	{
		this->AddHeaderC(CSTR("Accept"), CSTR("*/*"));
		this->AddHeaderC(CSTR("Accept-Language"), CSTR("*"));
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

void Net::HTTPOSClient::AddHeaderC(Text::CStringNN name, Text::CString value)
{
	NN<ClassData> data = this->clsData;
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
			sb.AppendOpt(value);
			data->headers = curl_slist_append(data->headers, (const Char*)sb.ToPtr());
//		}
	}
}

void Net::HTTPOSClient::EndRequest(OptOut<Double> timeReq, OptOut<Double> timeResp)
{
	if (this->clsData->curl == 0 || (this->writing && !this->canWrite))
	{
		timeReq.Set(-1);
		timeResp.Set(-1);
		return;
	}
	else
	{
		Double t1;
		NN<Text::StringBuilderUTF8> sbForm;
		if (this->sbForm.SetTo(sbForm))
		{
			UOSInt len = sbForm->GetLength();
			this->AddContentLength(len);
			this->sbForm = 0;
			this->Write(sbForm->ToByteArray());
			sbForm.Delete();
		}
		this->canWrite = false;
		this->writing = true;

		UOSInt reqSize;
		UnsafeArray<UInt8> reqBuff = this->reqMstm->GetBuff(reqSize);

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
		timeReq.Set(t1);

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
		timeResp.Set(t1);
	}
}

void Net::HTTPOSClient::SetTimeout(Data::Duration timeout)
{
	if (this->clsData->curl)
	{
		curl_easy_setopt(this->clsData->curl, CURLOPT_TIMEOUT_MS, (long)timeout.GetTotalMS());
	}
}

Bool Net::HTTPOSClient::IsSecureConn() const
{
	if (this->url->StartsWith(UTF8STRC("https://")))
	{
		return true;
	}
	return false;
}

Bool Net::HTTPOSClient::SetClientCert(NN<Crypto::Cert::X509Cert> cert, NN<Crypto::Cert::X509File> key)
{
#if defined(CURLOPTTYPE_BLOB)
	if (this->clsData->curl)
	{
		struct curl_blob blob;
		blob.data = (void*)cert->GetASN1Buff().Ptr();
		blob.len = (size_t)cert->GetASN1BuffSize();
		blob.flags = CURL_BLOB_COPY;
		curl_easy_setopt(this->clsData->curl, CURLOPT_SSLCERTTYPE, "DER");
		curl_easy_setopt(this->clsData->curl, CURLOPT_SSLCERT_BLOB, &blob);
		blob.data = (void*)key->GetASN1Buff().Ptr();
		blob.len = (size_t)key->GetASN1BuffSize();
		blob.flags = CURL_BLOB_COPY;
		curl_easy_setopt(this->clsData->curl, CURLOPT_SSLKEYTYPE, "DER");
		curl_easy_setopt(this->clsData->curl, CURLOPT_SSLKEY_BLOB, &blob);
		return true;
	}
#endif
	return false;
}


Optional<const Data::ReadingListNN<Crypto::Cert::Certificate>> Net::HTTPOSClient::GetServerCerts()
{
	if (this->IsSecureConn() && this->clsData->curl)
	{
		if (this->clsData->certs.NotNull())
			return this->clsData->certs;
		struct curl_certinfo *ci;
		if (!curl_easy_getinfo(this->clsData->curl, CURLINFO_CERTINFO, &ci))
		{
			if (ci->num_of_certs > 0)
			{
				NN<Data::ArrayListNN<Crypto::Cert::Certificate>> certList;
				NEW_CLASSNN(certList, Data::ArrayListNN<Crypto::Cert::Certificate>());
				this->clsData->certs = certList;
				NN<Crypto::Cert::Certificate> cert;
				int i = 0;
				while (i < ci->num_of_certs)
				{
					NEW_CLASSNN(cert, Crypto::Cert::CurlCert(ci->certinfo[i]));
					certList->Add(cert);
					i++;
				}
				return certList;
			}
		}
	}
	return 0;
}
