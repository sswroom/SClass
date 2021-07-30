#include "Stdafx.h"
#include "Net/DefaultSSLEngine.h"
#include "Net/SSLEngine.h"
#include "Text/MyString.h"

Net::SSLEngine::SSLEngine(Net::SocketFactory *sockf)
{
	this->sockf = sockf;
}

Net::SSLEngine::~SSLEngine()
{
}

const UTF8Char *Net::SSLEngine::ErrorTypeGetName(ErrorType err)
{
	switch (err)
	{
	case ET_NONE:
		return (const UTF8Char*)"No Error";
	case ET_HOSTNAME_NOT_RESOLVED:
		return (const UTF8Char*)"Hostname cannot not resolve";
	case ET_OUT_OF_MEMORY:
		return (const UTF8Char*)"Out of memory";
	case ET_CANNOT_CONNECT:
		return (const UTF8Char*)"Cannot connect to destination";
	case ET_INIT_SESSION:
		return (const UTF8Char*)"Failed in initializing session";
	case ET_CERT_NOT_FOUND:
		return (const UTF8Char*)"Server Certification not found";
	case ET_INVALID_NAME:
		return (const UTF8Char*)"Invalid cert name";
	case ET_SELF_SIGN:
		return (const UTF8Char*)"Self Signed Certification";
	case ET_INVALID_PERIOD:
		return (const UTF8Char*)"Valid period out of range";
	default:
		return (const UTF8Char*)"Unknown";
	}
}
