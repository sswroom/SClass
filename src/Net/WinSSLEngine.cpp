#include "Stdafx.h"
#include "Data/DateTime.h"
#include "Net/WinSSLClient.h"
#include "Net/WinSSLEngine.h"
#include "Text/MyString.h"
#include <Windows.h>
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

UInt32 WinSSLEngine_GetProtocols(Net::SSLEngine::Method method, Bool server)
{
	if (server)
	{
		switch (method)
		{
		case Net::SSLEngine::M_SSLV3:
			return SP_PROT_SSL3;
		case Net::SSLEngine::M_SSLV23:
			return SP_PROT_SSL2 || SP_PROT_SSL3;
		case Net::SSLEngine::M_TLS:
			return SP_PROT_TLS1_0 || SP_PROT_TLS1_1 || SP_PROT_TLS1_2;
		case Net::SSLEngine::M_TLSV1:
			return SP_PROT_TLS1_0;
		case Net::SSLEngine::M_TLSV1_1:
			return SP_PROT_TLS1_1;
		case Net::SSLEngine::M_TLSV1_2:
			return SP_PROT_TLS1_2;
		case Net::SSLEngine::M_DTLS:
			return SP_PROT_DTLS1_0 || SP_PROT_DTLS1_2;
		case Net::SSLEngine::M_DTLSV1:
			return SP_PROT_DTLS1_0;
		case Net::SSLEngine::M_DTLSV1_2:
			return SP_PROT_DTLS1_2;
		default:
			return SP_PROT_TLS1_0 || SP_PROT_TLS1_1 || SP_PROT_TLS1_2 || SP_PROT_TLS1_3;
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

Bool Net::WinSSLEngine::SetServerCerts(const UTF8Char *certFile, const UTF8Char *keyFile)
{
	if (this->clsData->svrInit)
	{
		return false;
	}
	PCCERT_CONTEXT serverCert = 0;
//	CertOpenStore()
	SCHANNEL_CRED credData;
	MemClear(&credData, sizeof(credData));
	credData.dwVersion = SCHANNEL_CRED_VERSION;
	credData.grbitEnabledProtocols = WinSSLEngine_GetProtocols(this->clsData->method, true);


	////////////////////////
	return false;
}

UTF8Char *Net::WinSSLEngine::GetErrorDetail(UTF8Char *sbuff)
{
	*sbuff = 0;
	return sbuff;
}

Net::TCPClient *Net::WinSSLEngine::CreateServerConn(UInt32 *s)
{
	if (!this->clsData->svrInit)
	{
		return 0;
	}

	////////////////////////////////
	return 0;
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

	Net::TCPClient *cli;
	NEW_CLASS(cli, Net::WinSSLClient(sockf, s, &this->clsData->hCredCli, hostName, this->skipCertCheck));
	return cli;
}
