#include "Stdafx.h"
#include "Crypto/Cert/X509Cert.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "Data/DateTime.h"
//#include "IO/DebugWriter.h"
#include "Net/WinSSLClient.h"
#include "Net/WinSSLEngine.h"
#include "Text/MyString.h"
#include "Text/StringBuilderW.h"
#include <windows.h>
#include <ntsecapi.h>
#define SECURITY_WIN32
#include <sspi.h>
#include <schnlsp.h>

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

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
		case Net::SSLEngine::Method::Default:
			return 0;
		case Net::SSLEngine::Method::SSLV3:
			return SP_PROT_SSL3_SERVER;
		case Net::SSLEngine::Method::SSLV23:
			return SP_PROT_SSL2_SERVER;
		#if defined(SP_PROT_TLS1_2)
		case Net::SSLEngine::Method::TLS:
			return SP_PROT_TLS1_1_SERVER;
		case Net::SSLEngine::Method::TLSV1:
			return SP_PROT_TLS1_0_SERVER;
		case Net::SSLEngine::Method::TLSV1_1:
			return SP_PROT_TLS1_1_SERVER;
		case Net::SSLEngine::Method::TLSV1_2:
			return SP_PROT_TLS1_2_SERVER;
		case Net::SSLEngine::Method::DTLS:
			return SP_PROT_DTLS1_0_SERVER;
		case Net::SSLEngine::Method::DTLSV1:
			return SP_PROT_DTLS1_0_SERVER;
		case Net::SSLEngine::Method::DTLSV1_2:
			return SP_PROT_DTLS1_2_SERVER;
		default:
			return SP_PROT_TLS1_0_SERVER || SP_PROT_TLS1_1_SERVER || SP_PROT_TLS1_2_SERVER || SP_PROT_TLS1_3_SERVER;
		#else
		case Net::SSLEngine::Method::TLS:
			return SP_PROT_TLS1;
		case Net::SSLEngine::Method::TLSV1:
			return SP_PROT_TLS1;
		case Net::SSLEngine::Method::TLSV1_1:
			return SP_PROT_TLS1;
		case Net::SSLEngine::Method::TLSV1_2:
			return SP_PROT_TLS1;
		case Net::SSLEngine::Method::DTLS:
			return SP_PROT_TLS1;
		case Net::SSLEngine::Method::DTLSV1:
			return SP_PROT_TLS1;
		case Net::SSLEngine::Method::DTLSV1_2:
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
		case Net::SSLEngine::Method::Default:
			return 0;
		case Net::SSLEngine::Method::SSLV3:
			return SP_PROT_SSL3_CLIENT;
		case Net::SSLEngine::Method::SSLV23:
			return SP_PROT_SSL3_CLIENT;
		#if defined(SP_PROT_TLS1_2)
		case Net::SSLEngine::Method::TLS:
			return SP_PROT_TLS1_2_CLIENT;
		case Net::SSLEngine::Method::TLSV1:
			return SP_PROT_TLS1_0_CLIENT;
		case Net::SSLEngine::Method::TLSV1_1:
			return SP_PROT_TLS1_1_CLIENT;
		case Net::SSLEngine::Method::TLSV1_2:
			return SP_PROT_TLS1_2_CLIENT;
		case Net::SSLEngine::Method::DTLS:
			return SP_PROT_DTLS1_2_CLIENT;
		case Net::SSLEngine::Method::DTLSV1:
			return SP_PROT_DTLS1_0_CLIENT;
		case Net::SSLEngine::Method::DTLSV1_2:
			return SP_PROT_DTLS1_2_CLIENT;
		default:
			return SP_PROT_TLS1_2_CLIENT;
		#else
		case Net::SSLEngine::Method::TLS:
			return SP_PROT_TLS1_CLIENT;
		case Net::SSLEngine::Method::TLSV1:
			return SP_PROT_TLS1_CLIENT;
		case Net::SSLEngine::Method::TLSV1_1:
			return SP_PROT_TLS1_CLIENT;
		case Net::SSLEngine::Method::TLSV1_2:
			return SP_PROT_TLS1_CLIENT;
		case Net::SSLEngine::Method::DTLS:
			return SP_PROT_TLS1_CLIENT;
		case Net::SSLEngine::Method::DTLSV1:
			return SP_PROT_TLS1_CLIENT;
		case Net::SSLEngine::Method::DTLSV1_2:
			return SP_PROT_TLS1_CLIENT;
		default:
			return SP_PROT_TLS1_CLIENT;
		#endif
		}
	}
}

void Net::WinSSLEngine::DeinitClient()
{
	if (this->clsData->cliInit)
	{
		FreeCredentialsHandle(&this->clsData->hCredCli);
		this->clsData->cliInit = false;
	}
}
Bool Net::WinSSLEngine::InitClient(Method method, void *cred)
{
	SCHANNEL_CRED credData;
	SECURITY_STATUS status;
	TimeStamp lifetime;
	MemClear(&credData, sizeof(credData));
	credData.dwVersion = SCHANNEL_CRED_VERSION;
	credData.grbitEnabledProtocols = WinSSLEngine_GetProtocols(method, false);
	if (cred)
	{
		credData.paCred = (PCCERT_CONTEXT*)&cred;
		credData.cCreds = 1;
	}

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

Bool Net::WinSSLEngine::InitServer(Method method, void *cred, void *caCred)
{
	SCHANNEL_CRED credData;
	SECURITY_STATUS status;
	TimeStamp lifetime;
	MemClear(&credData, sizeof(credData));
	credData.dwVersion = SCHANNEL_CRED_VERSION;
	credData.grbitEnabledProtocols = WinSSLEngine_GetProtocols(method, true);
	credData.paCred = (PCCERT_CONTEXT*)&cred;
	credData.cCreds = 1;

	if (caCred)
	{
		
	}

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
	if (status == SEC_E_INTERNAL_ERROR)
	{

	}
	return status == 0;
}

Net::SSLClient *Net::WinSSLEngine::CreateClientConn(void *sslObj, Socket *s, const UTF8Char *hostName, ErrorType *err)
{
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
#if defined(VERBOSE)
		printf("SSL: Error in InitializeSecurityContext, ret = %x\r\n", (UInt32)status);
#endif
		Text::StrDelNew(wptr);
		return 0;
	}
	Net::SocketFactory::ErrorType et;
#if defined(VERBOSE)
	printf("SSL: SendData, size = %d\r\n", (Int32)outputBuff[0].cbBuffer);
#endif
	if (this->sockf->SendData(s, (UInt8*)outputBuff[0].pvBuffer, outputBuff[0].cbBuffer, &et) != outputBuff[0].cbBuffer)
	{
#if defined(VERBOSE)
		printf("SSL: Error in sendData, ret = %x\r\n", (UInt32)status);
#endif
		DeleteSecurityContext(&ctxt);
		FreeContextBuffer(outputBuff[0].pvBuffer);
		Text::StrDelNew(wptr);
		return 0;
	}
	FreeContextBuffer(outputBuff[0].pvBuffer);

	this->sockf->SetRecvTimeout(s, 3000);
	SecBuffer inputBuff[2];
	UInt8 recvBuff[8192];
	UOSInt recvOfst = 0;
	UOSInt recvSize;
	UOSInt i;
	while (status == SEC_I_CONTINUE_NEEDED || status == SEC_E_INCOMPLETE_MESSAGE)
	{
		if (recvOfst == 0 || status == SEC_E_INCOMPLETE_MESSAGE)
		{
			recvSize = this->sockf->ReceiveData(s, &recvBuff[recvOfst], 8192 - recvOfst, &et);
#if defined(VERBOSE)
			printf("SSL: recvData, size = %d\r\n", (UInt32)recvSize);
#endif
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
				MemCopyO(recvBuff, &recvBuff[recvOfst - inputBuff[1].cbBuffer], inputBuff[1].cbBuffer);
				recvOfst = inputBuff[1].cbBuffer;
			}
			else
			{
				recvOfst = 0;
			}
		}
		else
		{
#if defined(VERBOSE)
			printf("SSL: Error in InitializeSecurityContext 2, ret = %x\r\n", (UInt32)status);
#endif
			if (status == SEC_I_INCOMPLETE_CREDENTIALS)
			{

			}
			DeleteSecurityContext(&ctxt);
			Text::StrDelNew(wptr);
			return 0;
		}
	}
	Text::StrDelNew(wptr);

	sockf->SetRecvTimeout(s, 120000);
	Net::SSLClient *cli;
	NEW_CLASS(cli, Net::WinSSLClient(sockf, s, &ctxt));
	return cli;
}

Net::SSLClient *Net::WinSSLEngine::CreateServerConn(Socket *s)
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

/*	IO::DebugWriter debug;
	Text::StringBuilderW sb;
	sb.ClearStr();
	sb.Append((const UTF8Char*)"Received ");
	sb.AppendUOSInt(recvSize);
	sb.Append((const UTF8Char*)" bytes");
	debug.WriteLineW(sb.ToString());*/

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

/*			sb.ClearStr();
			sb.Append((const UTF8Char*)"Received ");
			sb.AppendUOSInt(recvSize);
			sb.Append((const UTF8Char*)" bytes");
			debug.WriteLineW(sb.ToString());*/
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
/*			sb.ClearStr();
			sb.Append((const UTF8Char*)"Error in initializing SSL Server connection: 0x");
			sb.AppendHex32(status);
			debug.WriteLineW(sb.ToString());*/
			if (status == SEC_I_INCOMPLETE_CREDENTIALS)
			{

			}
			DeleteSecurityContext(&ctxt);
			this->sockf->DestroySocket(s);
			return 0;
		}
	}

	Net::SSLClient *cli;
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

void WinSSLEngine_HCRYPTKEY_ToString(HCRYPTKEY hKey, Text::StringBuilderUTF *sb)
{
	UInt8 buff[4096];
	DWORD dataLen;
	ALG_ID algId;
	dataLen = sizeof(algId);
	if (CryptGetKeyParam(hKey, KP_ALGID, (BYTE*)&algId, &dataLen, 0))
	{
		sb->Append((const UTF8Char*)"Key ALG ID = 0x");
		sb->AppendHex32(algId);
		sb->Append((const UTF8Char*)", Class=");
		switch (GET_ALG_CLASS(algId))
		{
		case ALG_CLASS_ANY:
			sb->Append((const UTF8Char*)"Any");
			break;
		case ALG_CLASS_SIGNATURE:
			sb->Append((const UTF8Char*)"Signature");
			break;
		case ALG_CLASS_MSG_ENCRYPT:
			sb->Append((const UTF8Char*)"Message Encrypt");
			break;
		case ALG_CLASS_DATA_ENCRYPT:
			sb->Append((const UTF8Char*)"Data Encrypt");
			break;
		case ALG_CLASS_HASH:
			sb->Append((const UTF8Char*)"Hash");
			break;
		case ALG_CLASS_KEY_EXCHANGE:
			sb->Append((const UTF8Char*)"Key Exchange");
			break;
		case ALG_CLASS_ALL:
			sb->Append((const UTF8Char*)"All");
			break;
		default:
			sb->Append((const UTF8Char*)"Unknown");
			break;
		}
		sb->Append((const UTF8Char*)", Type=");
		switch (GET_ALG_TYPE(algId))
		{
		case ALG_TYPE_ANY:
			sb->Append((const UTF8Char*)"Any");
			break;
		case ALG_TYPE_DSS:
			sb->Append((const UTF8Char*)"DSS");
			break;
		case ALG_TYPE_RSA:
			sb->Append((const UTF8Char*)"RSA");
			break;
		case ALG_TYPE_BLOCK:
			sb->Append((const UTF8Char*)"Block");
			break;
		case ALG_TYPE_STREAM:
			sb->Append((const UTF8Char*)"Stream");
			break;
		case ALG_TYPE_DH:
			sb->Append((const UTF8Char*)"DH");
			break;
		case ALG_TYPE_SECURECHANNEL:
			sb->Append((const UTF8Char*)"Secure Channel");
			break;
#if (NTDDI_VERSION >= NTDDI_VISTA)
		case ALG_TYPE_ECDH:
			sb->Append((const UTF8Char*)"ECDH");
			break;
#endif
#if defined(_MSC_VER) && (NTDDI_VERSION >= NTDDI_WIN10_RS1)
		case ALG_TYPE_THIRDPARTY:
			sb->Append((const UTF8Char*)"ThirdParty");
			break;
#endif
		default:
			sb->Append((const UTF8Char*)"Unknown");
			break;
		}
		sb->Append((const UTF8Char*)", SID=");
		sb->AppendU32(GET_ALG_SID(algId));
		sb->Append((const UTF8Char*)"\r\n");
	}
	DWORD blockLen;
	dataLen = sizeof(blockLen);
	if (CryptGetKeyParam(hKey, KP_BLOCKLEN, (BYTE*)&blockLen, &dataLen, 0))
	{
		sb->Append((const UTF8Char*)"Key BlockLen=");
		sb->AppendU32(blockLen);
		sb->Append((const UTF8Char*)"\r\n");
	}
	dataLen = sizeof(buff);
	if (CryptGetKeyParam(hKey, KP_CERTIFICATE, buff, &dataLen, 0))
	{
		sb->Append((const UTF8Char*)"Key Certificate=\r\n");
		sb->AppendHexBuff(buff, dataLen, ' ', Text::LineBreakType::CRLF);
		sb->Append((const UTF8Char*)"\r\n");
	}
	dataLen = sizeof(blockLen);
	if (CryptGetKeyParam(hKey, KP_KEYLEN, (BYTE*)&blockLen, &dataLen, 0))
	{
		sb->Append((const UTF8Char*)"Key Len=");
		sb->AppendU32(blockLen);
		sb->Append((const UTF8Char*)"\r\n");
	}
	dataLen = sizeof(buff);
	if (CryptGetKeyParam(hKey, KP_SALT, buff, &dataLen, 0))
	{
		sb->Append((const UTF8Char*)"Key Salt=");
		sb->AppendHexBuff(buff, dataLen, ' ', Text::LineBreakType::None);
		sb->Append((const UTF8Char*)"\r\n");
	}
	dataLen = sizeof(blockLen);
	if (CryptGetKeyParam(hKey, KP_PERMISSIONS, (BYTE*)&blockLen, &dataLen, 0))
	{
		sb->Append((const UTF8Char*)"Key Permissions=0x");
		sb->AppendHex32(blockLen);
		sb->Append((const UTF8Char*)"\r\n");
	}
}

void WinSSLEngine_HCRYPTPROV_ToString(HCRYPTPROV hProv, Text::StringBuilderUTF *sb)
{
	HCRYPTKEY hKey;
	if (CryptGetUserKey(hProv, AT_KEYEXCHANGE, &hKey))
	{
		sb->Append((const UTF8Char*)"KEYEXCHANGE: Success,\r\n");
		WinSSLEngine_HCRYPTKEY_ToString(hKey, sb);
		CryptDestroyKey(hKey);
	}
	else
	{
		sb->Append((const UTF8Char*)"KEYEXCHANGE: Failed, code = 0x");
		sb->AppendHex32(GetLastError());
		sb->Append((const UTF8Char*)"\r\n");
	}

	if (CryptGetUserKey(hProv, AT_SIGNATURE, &hKey))
	{
		sb->Append((const UTF8Char*)"SIGNATURE: Success,\r\n");
		WinSSLEngine_HCRYPTKEY_ToString(hKey, sb);
		CryptDestroyKey(hKey);
	}
	else
	{
		sb->Append((const UTF8Char*)"SIGNATURE: Failed, code = 0x");
		sb->AppendHex32(GetLastError());
		sb->Append((const UTF8Char*)"\r\n");
	}
}

Bool WinSSLEngine_CryptImportPrivateKey(_Out_ HCRYPTKEY* phKey,
	_In_ HCRYPTPROV hProv,
	_In_ const UInt8 *pbKey,
	_In_ ULONG cbKey,
	Bool signature)
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
			if (signature && ppks->aiKeyAlg == CALG_RSA_KEYX)
			{
				ppks->aiKeyAlg = CALG_RSA_SIGN;
			}
			succ = CryptImportKey(hProv, (PUCHAR)ppks, cb, 0, CRYPT_EXPORTABLE, phKey);
			if (!succ)
			{
#if defined(VERBOSE)
				printf("SSL: Import Key failed: CryptImportKey\r\n");
#endif
			}
			LocalFree(ppks);
		}
		else
		{
#if defined(VERBOSE)
			printf("SSL: Import Key failed: CryptDecodeObjectEx\r\n");
#endif
		}

/*		if (succ)
		{
			ALG_ID algId = AT_SIGNATURE;
			BOOL succ2 = CryptSetKeyParam(*phKey, KP_ALGID, (const BYTE*)&algId, CRYPT_EXPORTABLE);
			if (succ2)
			{
				succ = TRUE;
			}
		}*/
	}
	else
	{
#if defined(VERBOSE)
		printf("SSL: Import Key failed: GetKeyDecodeSize\r\n");
#endif
	}

	return succ;
}

Bool Net::WinSSLEngine::SetServerCertsASN1(Crypto::Cert::X509Cert *certASN1, Crypto::Cert::X509File *keyASN1, Crypto::Cert::X509Cert *caCert)
{
	if (this->clsData->svrInit)
	{
		return false;
	}
	if (certASN1 == 0 || keyASN1 == 0)
	{
		return false;
	}
//	const WChar *containerName = L"SelfSign";
	const WChar *containerName = L"ServerCert";
	HCRYPTKEY hKey;
	HCRYPTPROV hProv;
	if (!CryptAcquireContext(&hProv, containerName, NULL, PROV_RSA_FULL, CRYPT_MACHINE_KEYSET))
	{
		if (!CryptAcquireContext(&hProv, containerName, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET | CRYPT_MACHINE_KEYSET))
		{
			return false;
		}
	}
	if (keyASN1->GetFileType() == Crypto::Cert::X509File::FileType::PrivateKey)
	{
		if (!WinSSLEngine_CryptImportPrivateKey(&hKey, hProv, keyASN1->GetASN1Buff(), (ULONG)keyASN1->GetASN1BuffSize(), false))
		{
			CryptReleaseContext(hProv, 0);
			return false;
		}
	}
	else if (keyASN1->GetFileType() == Crypto::Cert::X509File::FileType::Key)
	{
		Crypto::Cert::X509PrivKey *privKey = Crypto::Cert::X509PrivKey::CreateFromKey((Crypto::Cert::X509Key*)keyASN1);
		if (!WinSSLEngine_CryptImportPrivateKey(&hKey, hProv, privKey->GetASN1Buff(), (ULONG)privKey->GetASN1BuffSize(), false))
		{
			DEL_CLASS(privKey);
			CryptReleaseContext(hProv, 0);
			return false;
		}
		DEL_CLASS(privKey);
	}
/*	IO::DebugWriter debug;
	Text::StringBuilderUTF16 sbDebug;
	WinSSLEngine_HCRYPTPROV_ToString(hProv, &sbDebug);
	debug.WriteLineW(sbDebug.ToString());*/

	PCCERT_CONTEXT serverCert = CertCreateCertificateContext(X509_ASN_ENCODING, certASN1->GetASN1Buff(), (DWORD)certASN1->GetASN1BuffSize());
	CRYPT_KEY_PROV_INFO keyProvInfo;
	MemClear(&keyProvInfo, sizeof(keyProvInfo));
	keyProvInfo.pwszContainerName = (WChar*)containerName;
	keyProvInfo.pwszProvName = NULL;
	keyProvInfo.dwProvType = PROV_RSA_FULL;
	keyProvInfo.dwFlags = CRYPT_MACHINE_KEYSET;
	keyProvInfo.cProvParam = 0;
	keyProvInfo.rgProvParam = NULL;
	keyProvInfo.dwKeySpec = AT_KEYEXCHANGE;
	CertSetCertificateContextProperty(serverCert, CERT_KEY_PROV_INFO_PROP_ID, 0, &keyProvInfo);

	HCRYPTPROV_OR_NCRYPT_KEY_HANDLE hCryptProvOrNCryptKey = 0;
	BOOL fCallerFreeProvOrNCryptKey = FALSE;
	DWORD dwKeySpec;
	CryptAcquireCertificatePrivateKey(serverCert, 0, NULL, &hCryptProvOrNCryptKey, &dwKeySpec, &fCallerFreeProvOrNCryptKey);

	PCCERT_CONTEXT caCertCred = 0;
	if (caCert)
	{
		caCertCred = CertCreateCertificateContext(X509_ASN_ENCODING, caCert->GetASN1Buff(), (DWORD)caCert->GetASN1BuffSize());
	}
	if (!this->InitServer(this->clsData->method, (void*)serverCert, (void*)caCertCred))
	{
		CertFreeCertificateContext(serverCert);
		if (caCertCred)
		{
			CertFreeCertificateContext(caCertCred);
		}
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	this->clsData->svrInit = true;
	CertFreeCertificateContext(serverCert);
	if (caCertCred)
	{
		CertFreeCertificateContext(caCertCred);
	}
	CryptDestroyKey(hKey);
	CryptReleaseContext(hProv, 0);
	return true;
}

Bool Net::WinSSLEngine::SetClientCertASN1(Crypto::Cert::X509Cert *certASN1, Crypto::Cert::X509File *keyASN1)
{
	if (certASN1 == 0 || keyASN1 == 0)
	{
		return false;
	}
	const WChar *containerName = L"ClientCert";
	HCRYPTKEY hKey;
	HCRYPTPROV hProv;
	if (!CryptAcquireContext(&hProv, containerName, NULL, PROV_RSA_FULL, CRYPT_MACHINE_KEYSET))
	{
		if (!CryptAcquireContext(&hProv, containerName, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET | CRYPT_MACHINE_KEYSET))
		{
			return false;
		}
	}
	if (!WinSSLEngine_CryptImportPrivateKey(&hKey, hProv, keyASN1->GetASN1Buff(), (ULONG)keyASN1->GetASN1BuffSize(), false))
	{
		CryptReleaseContext(hProv, 0);
		return false;
	}

/*	IO::DebugWriter debug;
	Text::StringBuilderUTF16 sbDebug;
	WinSSLEngine_HCRYPTPROV_ToString(hProv, &sbDebug);
	debug.WriteLineW(sbDebug.ToString());*/

	PCCERT_CONTEXT serverCert = CertCreateCertificateContext(X509_ASN_ENCODING, certASN1->GetASN1Buff(), (DWORD)certASN1->GetASN1BuffSize());
	CRYPT_KEY_PROV_INFO keyProvInfo;
	MemClear(&keyProvInfo, sizeof(keyProvInfo));
	keyProvInfo.pwszContainerName = (WChar*)containerName;
	keyProvInfo.pwszProvName = NULL;
	keyProvInfo.dwProvType = PROV_RSA_FULL;
	keyProvInfo.dwFlags = CRYPT_MACHINE_KEYSET;
	keyProvInfo.cProvParam = 0;
	keyProvInfo.rgProvParam = NULL;
	keyProvInfo.dwKeySpec = AT_KEYEXCHANGE;
	CertSetCertificateContextProperty(serverCert, CERT_KEY_PROV_INFO_PROP_ID, 0, &keyProvInfo);

	HCRYPTPROV_OR_NCRYPT_KEY_HANDLE hCryptProvOrNCryptKey = 0;
	BOOL fCallerFreeProvOrNCryptKey = FALSE;
	DWORD dwKeySpec;
	CryptAcquireCertificatePrivateKey(serverCert, 0, NULL, &hCryptProvOrNCryptKey, &dwKeySpec, &fCallerFreeProvOrNCryptKey);

	this->DeinitClient();
	if (!this->InitClient(this->clsData->method, (void*)serverCert) &&
			!this->InitClient(Method::TLSV1_2, (void*)serverCert) &&
			!this->InitClient(Method::TLSV1_1, (void*)serverCert) &&
			!this->InitClient(Method::TLSV1, (void*)serverCert))
	{
		CertFreeCertificateContext(serverCert);
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	this->clsData->cliInit = true;
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

Net::SSLClient *Net::WinSSLEngine::Connect(const UTF8Char *hostName, UInt16 port, ErrorType *err)
{
	if (!this->clsData->cliInit)
	{
		if (!this->InitClient(this->clsData->method, 0) &&
			!this->InitClient(Method::TLSV1_2, 0) &&
			!this->InitClient(Method::TLSV1_1, 0) &&
			!this->InitClient(Method::TLSV1, 0))
		{
			return 0;
		}
		this->clsData->cliInit = true;
	}

	Net::SocketUtil::AddressInfo addr;
	Socket *s;
	if (!this->sockf->DNSResolveIP(hostName, &addr))
	{
		if (err)
			*err = ErrorType::HostnameNotResolved;
		return 0;
	}
	if (addr.addrType == Net::AddrType::IPv4)
	{
		s = this->sockf->CreateTCPSocketv4();
	}
	else if (addr.addrType == Net::AddrType::IPv6)
	{
		s = this->sockf->CreateTCPSocketv6();
	}
	else
	{
		if (err)
			*err = ErrorType::HostnameNotResolved;
		return 0;
	}
	if (s == 0)
	{
		if (err)
			*err = ErrorType::OutOfMemory;
		return 0;
	}
	if (!this->sockf->Connect(s, &addr, port))
	{
		this->sockf->DestroySocket(s);
		if (err)
			*err = ErrorType::CannotConnect;
		return 0;
	}
	
	return CreateClientConn(0, s, hostName, err);
}

Net::SSLClient *Net::WinSSLEngine::ClientInit(Socket *s, const UTF8Char *hostName, ErrorType *err)
{
	if (!this->clsData->cliInit)
	{
		if (!this->InitClient(this->clsData->method, 0) &&
			!this->InitClient(Method::TLSV1_2, 0) &&
			!this->InitClient(Method::TLSV1_1, 0) &&
			!this->InitClient(Method::TLSV1, 0))
		{
			return 0;
		}
		this->clsData->cliInit = true;
	}
	return this->CreateClientConn(0, s, hostName, err);
}

Bool Net::WinSSLEngine::GenerateCert(const UTF8Char *country, const UTF8Char *company, const UTF8Char *commonName, Crypto::Cert::X509Cert **certASN1, Crypto::Cert::X509File **keyASN1)
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
	if (!CryptGenKey(hProv, AT_SIGNATURE, 0x08000000 | CRYPT_EXPORTABLE, &hKey))
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

	pCertContext = CertCreateSelfSignCertificate(0, &subjectIssuerBlob, 0, &keyProvInfo, &signatureAlgorithm, 0, &endTime, 0);
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
	NEW_CLASS(*certASN1, Crypto::Cert::X509Cert((const UTF8Char *)"selfsign.crt", pCertContext->pbCertEncoded, pCertContext->cbCertEncoded));
	*keyASN1 = Crypto::Cert::X509PrivKey::CreateFromKeyBuff(Crypto::Cert::X509File::KeyType::RSA, certBuff, certBuffSize, (const UTF8Char*)"RSAKey.key");
	CertFreeCertificateContext(pCertContext);
	//CryptReleaseContext(hCryptProvOrNCryptKey, 0);
	MemFree(pbEncoded);
	CryptDestroyKey(hKey);
	CryptReleaseContext(hProv, 0);
	return true;
}

Crypto::Cert::X509Key *Net::WinSSLEngine::GenerateRSAKey()
{
	HCRYPTKEY hKey;
	HCRYPTPROV hProv;
	UInt8 privKeyBuff[2048];
	DWORD privKeySize = 2048;
	UInt8 certBuff[4096];
	DWORD certBuffSize = 4096;
	if (!CryptAcquireContext(&hProv, L"SelfSign", NULL, PROV_RSA_FULL, CRYPT_MACHINE_KEYSET))
	{
		if (!CryptAcquireContext(&hProv, L"SelfSign", NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET | CRYPT_MACHINE_KEYSET))
		{
			return 0;
		}
	}
	if (!CryptGenKey(hProv, AT_SIGNATURE, 0x08000000 | CRYPT_EXPORTABLE, &hKey))
	{
		CryptReleaseContext(hProv, 0);
		return 0;
	}

	if (!CryptExportKey(hKey, 0, PRIVATEKEYBLOB, 0, privKeyBuff, &privKeySize))
	{
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return 0;
	}
	if (!CryptEncodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, PKCS_RSA_PRIVATE_KEY, privKeyBuff, 0, 0, certBuff, &certBuffSize))
	{
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return 0;
	}

	Crypto::Cert::X509Key *key;
	NEW_CLASS(key, Crypto::Cert::X509Key((const UTF8Char*)"RSAKey.key", certBuff, certBuffSize, Crypto::Cert::X509File::KeyType::RSA));
	CryptDestroyKey(hKey);
	CryptReleaseContext(hProv, 0);
	return key;
}

Bool Net::WinSSLEngine::Signature(Crypto::Cert::X509Key *key, Crypto::Hash::HashType hashType, const UInt8 *payload, UOSInt payloadLen, UInt8 *signData, UOSInt *signLen)
{
	if (key == 0)
	{
#if defined(VERBOSE)
		printf("SSL: key is null\r\n");
#endif
		return false;
	}
	ALG_ID alg;
	if (hashType == Crypto::Hash::HT_SHA256)
	{
		alg = CALG_SHA_256;
	}
	else if (hashType == Crypto::Hash::HT_SHA384)
	{
		alg = CALG_SHA_384;
	}
	else if (hashType == Crypto::Hash::HT_SHA512)
	{
		alg = CALG_SHA_512;
	}
	else
	{
#if defined(VERBOSE)
		printf("SSL: hashType not supported\r\n");
#endif
		return false;
	}
	HCRYPTKEY hKey;
	HCRYPTPROV hProv;
	if (!CryptAcquireContext(&hProv, 0, MS_ENH_RSA_AES_PROV, PROV_RSA_AES, 0))// CRYPT_VERIFYCONTEXT))
	{
		if (!CryptAcquireContext(&hProv, 0, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET | CRYPT_MACHINE_KEYSET))
		{
#if defined(VERBOSE)
			printf("SSL: CryptAcquireContext failed\r\n");
#endif
			return false;
		}
	}
	Crypto::Cert::X509PrivKey *privKey = Crypto::Cert::X509PrivKey::CreateFromKey(key);
	if (!WinSSLEngine_CryptImportPrivateKey(&hKey, hProv, privKey->GetASN1Buff(), (ULONG)privKey->GetASN1BuffSize(), true))
	{
#if defined(VERBOSE)
		printf("SSL: Import Key failed\r\n");
#endif
		DEL_CLASS(privKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	DEL_CLASS(privKey);
	HCRYPTHASH hHash;
	if (!CryptCreateHash(hProv, alg, 0, 0, &hHash))
	{
#if defined(VERBOSE)
		UInt32 errCode = GetLastError();
		printf("SSL: CryptCreateHash failed, errCode = 0x%x\r\n", errCode);
#endif
		CryptReleaseContext(hProv, 0);
		CryptDestroyKey(hKey);
		return false;
	}
	if (!CryptHashData(hHash, payload, (DWORD)payloadLen, 0))
	{
		CryptReleaseContext(hProv, 0);
		CryptDestroyHash(hHash);
		CryptDestroyKey(hKey);
#if defined(VERBOSE)
		printf("SSL: CryptHashData failed\r\n");
#endif
		return false;
	}
	DWORD len = 512;
	if (!CryptSignHash(hHash, AT_SIGNATURE, 0, 0, signData, &len))
	{
#if defined(VERBOSE)
		UInt32 errCode = GetLastError();
		printf("SSL: CryptSignHash failed, errCode = 0x%x\r\n", errCode);
#endif
		CryptReleaseContext(hProv, 0);
		CryptDestroyHash(hHash);
		CryptDestroyKey(hKey);
		return false;
	}
	*signLen = len;
	CryptReleaseContext(hProv, 0);
	CryptDestroyHash(hHash);
	CryptDestroyKey(hKey);
	UOSInt i = 0;
	UOSInt j = len - 1;
	UInt8 t;
	while (i < j)
	{
		t = signData[i];
		signData[i] = signData[j];
		signData[j] = t;
		i++;
		j--;
	}
#if defined(VERBOSE)
	printf("SSL: Signature success, len = %d\r\n", (UInt32)len);
#endif
	return true;
}
