#include "Stdafx.h"
#include "Net/Email/SMTPClient.h"

Net::Email::SMTPClient::SMTPClient(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const UTF8Char *host, UInt16 port, Net::Email::SMTPConn::ConnType connType, IO::Writer *logWriter)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->host = Text::StrCopyNew(host);
	this->port = port;
	this->connType = connType;
	this->logWriter = logWriter;
}

Net::Email::SMTPClient::~SMTPClient()
{
	Text::StrDelNew(this->host);
}

Bool Net::Email::SMTPClient::Send();
