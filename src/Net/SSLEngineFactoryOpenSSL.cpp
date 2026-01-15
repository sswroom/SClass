#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "Net/OpenSSLEngine.h"

Optional<Net::SSLEngine> Net::SSLEngineFactory::Create(NN<Net::TCPClientFactory> clif, Bool skipCertCheck)
{
	NN<Net::SSLEngine> ssl;
	NEW_CLASSNN(ssl, Net::OpenSSLEngine(clif, Net::SSLEngine::Method::Default));
	if (ssl->IsError())
	{
		ssl.Delete();
		return nullptr;
	}
	ssl->ClientSetSkipCertCheck(skipCertCheck);
	return ssl;
}
