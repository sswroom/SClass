#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "Net/WinSSLEngine.h"

Optional<Net::SSLEngine> Net::SSLEngineFactory::Create(NN<Net::SocketFactory> sockf, Bool skipCertCheck)
{
	NN<Net::SSLEngine> ssl;
	NEW_CLASSNN(ssl, Net::WinSSLEngine(sockf, Net::SSLEngine::Method::Default));
	if (ssl->IsError())
	{
		ssl.Delete();
		return 0;
	}
	ssl->ClientSetSkipCertCheck(skipCertCheck);
	return ssl;
}