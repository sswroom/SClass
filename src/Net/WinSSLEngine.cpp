#include "Stdafx.h"
#include "Data/DateTime.h"
#include "IO/DebugWriter.h"
#include "Net/ASN1PDUBuilder.h"
#include "Net/WinSSLClient.h"
#include "Net/WinSSLEngine.h"
#include "Text/MyString.h"
#include "Text/StringBuilderW.h"
#include <windows.h>
#include <ntsecapi.h>
#define SECURITY_WIN32
#include <sspi.h>
#include <schnlsp.h>

struct Net::WinSSLEngine::ClassData
{
	CredHandle hCredCli;
	Bool cliInit;
	CredHandle hCredSvr;
	Bool svrInit;
	Method method;
};


extern void SecBuffer_Set(SecBuffer *buff, UInt32 type, UInt8 *inpBuff, UInt32 leng);
extern void SecBufferDesc_Set(SecBufferDesc *desc, SecBuffer *buffs, UInt32 nBuffs);

UInt32 WinSSLEngine_GetProtocols(Net::SSLEngine::Method method, Bool server)
{
	if (server)
	{
		switch (method)
		{
		case Net::SSLEngine::M_SSLV3:
			return SP_PROT_SSL3_SERVER;
		case Net::SSLEngine::M_SSLV23:
			return SP_PROT_SSL2_SERVER;
		#if defined(SP_PROT_TLS1_2)
		case Net::SSLEngine::M_TLS:
			return SP_PROT_TLS1_1_SERVER;
		case Net::SSLEngine::M_TLSV1:
			return SP_PROT_TLS1_0_SERVER;
		case Net::SSLEngine::M_TLSV1_1:
			return SP_PROT_TLS1_1_SERVER;
		case Net::SSLEngine::M_TLSV1_2:
			return SP_PROT_TLS1_2_SERVER;
		case Net::SSLEngine::M_DTLS:
			return SP_PROT_DTLS1_0_SERVER;
		case Net::SSLEngine::M_DTLSV1:
			return SP_PROT_DTLS1_0_SERVER;
		case Net::SSLEngine::M_DTLSV1_2:
			return SP_PROT_DTLS1_2_SERVER;
		default:
			return SP_PROT_TLS1_0_SERVER || SP_PROT_TLS1_1_SERVER || SP_PROT_TLS1_2_SERVER || SP_PROT_TLS1_3_SERVER;
		#else
		case Net::SSLEngine::M_TLS:
			return SP_PROT_TLS1;
		case Net::SSLEngine::M_TLSV1:
			return SP_PROT_TLS1;
		case Net::SSLEngine::M_TLSV1_1:
			return SP_PROT_TLS1;
		case Net::SSLEngine::M_TLSV1_2:
			return SP_PROT_TLS1;
		case Net::SSLEngine::M_DTLS:
			return SP_PROT_TLS1;
		case Net::SSLEngine::M_DTLSV1:
			return SP_PROT_TLS1;
		case Net::SSLEngine::M_DTLSV1_2:
			return SP_PROT_TLS1;
		default:
			return SP_PROT_TLS1;
		#endif
		}
	}
	else
	{
		switch (method)
		{
		case Net::SSLEngine::M_SSLV3:
			return SP_PROT_SSL3_CLIENT;
		case Net::SSLEngine::M_SSLV23:
			return SP_PROT_SSL3_CLIENT;
		#if defined(SP_PROT_TLS1_2)
		case Net::SSLEngine::M_TLS:
			return SP_PROT_TLS1_2_CLIENT;
		case Net::SSLEngine::M_TLSV1:
			return SP_PROT_TLS1_0_CLIENT;
		case Net::SSLEngine::M_TLSV1_1:
			return SP_PROT_TLS1_1_CLIENT;
		case Net::SSLEngine::M_TLSV1_2:
			return SP_PROT_TLS1_2_CLIENT;
		case Net::SSLEngine::M_DTLS:
			return SP_PROT_DTLS1_2_CLIENT;
		case Net::SSLEngine::M_DTLSV1:
			return SP_PROT_DTLS1_0_CLIENT;
		case Net::SSLEngine::M_DTLSV1_2:
			return SP_PROT_DTLS1_2_CLIENT;
		default:
			return SP_PROT_TLS1_2_CLIENT;
		#else
		case Net::SSLEngine::M_TLS:
			return SP_PROT_TLS1_CLIENT;
		case Net::SSLEngine::M_TLSV1:
			return SP_PROT_TLS1_CLIENT;
		case Net::SSLEngine::M_TLSV1_1:
			return SP_PROT_TLS1_CLIENT;
		case Net::SSLEngine::M_TLSV1_2:
			return SP_PROT_TLS1_CLIENT;
		case Net::SSLEngine::M_DTLS:
			return SP_PROT_TLS1_CLIENT;
		case Net::SSLEngine::M_DTLSV1:
			return SP_PROT_TLS1_CLIENT;
		case Net::SSLEngine::M_DTLSV1_2:
			return SP_PROT_TLS1_CLIENT;
		default:
			return SP_PROT_TLS1_CLIENT;
		#endif
		}
	}
}

Bool Net::WinSSLEngine::InitClient(Method method)
{
	SCHANNEL_CRED credData;
	SECURITY_STATUS status;
	TimeStamp lifetime;
	MemClear(&credData, sizeof(credData));
	credData.dwVersion = SCHANNEL_CRED_VERSION;
	credData.grbitEnabledProtocols = WinSSLEngine_GetProtocols(method, false);

	status = AcquireCredentialsHandle(
		NULL,
		(LPWSTR)UNISP_NAME,
		SECPKG_CRED_OUTBOUND,
		NULL,
		&credData,
		NULL,
		NULL,
		&this->clsData->hCredCli,
		&lifetime
	);
	return status == 0;
}

Bool Net::WinSSLEngine::InitServer(Method method, void *cred)
{
	SCHANNEL_CRED credData;
	SECURITY_STATUS status;
	TimeStamp lifetime;
	MemClear(&credData, sizeof(credData));
	credData.dwVersion = SCHANNEL_CRED_VERSION;
	credData.grbitEnabledProtocols = 0; // WinSSLEngine_GetProtocols(method, true);
	credData.paCred = (PCCERT_CONTEXT*)&cred;
	credData.cCreds = 1;

	status = AcquireCredentialsHandle(
		NULL,
		(LPWSTR)UNISP_NAME,
		SECPKG_CRED_INBOUND,
		NULL,
		&credData,
		NULL,
		NULL,
		&this->clsData->hCredSvr,
		&lifetime
	);
	return status == 0;
}

Net::TCPClient *Net::WinSSLEngine::CreateServerConn(UInt32 *s)
{
	if (!this->clsData->svrInit)
	{
		this->sockf->DestroySocket(s);
		return 0;
	}


	this->sockf->SetRecvTimeout(s, 3000);
	CtxtHandle ctxt;
	SecBuffer inputBuff[2];
	SecBuffer outputBuff[3];
	SecBufferDesc inputDesc;
	SecBufferDesc outputDesc;
	UInt8 recvBuff[2048];
	unsigned long retFlags = ASC_REQ_ALLOCATE_MEMORY | ASC_REQ_STREAM;
	UOSInt recvOfst = 0;
	UOSInt recvSize;
	TimeStamp ts;
	Net::SocketFactory::ErrorType et;
	recvSize = this->sockf->ReceiveData(s, recvBuff, 2048, &et);
	if (recvSize == 0)
	{
		this->sockf->DestroySocket(s);
		return 0;
	}

	SecBuffer_Set(&inputBuff[0], SECBUFFER_TOKEN, recvBuff, (UInt32)recvSize);
	SecBuffer_Set(&inputBuff[1], SECBUFFER_EMPTY, 0, 0);
	SecBufferDesc_Set(&inputDesc, inputBuff, 2);

	SecBuffer_Set(&outputBuff[0], SECBUFFER_TOKEN, 0, 0);
#if defined(SECBUFFER_ALERT)
	SecBuffer_Set(&outputBuff[1], SECBUFFER_ALERT, 0, 0);
#else
	SecBuffer_Set(&outputBuff[1], SECBUFFER_EMPTY, 0, 0);
#endif
	SecBuffer_Set(&outputBuff[2], SECBUFFER_EMPTY, 0, 0);
	SecBufferDesc_Set(&outputDesc, outputBuff, 3);

	IO::DebugWriter debug;
	Text::StringBuilderW sb;
	sb.ClearStr();
	sb.Append((const UTF8Char*)"Received ");
	sb.AppendU32(recvSize);
	sb.Append((const UTF8Char*)" bytes");
	debug.WriteLineW(sb.ToString());

	Bool succ = true;
	UOSInt i;
	SECURITY_STATUS status;
	status = AcceptSecurityContext(
		&this->clsData->hCredSvr,
		0,
		&inputDesc,
		retFlags,
		0,
		&ctxt,
		&outputDesc,
		(unsigned long*)&retFlags,
		&ts
	);
	if (status != SEC_I_CONTINUE_NEEDED)
	{
		this->sockf->DestroySocket(s);
		return 0;
	}
	recvOfst = 0;
	i = 0;
	while (i < 3)
	{
		if (outputBuff[i].BufferType == SECBUFFER_TOKEN && outputBuff[i].cbBuffer > 0)
		{
			if (this->sockf->SendData(s, (const UInt8*)outputBuff[i].pvBuffer, outputBuff[i].cbBuffer, &et) != outputBuff[i].cbBuffer)
			{
				succ = false;
			}
		}

		if (inputBuff[1].BufferType == SECBUFFER_EXTRA)
		{
			MemCopyNO(recvBuff, inputBuff[1].pvBuffer, inputBuff[1].cbBuffer);
			recvOfst = inputBuff[1].cbBuffer;
		}
		if (outputBuff[i].pvBuffer)
		{
			FreeContextBuffer(outputBuff[i].pvBuffer);
		}
		i++;
	}
	if (!succ)
	{
		DeleteSecurityContext(&ctxt);
		this->sockf->DestroySocket(s);
		return 0;
	}

	while (status == SEC_I_CONTINUE_NEEDED || status == SEC_E_INCOMPLETE_MESSAGE)
	{
		if (recvOfst == 0 || status == SEC_E_INCOMPLETE_MESSAGE)
		{
			recvSize = this->sockf->ReceiveData(s, &recvBuff[recvOfst], 2048 - recvOfst, &et);
			if (recvSize <= 0)
			{
				DeleteSecurityContext(&ctxt);
				this->sockf->DestroySocket(s);
				return 0;
			}
			recvOfst += recvSize;

			sb.ClearStr();
			sb.Append((const UTF8Char*)"Received ");
			sb.AppendU32(recvSize);
			sb.Append((const UTF8Char*)" bytes");
			debug.WriteLineW(sb.ToString());
		}
		SecBuffer_Set(&inputBuff[0], SECBUFFER_TOKEN, recvBuff, (UInt32)recvOfst);
		SecBuffer_Set(&inputBuff[1], SECBUFFER_EMPTY, 0, 0);
		SecBufferDesc_Set(&inputDesc, inputBuff, 2);

		SecBuffer_Set(&outputBuff[0], SECBUFFER_TOKEN, 0, 0);
#if defined(SECBUFFER_ALERT)
		SecBuffer_Set(&outputBuff[1], SECBUFFER_ALERT, 0, 0);
#else
		SecBuffer_Set(&outputBuff[1], SECBUFFER_EMPTY, 0, 0);
#endif
		SecBuffer_Set(&outputBuff[2], SECBUFFER_EMPTY, 0, 0);
		SecBufferDesc_Set(&outputDesc, outputBuff, 3);

		status = AcceptSecurityContext(
			&this->clsData->hCredSvr,
			&ctxt,
			&inputDesc,
			retFlags,
			0,
			&ctxt,
			&outputDesc,
			(unsigned long*)&retFlags,
			&ts
		);

		if (status == SEC_E_INCOMPLETE_MESSAGE)
		{

		}
		else if (status == SEC_I_CONTINUE_NEEDED || status == SEC_E_OK)
		{
			succ = true;
			recvOfst = 0;
			i = 0;
			while (i < 3)
			{
				if (outputBuff[i].BufferType == SECBUFFER_TOKEN && outputBuff[i].cbBuffer > 0)
				{
					if (this->sockf->SendData(s, (const UInt8*)outputBuff[i].pvBuffer, outputBuff[i].cbBuffer, &et) != outputBuff[i].cbBuffer)
					{
						succ = false;
					}
				}

				if (inputBuff[1].BufferType == SECBUFFER_EXTRA)
				{
					MemCopyNO(recvBuff, inputBuff[1].pvBuffer, inputBuff[1].cbBuffer);
					recvOfst = inputBuff[1].cbBuffer;
				}
				if (outputBuff[i].pvBuffer)
				{
					FreeContextBuffer(outputBuff[i].pvBuffer);
				}
				i++;
			}
			if (!succ)
			{
				DeleteSecurityContext(&ctxt);
				this->sockf->DestroySocket(s);
				return 0;
			}
		}
		else
		{
			sb.ClearStr();
			sb.Append((const UTF8Char*)"Error in initializing SSL Server connection: 0x");
			sb.AppendHex32(status);
			debug.WriteLineW(sb.ToString());
			if (status == SEC_I_INCOMPLETE_CREDENTIALS)
			{

			}
			DeleteSecurityContext(&ctxt);
			this->sockf->DestroySocket(s);
			return 0;
		}
	}

	Net::TCPClient *cli;
	NEW_CLASS(cli, Net::WinSSLClient(sockf, s, &ctxt));
	return cli;
}

Net::WinSSLEngine::WinSSLEngine(Net::SocketFactory *sockf, Method method) : Net::SSLEngine(sockf)
{
	this->clsData = MemAlloc(ClassData, 1);
	MemClear(this->clsData, sizeof(ClassData));
	this->clsData->method = method;

	this->skipCertCheck = false;
}

Net::WinSSLEngine::~WinSSLEngine()
{
	if (this->clsData->cliInit)
	{
		FreeCredentialsHandle(&this->clsData->hCredCli);
	}
	if (this->clsData->svrInit)
	{
		FreeCredentialsHandle(&this->clsData->hCredSvr);
	}
	MemFree(this->clsData);
}

void Net::WinSSLEngine::SetSkipCertCheck(Bool skipCertCheck)
{
	this->skipCertCheck = skipCertCheck;
}

Bool Net::WinSSLEngine::IsError()
{
	return false;
}

Bool WinSSLEngine_CryptImportPrivateKey(_Out_ HCRYPTKEY* phKey,
	_In_ HCRYPTPROV hProv,
	_In_ const UInt8 *pbKey,
	_In_ ULONG cbKey)
{
	ULONG cb;
	PCRYPT_PRIVATE_KEY_INFO PrivateKeyInfo;

	BOOL succ = CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, PKCS_PRIVATE_KEY_INFO,
		pbKey, cbKey, CRYPT_DECODE_ALLOC_FLAG | CRYPT_DECODE_NOCOPY_FLAG, 0, (void**)&PrivateKeyInfo, &cb);

	if (succ)
	{
		PUBLICKEYSTRUC* ppks;

		succ = CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
			PKCS_RSA_PRIVATE_KEY, PrivateKeyInfo->PrivateKey.pbData, PrivateKeyInfo->PrivateKey.cbData,
			CRYPT_DECODE_ALLOC_FLAG, 0, (void**)&ppks, &cb);

		LocalFree(PrivateKeyInfo);

		if (succ)
		{
			succ = CryptImportKey(hProv, (PUCHAR)ppks, cb, 0, CRYPT_EXPORTABLE, phKey);
			LocalFree(ppks);
		}
	}

	return succ;
}

Bool Net::WinSSLEngine::SetServerCertsASN1(Crypto::X509File *certASN1, Crypto::X509File *keyASN1)
{
	if (this->clsData->svrInit)
	{
		return false;
	}
	if (certASN1 == 0 || keyASN1 == 0)
	{
		return false;
	}
	const WChar *containerName = L"SelfSign";
//	const WChar *containerName = L"ServerCert";
	HCRYPTKEY hKey;
	HCRYPTPROV hProv;
	if (!CryptAcquireContext(&hProv, containerName, NULL, PROV_RSA_FULL, CRYPT_MACHINE_KEYSET))
	{
		if (!CryptAcquireContext(&hProv, containerName, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET | CRYPT_MACHINE_KEYSET))
		{
			return false;
		}
	}
	if (!WinSSLEngine_CryptImportPrivateKey(&hKey, hProv, keyASN1->GetASN1Buff(), (ULONG)keyASN1->GetASN1BuffSize()))
	{
		CryptReleaseContext(hProv, 0);
		return false;
	}


	PCCERT_CONTEXT serverCert = CertCreateCertificateContext(X509_ASN_ENCODING, certASN1->GetASN1Buff(), (DWORD)certASN1->GetASN1BuffSize());
	CRYPT_KEY_PROV_INFO keyProvInfo;
	MemClear(&keyProvInfo, sizeof(keyProvInfo));
	keyProvInfo.pwszContainerName = (WChar*)containerName;
	keyProvInfo.pwszProvName = NULL;
	keyProvInfo.dwProvType = PROV_RSA_FULL;
	keyProvInfo.dwFlags = CRYPT_MACHINE_KEYSET;
	keyProvInfo.cProvParam = 0;
	keyProvInfo.rgProvParam = NULL;
	keyProvInfo.dwKeySpec = AT_SIGNATURE;
	BOOL succ = CertSetCertificateContextProperty(serverCert, CERT_KEY_PROV_INFO_PROP_ID, 0, &keyProvInfo);

	HCRYPTPROV_OR_NCRYPT_KEY_HANDLE hCryptProvOrNCryptKey = NULL;
	BOOL fCallerFreeProvOrNCryptKey = FALSE;
	DWORD dwKeySpec;
	succ = CryptAcquireCertificatePrivateKey(serverCert, 0, NULL, &hCryptProvOrNCryptKey, &dwKeySpec, &fCallerFreeProvOrNCryptKey);

	if (!this->InitServer(this->clsData->method, (void*)serverCert) &&
		!this->InitServer(M_TLSV1_2, (void*)serverCert) &&
		!this->InitServer(M_TLSV1_1, (void*)serverCert) &&
		!this->InitServer(M_TLSV1, (void*)serverCert))
	{
		CertFreeCertificateContext(serverCert);
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	this->clsData->svrInit = true;
	CertFreeCertificateContext(serverCert);
	CryptDestroyKey(hKey);
	CryptReleaseContext(hProv, 0);
	return true;
}

UTF8Char *Net::WinSSLEngine::GetErrorDetail(UTF8Char *sbuff)
{
	*sbuff = 0;
	return sbuff;
}

Net::TCPClient *Net::WinSSLEngine::Connect(const UTF8Char *hostName, UInt16 port, ErrorType *err)
{
	if (!this->clsData->cliInit)
	{
		if (!this->InitClient(this->clsData->method) &&
			!this->InitClient(M_TLSV1_2) &&
			!this->InitClient(M_TLSV1_1) &&
			!this->InitClient(M_TLSV1))
		{
			return 0;
		}
		this->clsData->cliInit = true;
	}

	Net::SocketUtil::AddressInfo addr;
	UInt32 *s;
	if (!this->sockf->DNSResolveIP(hostName, &addr))
	{
		if (err)
			*err = ET_HOSTNAME_NOT_RESOLVED;
		return 0;
	}
	if (addr.addrType == Net::SocketUtil::AT_IPV4)
	{
		s = this->sockf->CreateTCPSocketv4();
	}
	else if (addr.addrType == Net::SocketUtil::AT_IPV6)
	{
		s = this->sockf->CreateTCPSocketv6();
	}
	else
	{
		if (err)
			*err = ET_HOSTNAME_NOT_RESOLVED;
		return 0;
	}
	if (s == 0)
	{
		if (err)
			*err = ET_OUT_OF_MEMORY;
		return 0;
	}
	if (!this->sockf->Connect(s, &addr, port))
	{
		this->sockf->DestroySocket(s);
		if (err)
			*err = ET_CANNOT_CONNECT;
		return 0;
	}

	CtxtHandle ctxt;
	const WChar *wptr = Text::StrToWCharNew(hostName);
	UInt32 retFlags = ISC_REQ_SEQUENCE_DETECT | ISC_REQ_REPLAY_DETECT | ISC_REQ_CONFIDENTIALITY | ISC_REQ_ALLOCATE_MEMORY | ISC_REQ_STREAM;
	TimeStamp ts;
	SecBuffer outputBuff[3];
	SecBuffer_Set(&outputBuff[0], SECBUFFER_EMPTY, 0, 0);
	SecBufferDesc inputDesc;
	SecBufferDesc outputDesc;
	SecBufferDesc_Set(&outputDesc, outputBuff, 1);

	if (this->skipCertCheck)
	{
		retFlags |= ISC_REQ_MANUAL_CRED_VALIDATION;
	}

	SECURITY_STATUS status;
	status = InitializeSecurityContext(
		&this->clsData->hCredCli,
		0,
		(WChar*)wptr,
		(unsigned long)retFlags,
		0,
		0,
		0,
		0,
		&ctxt,
		&outputDesc,
		(unsigned long*)&retFlags,
		&ts
	);
	if (status != SEC_I_CONTINUE_NEEDED)
	{
		Text::StrDelNew(wptr);
		return 0;
	}
	Net::SocketFactory::ErrorType et;
	if (this->sockf->SendData(s, (UInt8*)outputBuff[0].pvBuffer, outputBuff[0].cbBuffer, &et) != outputBuff[0].cbBuffer)
	{
		DeleteSecurityContext(&ctxt);
		FreeContextBuffer(outputBuff[0].pvBuffer);
		Text::StrDelNew(wptr);
		return 0;
	}
	FreeContextBuffer(outputBuff[0].pvBuffer);

	this->sockf->SetRecvTimeout(s, 3000);
	SecBuffer inputBuff[2];
	UInt8 recvBuff[2048];
	UOSInt recvOfst = 0;
	UOSInt recvSize;
	UOSInt i;
	while (status == SEC_I_CONTINUE_NEEDED || status == SEC_E_INCOMPLETE_MESSAGE)
	{
		if (recvOfst == 0 || status == SEC_E_INCOMPLETE_MESSAGE)
		{
			recvSize = this->sockf->ReceiveData(s, &recvBuff[recvOfst], 2048 - recvOfst, &et);
			if (recvSize <= 0)
			{
				Text::StrDelNew(wptr);
				return 0;
			}
			recvOfst += recvSize;
		}

		SecBuffer_Set(&inputBuff[0], SECBUFFER_TOKEN, recvBuff, (UInt32)recvOfst);
		SecBuffer_Set(&inputBuff[1], SECBUFFER_EMPTY, 0, 0);
		SecBufferDesc_Set(&inputDesc, inputBuff, 2);

		SecBuffer_Set(&outputBuff[0], SECBUFFER_TOKEN, 0, 0);
#if defined(SECBUFFER_ALERT)
		SecBuffer_Set(&outputBuff[1], SECBUFFER_ALERT, 0, 0);
#else
		SecBuffer_Set(&outputBuff[1], SECBUFFER_EMPTY, 0, 0);
#endif
		SecBuffer_Set(&outputBuff[2], SECBUFFER_EMPTY, 0, 0);
		SecBufferDesc_Set(&outputDesc, outputBuff, 3);

		status = InitializeSecurityContext(
			&this->clsData->hCredCli,
			&ctxt,
			(WChar*)wptr,
			retFlags,
			0,
			0,
			&inputDesc,
			0,
			0,
			&outputDesc,
			(unsigned long*)&retFlags,
			&ts);

		if (status == SEC_E_INCOMPLETE_MESSAGE)
		{

		}
		else if (status == SEC_I_CONTINUE_NEEDED || status == SEC_E_OK)
		{
			Bool succ = true;
			i = 0;
			while (i < 3)
			{
				if (outputBuff[i].BufferType == SECBUFFER_TOKEN && outputBuff[i].cbBuffer > 0)
				{
					if (this->sockf->SendData(s, (const UInt8*)outputBuff[i].pvBuffer, outputBuff[i].cbBuffer, &et) != outputBuff[i].cbBuffer)
					{
						succ = false;
					}
				}

				if (outputBuff[i].pvBuffer)
				{
					FreeContextBuffer(outputBuff[i].pvBuffer);
				}
				i++;
			}
			if (!succ)
			{
				DeleteSecurityContext(&ctxt);
				Text::StrDelNew(wptr);
				return 0;
			}
			if (inputBuff[1].BufferType == SECBUFFER_EXTRA)
			{
				MemCopyNO(recvBuff, inputBuff[1].pvBuffer, inputBuff[1].cbBuffer);
				recvOfst = inputBuff[1].cbBuffer;
			}
			else
			{
				recvOfst = 0;
			}
		}
		else
		{
			if (status == SEC_I_INCOMPLETE_CREDENTIALS)
			{

			}
			DeleteSecurityContext(&ctxt);
			Text::StrDelNew(wptr);
			return 0;
		}
	}
	Text::StrDelNew(wptr);

	Net::TCPClient *cli;
	NEW_CLASS(cli, Net::WinSSLClient(sockf, s, &ctxt));
	return cli;
}

Bool Net::WinSSLEngine::GenerateCert(const UTF8Char *country, const UTF8Char *company, const UTF8Char *commonName, Crypto::X509File **certASN1, Crypto::X509File **keyASN1)
{
	HCRYPTKEY hKey;
	HCRYPTPROV hProv;
	if (!CryptAcquireContext(&hProv, L"SelfSign", NULL, PROV_RSA_FULL, CRYPT_MACHINE_KEYSET))
	{
		if (!CryptAcquireContext(&hProv, L"SelfSign", NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET | CRYPT_MACHINE_KEYSET))
		{
			return false;
		}
	}
	if (!CryptGenKey(hProv, AT_SIGNATURE, CRYPT_EXPORTABLE, &hKey))
	{
		CryptReleaseContext(hProv, 0);
		return false;
	}

	Text::StringBuilderW sb;
	sb.Append((const UTF8Char*)"C=");
	sb.Append(country);
	sb.Append((const UTF8Char*)", O=");
	sb.Append(company);
	sb.Append((const UTF8Char*)", CN=");
	sb.Append(commonName);

	PCCERT_CONTEXT pCertContext = NULL;
	BYTE *pbEncoded = NULL;
	DWORD cbEncoded = 0;
	HCERTSTORE hStore = NULL;
	HCRYPTPROV_OR_NCRYPT_KEY_HANDLE hCryptProvOrNCryptKey = NULL;
	BOOL fCallerFreeProvOrNCryptKey = FALSE;

	if (!CertStrToName(X509_ASN_ENCODING, sb.ToString(), CERT_X500_NAME_STR, NULL, pbEncoded, &cbEncoded, NULL))
	{
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	pbEncoded = MemAlloc(BYTE, cbEncoded);
	if (!CertStrToName(X509_ASN_ENCODING, sb.ToString(), CERT_X500_NAME_STR, NULL, pbEncoded, &cbEncoded, NULL))
	{
		MemFree(pbEncoded);
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}

	// Prepare certificate Subject for self-signed certificate
	CERT_NAME_BLOB subjectIssuerBlob;
	MemClear(&subjectIssuerBlob, sizeof(subjectIssuerBlob));
	subjectIssuerBlob.cbData = cbEncoded;
	subjectIssuerBlob.pbData = pbEncoded;

	CRYPT_KEY_PROV_INFO keyProvInfo;
	MemClear(&keyProvInfo, sizeof(keyProvInfo));
	keyProvInfo.pwszContainerName = (WChar*)L"SelfSign";
	keyProvInfo.pwszProvName = NULL;
	keyProvInfo.dwProvType = PROV_RSA_FULL;
	keyProvInfo.dwFlags = CRYPT_MACHINE_KEYSET;
	keyProvInfo.cProvParam = 0;
	keyProvInfo.rgProvParam = NULL;
	keyProvInfo.dwKeySpec = AT_SIGNATURE;

	CRYPT_ALGORITHM_IDENTIFIER signatureAlgorithm;
	MemClear(&signatureAlgorithm, sizeof(signatureAlgorithm));
	signatureAlgorithm.pszObjId = (Char*)szOID_RSA_SHA256RSA;

	SYSTEMTIME endTime;
	GetSystemTime(&endTime);
	endTime.wYear += 1;

	pCertContext = CertCreateSelfSignCertificate(NULL, &subjectIssuerBlob, 0, &keyProvInfo, &signatureAlgorithm, 0, &endTime, 0);
	if (!pCertContext)
	{
		MemFree(pbEncoded);
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	UInt8 privKeyBuff[2048];
	UInt8 certBuff[4096];
	DWORD certBuffSize = 4096;
	DWORD privKeySize = 2048;
	if (!CryptExportKey(hKey, 0, PRIVATEKEYBLOB, 0, privKeyBuff, &privKeySize))
	{
		CertFreeCertificateContext(pCertContext);
		MemFree(pbEncoded);
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	if (!CryptEncodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, PKCS_RSA_PRIVATE_KEY, privKeyBuff, 0, 0, certBuff, &certBuffSize))
	{
		CertFreeCertificateContext(pCertContext);
		MemFree(pbEncoded);
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	Net::ASN1PDUBuilder keyPDU;
	keyPDU.BeginSequence();
	keyPDU.AppendInt32(0);
	keyPDU.BeginSequence();
	keyPDU.AppendOIDString("1.2.840.113549.1.1.1");
	keyPDU.AppendNull();
	keyPDU.EndLevel();
	keyPDU.AppendOctetString(certBuff, certBuffSize);
	keyPDU.EndLevel();
	*certASN1 = Crypto::X509File::LoadFile((const UTF8Char *)"SelfSigned", pCertContext->pbCertEncoded, pCertContext->cbCertEncoded, Crypto::X509File::FT_CERT);
	*keyASN1 = Crypto::X509File::LoadFile((const UTF8Char *)"SelfSignedKey", keyPDU.GetBuff(0), keyPDU.GetBuffSize(), Crypto::X509File::FT_PRIV_KEY);
	CertFreeCertificateContext(pCertContext);
	//CryptReleaseContext(hCryptProvOrNCryptKey, 0);
	MemFree(pbEncoded);
	CryptDestroyKey(hKey);
	CryptReleaseContext(hProv, 0);
	return true;
}
