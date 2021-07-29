#include "Stdafx.h"
#include "Net/DefaultSSLEngine.h"
#include "Net/OpenSSLEngine.h"

Net::SSLEngine *Net::DefaultSSLEngine::Create(Net::SocketFactory *sockf)
{
	Net::SSLEngine *ssl;
	NEW_CLASS(ssl, Net::OpenSSLEngine(sockf, Net::SSLEngine::M_TLS));
	if (ssl->IsError())
	{
		DEL_CLASS(ssl);
		return 0;
	}
	return ssl;
}