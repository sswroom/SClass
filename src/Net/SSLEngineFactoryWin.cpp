#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "Net/WinSSLEngine.h"

Optional<Net::SSLEngine> Net::SSLEngineFactory::Create(NotNullPtr<Net::SocketFactory> sockf, Bool skipCertCheck)
{
	Optional<Net::SSLEngine> ssl;
	NEW_CLASS(ssl, Net::WinSSLEngine(sockf, Net::SSLEngine::Method::Default));
	if (ssl->IsError())
	{
		DEL_CLASS(ssl);
		return 0;
	}
	ssl->ClientSetSkipCertCheck(skipCertCheck);
	return ssl;
}