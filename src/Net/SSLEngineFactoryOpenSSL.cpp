#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "Net/OpenSSLEngine.h"

Optional<Net::SSLEngine> Net::SSLEngineFactory::Create(NotNullPtr<Net::SocketFactory> sockf, Bool skipCertCheck)
{
	NotNullPtr<Net::SSLEngine> ssl;
	NEW_CLASSNN(ssl, Net::OpenSSLEngine(sockf, Net::SSLEngine::Method::Default));
	if (ssl->IsError())
	{
		ssl.Delete();
		return 0;
	}
	ssl->ClientSetSkipCertCheck(skipCertCheck);
	return ssl;
}
