#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "Net/WinSSLEngine.h"

Optional<Net::SSLEngine> Net::SSLEngineFactory::Create(NN<Net::TCPClientFactory> clif, Bool skipCertCheck)
{
	NN<Net::SSLEngine> ssl;
	NEW_CLASSNN(ssl, Net::WinSSLEngine(clif, Net::SSLEngine::Method::Default));
	if (ssl->IsError())
	{
		ssl.Delete();
		return nullptr;
	}
	ssl->ClientSetSkipCertCheck(skipCertCheck);
	return ssl;
}