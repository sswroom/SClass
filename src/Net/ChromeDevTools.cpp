#include "Stdafx.h"
#include "Net/ChromeDevTools.h"

Net::ChromeDevTools::ChromeDevTools(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, UInt16 port)
{
	this->clif = clif;
	this->ssl = ssl;
	this->port = port;
}

Net::ChromeDevTools::~ChromeDevTools()
{
}

Optional<Net::ChromeDevTools::ChromeVersion> Net::ChromeDevTools::GetVersion() const
{
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("http://localhost:"));
	sb.AppendU16(this->port);
	sb.Append(CSTR("/json/version"));
	JSONREQ_RET(this->clif, this->ssl, sb.ToCString(), ChromeVersion);
}

Optional<Net::ChromeDevTools::ChromeTargets> Net::ChromeDevTools::GetTargets() const
{
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("http://localhost:"));
	sb.AppendU16(this->port);
	sb.Append(CSTR("/json/list"));
	JSONREQ_RET(this->clif, this->ssl, sb.ToCString(), ChromeTargets);
}
