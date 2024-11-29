#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Cert/WinHttpCert.h"
#include "Data/ArrayList.h"
#include "Data/DateTime.h"
#include "IO/Stream.h"
#include "IO/WindowsError.h"
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
#include "Text/TextBinEnc/URIEncoding.h"
#include <windows.h>
#include <winhttp.h>
#include <stdio.h>

//#define VERBOSE
//#include "WinDebug.h"

#define BUFFSIZE 2048

struct Net::HTTPOSClient::ClassData
{
	HINTERNET hSession;
	HINTERNET hConnect;
	HINTERNET hRequest;
	Bool https;
	Data::ArrayListNN<Crypto::Cert::Certificate> *certs;
	Crypto::Cert::X509Cert* cliCert;
	Crypto::Cert::X509File* cliKey;
};

void __stdcall HTTPOSClient_StatusCb(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
	printf("WinHTTP Callback status = 0x%lx\r\n", (UInt32)dwInternetStatus);

	if (dwInternetStatus == WINHTTP_CALLBACK_STATUS_SECURE_FAILURE)
	{
//		OSInt i = 0;
		DWORD err = *(DWORD*)lpvStatusInformation;
		printf("WINHTTP_CALLBACK_STATUS_SECURE_FAILURE, info = 0x%lx\r\n", (UInt32)err);
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

Net::HTTPOSClient::HTTPOSClient(NN<Net::TCPClientFactory> clif, Text::CString userAgent, Bool kaConn) : Net::HTTPClient(clif, kaConn)
{
	ClassData *data = MemAlloc(ClassData, 1);
	this->clsData = data;
	data->hSession = 0;
	data->hConnect = 0;
	data->hRequest = 0;
	data->https = false;
	data->certs = 0;
	data->cliCert = 0;
	data->cliKey = 0;
	this->cliHost = 0;
	this->writing = false;
	this->dataBuff = 0;
	this->buffSize = 0;
	this->contRead = 0;
//	this->timeOutMS = 5000;
	this->dataBuff = MemAlloc(UInt8, BUFFSIZE);
	NEW_CLASS(this->reqMstm, IO::MemoryStream(1024));
	Text::CStringNN nnuserAgent;
	if (!userAgent.SetTo(nnuserAgent))
	{
		nnuserAgent = CSTR("sswr/1.0");
	}
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(nnuserAgent.v);
#if defined(WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY)
	data->hSession = WinHttpOpen(wptr.Ptr(), WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
#endif
	if (data->hSession == 0)
	{
		data->hSession = WinHttpOpen(wptr.Ptr(), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
		if (data->hSession == 0)
		{
			data->hSession = WinHttpOpen(wptr.Ptr(), WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
			if (data->hSession == 0)
			{
				printf("hSession is null, code = %ld\r\n", (UInt32)GetLastError());
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
	ClassData *data = this->clsData;
	if (data->hRequest)
		WinHttpCloseHandle(data->hRequest);
	if (data->hConnect)
		WinHttpCloseHandle(data->hConnect);
	if (data->hSession)
		WinHttpCloseHandle(data->hSession);
	if (data->certs)
	{
		UOSInt i = data->certs->GetCount();
		while (i-- > 0)
		{
			NN<Crypto::Cert::Certificate> cert = data->certs->GetItemNoCheck(i);
			cert.Delete();
		}
		DEL_CLASS(data->certs);
	}
	SDEL_CLASS(data->cliCert);
	SDEL_CLASS(data->cliKey);
	MemFree(data);
	DEL_CLASS(this->reqMstm);
}

Bool Net::HTTPOSClient::IsError() const
{
	ClassData *data = this->clsData;
	return data->hRequest == 0;
}

UOSInt Net::HTTPOSClient::Read(const Data::ByteArray &buff)
{
	ClassData *data = this->clsData;
	this->EndRequest(0, 0);
	if (data->hRequest == 0)
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
		DWORD bytesRead;
		BOOL succ = WinHttpReadData(data->hRequest, this->dataBuff, (DWORD)myBuff.GetSize(), &bytesRead);
		if (succ)
		{
			this->buffSize = bytesRead;
		}
	}
	if (this->buffSize >= myBuff.GetSize())
	{
		myBuff.CopyFrom(Data::ByteArrayR(this->dataBuff, myBuff.GetSize()));
		MemCopyO(this->dataBuff, &this->dataBuff[myBuff.GetSize()], this->buffSize - myBuff.GetSize());
		this->buffSize -= myBuff.GetSize();
		this->contRead += myBuff.GetSize();
		return myBuff.GetSize();
	}
	else
	{
		myBuff.CopyFrom(Data::ByteArrayR(this->dataBuff, this->buffSize));
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
	Bool https = false;

	UOSInt i;
	const UTF8Char *ptr1;
	const UTF8Char *ptr2;
	UnsafeArray<UTF8Char> ptrs[2];
	UInt16 port;
	UInt16 defPort;
	Double t1;
	ClassData *data = this->clsData;
	if (data->hSession == 0)
	{
		timeDNS.Set(-1);
		timeConn.Set(-1);
		return false;
	}

	this->url->Release();
	this->url = Text::String::New(url.v, url.leng);
	this->SetSourceName(this->url);
	if (url.StartsWith(UTF8STRC("http://")))
	{
		ptr1 = &url.v[7];
		i = Text::StrIndexOfChar(ptr1, '/');
		if (i != INVALID_INDEX)
		{
			MemCopyNO(urltmp, ptr1, i * sizeof(UTF8Char));
			urltmp[i] = 0;
			ptr2 = &ptr1[i];
		}
		else
		{
			i = url.leng - 7;
			ptr2 = 0;
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
			ptr2 = &ptr1[i];
		}
		else
		{
			i = url.leng - 8;
			ptr2 = 0;
			MemCopyNO(urltmp, ptr1, i * sizeof(UTF8Char));
			urltmp[i] = 0;
		}
		Text::TextBinEnc::URIEncoding::URIDecode(urltmp, urltmp);
		defPort = 443;
		https = true;
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
	UnsafeArray<const UTF8Char> cliHost;
	if (!this->cliHost.SetTo(cliHost))
	{
		cliHost = Text::StrCopyNew(urltmp);
		this->cliHost = cliHost.Ptr();
		if (Text::StrEqualsICaseC(svrname, (UOSInt)(svrnameEnd - svrname), UTF8STRC("localhost")))
		{
			this->svrAddr.addrType = Net::AddrType::IPv4;
			*(UInt32*)this->svrAddr.addr = Net::SocketUtil::GetIPAddr(CSTR("127.0.0.1"));
		}
		else if (!this->clif->GetSocketFactory()->DNSResolveIP(CSTRP(svrname, svrnameEnd), this->svrAddr))
		{
			this->writing = true;
			this->canWrite = false;
			return false;
		}
		timeDNS.Set(this->clk.GetTimeDiff());

		UnsafeArray<const WChar> wptr = Text::StrToWCharNew(cliHost);
		data->hConnect = WinHttpConnect(data->hSession, wptr.Ptr(), port, 0);
		Text::StrDelNew(wptr);
		if (data->hConnect == 0)
		{
			printf("hConnect is null, code = %ld\r\n", GetLastError());
			this->writing = true;
			this->canWrite = false;
			return false;
		}
		t1 = this->clk.GetTimeDiff();
		timeConn.Set(t1);
//		this->sockf->SetLinger(cli->GetSocket(), 0);
//		this->sockf->SetNoDelay(cli->GetSocket(), true);
	}
	else if (Text::StrEquals(cliHost, urltmp))
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

	if (ptr2 == 0)
	{
		ptr2 = (const UTF8Char*)"/";
	}

	UnsafeArray<const WChar> target = Text::StrToWCharNew(ptr2);
	UnsafeArray<const WChar> wmethod;
	if (method == Net::WebUtil::RequestMethod::HTTP_POST)
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
	data->hRequest = WinHttpOpenRequest(data->hConnect, wmethod.Ptr(), target.Ptr(), 0, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, https?WINHTTP_FLAG_SECURE:0);
	Text::StrDelNew(target);

	if (data->hRequest)
	{
		if (https)
		{
			NN<Crypto::Cert::X509Cert> cliCert;
			NN<Crypto::Cert::X509File> cliKey;
			if (cliCert.Set(this->clsData->cliCert) && cliKey.Set(this->clsData->cliKey))
			{
				this->SetClientCert(cliCert, cliKey);
			}
		}
		WinHttpSetStatusCallback(data->hRequest, HTTPOSClient_StatusCb, WINHTTP_CALLBACK_FLAG_SECURE_FAILURE, 0);
		this->AddHeaderC(CSTR("Accept"), CSTR("*/*"));
		this->AddHeaderC(CSTR("Accept-Charset"), CSTR("*"));
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

void Net::HTTPOSClient::AddHeaderC(Text::CStringNN name, Text::CString value)
{
	ClassData *data = this->clsData;
	if (data->hRequest && !writing)
	{
		if (name.Equals(UTF8STRC("User-Agent")))
		{
		}
		else if (name.Equals(UTF8STRC("Connection")))
		{
		}
		else
		{
			Text::StringBuilderUTF16 sb;
			sb.AppendC(name.v, name.leng);
			sb.AppendC(UTF8STRC(": "));
			sb.AppendC(value.OrEmpty().v, value.leng);
			WinHttpAddRequestHeaders(data->hRequest, sb.ToPtr(), (DWORD)sb.GetLength(), WINHTTP_ADDREQ_FLAG_ADD);
		}
	}
}

void Net::HTTPOSClient::EndRequest(OptOut<Double> timeReq, OptOut<Double> timeResp)
{
	ClassData *data = this->clsData;
	if (data->hRequest == 0 || (this->writing && !this->canWrite))
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

		BOOL succ;
		if (reqSize <= 0)
		{
			succ = WinHttpSendRequest(data->hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
		}
		else
		{
			succ = WinHttpSendRequest(data->hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, reqBuff.Ptr(), (DWORD)reqSize, (DWORD)reqSize, 0);
		}
		this->reqMstm->Clear();
		t1 = this->clk.GetTimeDiff();
		timeReq.Set(t1);
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
						NN<Text::String> s;
						UOSInt i;
						while ((i = Text::StrIndexOfW(wptr, L"\r\n")) != INVALID_INDEX && i > 0)
						{
							if (Text::StrStartsWith(wptr, L"HTTP/"))
							{
							}
							else
							{
								wptr[i] = 0;
								s = Text::String::NewNotNull(wptr);
								this->headers.Add(s);

								if (s->StartsWith(UTF8STRC("Content-Length: ")))
								{
									this->contLeng = Text::StrToUInt64(&s->v[16]);
								}
							}
							wptr = &wptr[i + 2];
						}
					}

					MemFree(buff);
				}
			}
		}
		t1 = this->clk.GetTimeDiff();
		timeResp.Set(t1);
	}
}

void Net::HTTPOSClient::SetTimeout(Data::Duration timeout)
{
	if (this->clsData->hRequest)
	{
		DWORD to = (DWORD)timeout.GetTotalMS();
		WinHttpSetOption(this->clsData->hRequest, WINHTTP_OPTION_CONNECT_TIMEOUT, &to, sizeof(to));
	}
}

Bool Net::HTTPOSClient::IsSecureConn() const
{
	return this->clsData->https;
}

Bool WinSSLEngine_InitKey(HCRYPTPROV* hProvOut, HCRYPTKEY* hKeyOut, NN<Crypto::Cert::X509File> keyASN1, const WChar* containerName, Bool signature, CRYPT_KEY_PROV_INFO *keyProvInfo);

Bool Net::HTTPOSClient::SetClientCert(NN<Crypto::Cert::X509Cert> cert, NN<Crypto::Cert::X509File> key)
{
	if (this->clsData->hRequest)
	{
		const WChar* containerName = L"ServerCert";
		HCRYPTKEY hKey;
		HCRYPTPROV hProv;
		CRYPT_KEY_PROV_INFO keyProvInfo;
		MemClear(&keyProvInfo, sizeof(keyProvInfo));
		if (!WinSSLEngine_InitKey(&hProv, &hKey, key, containerName, false, &keyProvInfo))
		{
			return false;
		}

		PCCERT_CONTEXT serverCert = CertCreateCertificateContext(X509_ASN_ENCODING, cert->GetASN1Buff().Ptr(), (DWORD)cert->GetASN1BuffSize());
		keyProvInfo.cProvParam = 0;
		keyProvInfo.rgProvParam = NULL;
		keyProvInfo.dwKeySpec = AT_KEYEXCHANGE;
		CertSetCertificateContextProperty(serverCert, CERT_KEY_PROV_INFO_PROP_ID, 0, &keyProvInfo);

		HCRYPTPROV_OR_NCRYPT_KEY_HANDLE hCryptProvOrNCryptKey = 0;
		BOOL fCallerFreeProvOrNCryptKey = FALSE;
		DWORD dwKeySpec;
		CryptAcquireCertificatePrivateKey(serverCert, 0, NULL, &hCryptProvOrNCryptKey, &dwKeySpec, &fCallerFreeProvOrNCryptKey);
		DWORD certSize = sizeof(CERT_CONTEXT);
		Bool succ = (WinHttpSetOption(this->clsData->hRequest, WINHTTP_OPTION_CLIENT_CERT_CONTEXT, (LPVOID)serverCert, certSize) == TRUE);
#if defined(VERBOSE)
		if (!succ)
		{
			printf("HTTPOSClient: WinHttpSetOption CLIENT_CERT_CONTEXT failed: 0x%x (%s)\r\n", GetLastError(), IO::WindowsError::GetString(GetLastError()).v);
		}
#endif
		CertFreeCertificateContext(serverCert);
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return succ;
	}
	else
	{
		SDEL_CLASS(this->clsData->cliCert);
		SDEL_CLASS(this->clsData->cliKey);
		this->clsData->cliCert = (Crypto::Cert::X509Cert*)cert->Clone().Ptr();
		this->clsData->cliKey = (Crypto::Cert::X509File*)key->Clone().Ptr();
		return true;
	}
}

Optional<const Data::ReadingListNN<Crypto::Cert::Certificate>> Net::HTTPOSClient::GetServerCerts()
{
	if (this->clsData->certs)
		return this->clsData->certs;
	PCCERT_CONTEXT serverCert = 0;
	DWORD certSize = sizeof(serverCert);
	if (WinHttpQueryOption(this->clsData->hRequest, WINHTTP_OPTION_SERVER_CERT_CONTEXT, &serverCert, &certSize) == FALSE)
	{
		return 0;
	}
	PCCERT_CONTEXT thisCert = 0;
	PCCERT_CONTEXT lastCert = 0;
	DWORD dwVerificationFlags = 0;
	NN<Crypto::Cert::X509Cert> cert;
	NEW_CLASS(this->clsData->certs, Data::ArrayListNN<Crypto::Cert::Certificate>());
	NEW_CLASSNN(cert, Crypto::Cert::X509Cert(CSTR("RemoteCert"), Data::ByteArrayR(serverCert->pbCertEncoded, serverCert->cbCertEncoded)));
	this->clsData->certs->Add(cert);
	thisCert = CertGetIssuerCertificateFromStore(serverCert->hCertStore, serverCert, NULL, &dwVerificationFlags);
	while (thisCert)
	{
		NEW_CLASSNN(cert, Crypto::Cert::X509Cert(CSTR("RemoteCert"), Data::ByteArrayR(thisCert->pbCertEncoded, thisCert->cbCertEncoded)));
		this->clsData->certs->Add(cert);
		lastCert = thisCert;
		thisCert = CertGetIssuerCertificateFromStore(serverCert->hCertStore, lastCert, NULL, &dwVerificationFlags);
		CertFreeCertificateContext(lastCert);
	}
	CertFreeCertificateContext(serverCert);
	return this->clsData->certs;
}
