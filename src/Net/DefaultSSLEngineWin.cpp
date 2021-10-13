#include "Stdafx.h"
#include "Net/DefaultSSLEngine.h"
#include "Net/WinSSLEngine.h"

Net::SSLEngine *Net::DefaultSSLEngine::Create(Net::SocketFactory *sockf, Bool skipCertCheck)
{
	Net::SSLEngine *ssl;
	NEW_CLASS(ssl, Net::WinSSLEngine(sockf, Net::SSLEngine::Method::Default));
	if (ssl->IsError())
	{
		DEL_CLASS(ssl);
		return 0;
	}
	ssl->SetSkipCertCheck(skipCertCheck);
	return ssl;
}