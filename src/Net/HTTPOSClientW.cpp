#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Cert/WinHttpCert.h"
#include "Data/ArrayList.h"
#include "Data/DateTime.h"
#include "IO/Stream.h"
#include "Net/HTTPClient.h"
#include "Net/HTTPOSClient.h"
#include "Net/SocketFactory.h"
#include "Net/TCPClient.h"
#include "Sync/Event.h"
#include "Text/Encoding.h"
#include "Text/EncodingFactory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringBuilderUTF16.h"
#include "Text/TextEnc/FormEncoding.h"
#include "Text/TextEnc/URIEncoding.h"
#include <windows.h>
#include <winhttp.h>
#include <stdio.h>

#define VERBOSE 1

#define BUFFSIZE 2048

struct Net::HTTPOSClient::ClassData
{
	HINTERNET hSession;
	HINTERNET hConnect;
	HINTERNET hRequest;
	Bool https;
};

void __stdcall HTTPOSClient_StatusCb(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
	printf("WinHTTP Callback status = 0x%lx\r\n", dwInternetStatus);

	if (dwInternetStatus == WINHTTP_CALLBACK_STATUS_SECURE_FAILURE)
	{
//		OSInt i = 0;
		DWORD err = *(DWORD*)lpvStatusInformation;
		printf("WINHTTP_CALLBACK_STATUS_SECURE_FAILURE, info = 0x%lx\r\n", err);
		if (err & WINHTTP_CALLBACK_STATUS_FLAG_CERT_REV_FAILED)
		{
//			i = 0;
		}
		if (err & WINHTTP_CALLBACK_STATUS_FLAG_INVALID_CERT)
		{
//			i = 0;
		}
		if (err & WINHTTP_CALLBACK_STATUS_FLAG_CERT_REV_FAILED)
		{
//			i = 0;
		}
		if (err & WINHTTP_CALLBACK_STATUS_FLAG_CERT_REVOKED)
		{
//			i = 0;
		}
		if (err & WINHTTP_CALLBACK_STATUS_FLAG_INVALID_CA)
		{
//			i = 0;
		}
		if (err & WINHTTP_CALLBACK_STATUS_FLAG_CERT_CN_INVALID)
		{
//			i = 0;
		}
		if (err & WINHTTP_CALLBACK_STATUS_FLAG_CERT_DATE_INVALID)
		{
//			i = 0;
		}
		if (err & WINHTTP_CALLBACK_STATUS_FLAG_SECURITY_CHANNEL_ERROR)
		{
//			i = 0;
		}
	}
}

Net::HTTPOSClient::HTTPOSClient(Net::SocketFactory *sockf, const UTF8Char *userAgent, Bool kaConn) : Net::HTTPClient(sockf, kaConn)
{
	ClassData *data = MemAlloc(ClassData, 1);
	this->clsData = data;
	data->hSession = 0;
	data->hConnect = 0;
	data->hRequest = 0;
	data->https = false;
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
	const WChar *wptr = Text::StrToWCharNew(userAgent);
#if defined(WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY)
	data->hSession = WinHttpOpen(wptr, WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
#endif
	if (data->hSession == 0)
	{
		data->hSession = WinHttpOpen(wptr, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
		if (data->hSession == 0)
		{
			data->hSession = WinHttpOpen(wptr, WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
			if (data->hSession == 0)
			{
				printf("hSession is null, code = %ld\r\n", GetLastError());
			}
		}
	}
	Text::StrDelNew(wptr);
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
	if (data->hRequest)
		WinHttpCloseHandle(data->hRequest);
	if (data->hConnect)
		WinHttpCloseHandle(data->hConnect);
	if (data->hSession)
		WinHttpCloseHandle(data->hSession);
	MemFree(data);
	DEL_CLASS(this->reqMstm);
}

Bool Net::HTTPOSClient::IsError()
{
	ClassData *data = (ClassData*)this->clsData;
	return data->hRequest == 0;
}

UOSInt Net::HTTPOSClient::Read(UInt8 *buff, UOSInt size)
{
	ClassData *data = (ClassData*)this->clsData;
	this->EndRequest(0, 0);
	if (data->hRequest == 0)
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
		DWORD bytesRead;
		BOOL succ = WinHttpReadData(data->hRequest, this->dataBuff, (DWORD)size, &bytesRead);
		if (succ)
		{
			this->buffSize = bytesRead;
		}
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
	Bool https = false;

	UOSInt i;
	const UTF8Char *ptr1;
	const UTF8Char *ptr2;
	UTF8Char *ptrs[2];
	UInt16 port;
	UInt16 defPort;
	Double t1;
	ClassData *data = (ClassData*)this->clsData;
	if (data->hSession == 0)
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
		if (i != INVALID_INDEX)
		{
			MemCopyNO(urltmp, ptr1, i * sizeof(UTF8Char));
			urltmp[i] = 0;
			ptr2 = &ptr1[i];
		}
		else
		{
			i = Text::StrCharCnt(ptr1);
			ptr2 = 0;
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
		if (i != INVALID_INDEX)
		{
			MemCopyNO(urltmp, ptr1, i * sizeof(UTF8Char));
			urltmp[i] = 0;
			ptr2 = &ptr1[i];
		}
		else
		{
			i = Text::StrCharCnt(ptr1);
			ptr2 = 0;
			MemCopyNO(urltmp, ptr1, i * sizeof(UTF8Char));
			urltmp[i] = 0;
		}
		Text::TextEnc::URIEncoding::URIDecode(urltmp, urltmp);
		defPort = 443;
		https = true;
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
		if (i == INVALID_INDEX)
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
			*(UInt32*)this->svrAddr.addr = Net::SocketUtil::GetIPAddr((const UTF8Char*)"127.0.0.1");
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

		const WChar *wptr = Text::StrToWCharNew(this->cliHost);
		data->hConnect = WinHttpConnect(data->hSession, wptr, port, 0);
		Text::StrDelNew(wptr);
		if (data->hConnect == 0)
		{
			printf("hConnect is null, code = %ld\r\n", GetLastError());
			this->writing = true;
			this->canWrite = false;
			return false;
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
		if (data->hRequest)
		{
			WinHttpCloseHandle(data->hRequest);
			data->hRequest = 0;
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

	if (ptr2 == 0)
	{
		ptr2 = (const UTF8Char*)"/";
	}

	const WChar *target = Text::StrToWCharNew(ptr2);
	const WChar *wmethod;
	if (method && Text::StrEqualsICase(method, "POST"))
	{
		wmethod = L"POST";
		this->canWrite = true;
		this->writing = false;
	}
	else
	{
		wmethod = L"GET";
		this->canWrite = false;
		this->writing = false;
	}
	data->hRequest = WinHttpOpenRequest(data->hConnect, wmethod, target, 0, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, https?WINHTTP_FLAG_SECURE:0);
	Text::StrDelNew(target);

	if (data->hRequest)
	{
		WinHttpSetStatusCallback(data->hRequest, HTTPOSClient_StatusCb, WINHTTP_CALLBACK_FLAG_SECURE_FAILURE, 0);
		this->AddHeader((const UTF8Char*)"Accept", (const UTF8Char*)"*/*");
		this->AddHeader((const UTF8Char*)"Accept-Charset", (const UTF8Char*)"*");
		if (!this->kaConn)
		{
			DWORD feature = WINHTTP_DISABLE_KEEP_ALIVE;
			WinHttpSetOption(data->hRequest, WINHTTP_OPTION_DISABLE_FEATURE, &feature, sizeof(feature));
		}
		if (https)
		{
			unsigned long flags = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID;
			WinHttpSetOption(data->hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &flags, sizeof(flags));
		}
	}
	data->https = https;
	return true;
}

void Net::HTTPOSClient::AddHeader(const UTF8Char *name, const UTF8Char *value)
{
	ClassData *data = (ClassData*)this->clsData;
	if (data->hRequest && !writing)
	{
		if (Text::StrEquals(name, (const UTF8Char*)"User-Agent"))
		{
		}
		else if (Text::StrEquals(name, (const UTF8Char*)"Connection"))
		{
		}
		else
		{
			Text::StringBuilderUTF16 sb;
			sb.Append(name);
			sb.Append((const UTF8Char*)": ");
			sb.Append(value);
			WinHttpAddRequestHeaders(data->hRequest, sb.ToString(), (DWORD)sb.GetLength(), WINHTTP_ADDREQ_FLAG_ADD);
		}
	}
}

void Net::HTTPOSClient::EndRequest(Double *timeReq, Double *timeResp)
{
	ClassData *data = (ClassData*)this->clsData;
	if (data->hRequest == 0 || (this->writing && !this->canWrite))
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

		BOOL succ;
		if (reqSize <= 0)
		{
			succ = WinHttpSendRequest(data->hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
		}
		else
		{
			succ = WinHttpSendRequest(data->hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, reqBuff, (DWORD)reqSize, (DWORD)reqSize, 0);
		}
		this->reqMstm->Clear();
		t1 = this->clk->GetTimeDiff();
		if (timeReq)
		{
			*timeReq = t1;
		}
		if (!succ)
		{
			DWORD err = GetLastError();
#if defined(VERBOSE)
			printf("HTTPOSClientW: WinHttpSendRequest Error = %ld\r\n", err);
#endif
			if (err == ERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED)
			{
				succ = false;
			}
		}

		if (succ)
		{
			succ = WinHttpReceiveResponse(data->hRequest, 0);
			if (!succ)
			{
				DWORD err = GetLastError();
				if (err == ERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED)
				{
					succ = false;
				}
			}
			if (succ)
			{
				this->contLeng = 0x7fffffff;
				this->contRead = 0;

				DWORD dwStatusCode = 0;
				DWORD dwSize = sizeof(dwStatusCode);

				WinHttpQueryHeaders(data->hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &dwStatusCode, &dwSize, WINHTTP_NO_HEADER_INDEX);
				this->respStatus = (Net::WebStatus::StatusCode)dwStatusCode;

				BOOL succ = WinHttpQueryHeaders(data->hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, 0, WINHTTP_NO_OUTPUT_BUFFER, &dwSize, WINHTTP_NO_HEADER_INDEX);
				if (!succ && (GetLastError() == ERROR_INSUFFICIENT_BUFFER) && dwSize > 0)
				{
					WChar *buff = MemAlloc(WChar, dwSize / sizeof(WChar));
					succ = WinHttpQueryHeaders(data->hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, 0, buff, &dwSize, WINHTTP_NO_HEADER_INDEX);
					if (succ)
					{
						WChar *wptr = buff;
						const UTF8Char *sptr;
						UOSInt i;
						while ((i = Text::StrIndexOf(wptr, L"\r\n")) != INVALID_INDEX && i > 0)
						{
							if (Text::StrStartsWith(wptr, L"HTTP/"))
							{
							}
							else
							{
								wptr[i] = 0;
								sptr = Text::StrToUTF8New(wptr);
								this->headers->Add((UTF8Char*)sptr);

								if (Text::StrStartsWith(sptr, (const UTF8Char*)"Content-Length: "))
								{
									this->contLeng = Text::StrToUInt64(&sptr[16]);
								}
							}
							wptr = &wptr[i + 2];
						}
					}

					MemFree(buff);
				}
			}
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

Bool Net::HTTPOSClient::IsSecureConn()
{
	return this->clsData->https;
}

Crypto::Cert::Certificate *Net::HTTPOSClient::GetServerCert()
{
	UInt8 *certInfo;
	DWORD certSize = 0;
	if (WinHttpQueryOption(this->clsData->hRequest, WINHTTP_OPTION_SECURITY_CERTIFICATE_STRUCT, 0, &certSize) != FALSE)
	{
		return 0;
	}
	if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
	{
		return 0;
	}
	certInfo = MemAlloc(UInt8, certSize);
	if (WinHttpQueryOption(this->clsData->hRequest, WINHTTP_OPTION_SECURITY_CERTIFICATE_STRUCT, certInfo, &certSize) == FALSE)
	{
		return 0;
	}
	Crypto::Cert::WinHttpCert *cert;
	NEW_CLASS(cert, Crypto::Cert::WinHttpCert(certInfo));
	return cert;
}
