#include "Stdafx.h"
#include "Net/DefaultSSLEngine.h"
#include "Net/WinSSLEngine.h"

Net::SSLEngine *Net::DefaultSSLEngine::Create(Net::SocketFactory *sockf)
{
	Net::SSLEngine *ssl;
	NEW_CLASS(ssl, Net::WinSSLEngine(sockf, Net::SSLEngine::M_TLS));
	if (ssl->IsError())
	{
		DEL_CLASS(ssl);
		return 0;
	}
	//ssl->SetServerCerts((const UTF8Char*)"C:\\Progs\\SSWR\\AVIRead2017\\test.crt", (const UTF8Char*)"C:\\Progs\\SSWR\\AVIRead2017\\test.key");
	return ssl;
}