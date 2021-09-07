#include "Stdafx.h"
#include "IO/MemoryStream.h"
#include "Net/Email/SMTPClient.h"

Net::Email::SMTPClient::SMTPClient(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const UTF8Char *host, UInt16 port, Net::Email::SMTPConn::ConnType connType, IO::Writer *logWriter)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->host = Text::StrCopyNew(host);
	this->port = port;
	this->connType = connType;
	this->logWriter = logWriter;
	this->authUser = 0;
	this->authPassword = 0;
}

Net::Email::SMTPClient::~SMTPClient()
{
	Text::StrDelNew(this->host);
	SDEL_TEXT(this->authUser);
	SDEL_TEXT(this->authPassword);
}

void Net::Email::SMTPClient::SetPlainAuth(const UTF8Char *userName, const UTF8Char *password)
{
	SDEL_TEXT(this->authUser);
	SDEL_TEXT(this->authPassword);
	this->authUser = SCOPY_TEXT(userName);
	this->authPassword = SCOPY_TEXT(password);
}

Bool Net::Email::SMTPClient::Send(Net::Email::EmailMessage *message)
{
	if (!message->CompletedMessage())
	{
		return false;
	}
	IO::MemoryStream mstm((const UTF8Char*)"Net.Email.SMTPClient.mstm");
	if (!message->WriteToStream(&mstm))
	{
		return false;
	}
	Net::Email::SMTPConn *conn;
	NEW_CLASS(conn, Net::Email::SMTPConn(this->sockf, this->ssl, this->host, this->port, this->connType, this->logWriter));
	if (conn->IsError())
	{
		DEL_CLASS(conn);
		return false;
	}
	if (!conn->SendEHlo((const UTF8Char*)"[127.0.0.1]"))
	{
		if (!conn->SendHelo((const UTF8Char*)"[127.0.0.1]"))
		{
			DEL_CLASS(conn);
			return false;
		}
	}
	if (this->authUser && this->authPassword)
	{
		if (!conn->SendAuth(this->authUser, this->authPassword))
		{
			DEL_CLASS(conn);
			return false;
		}
	}
	if (!conn->SendMailFrom(message->GetFromAddr()))
	{
		DEL_CLASS(conn);
		return false;
	}
	Data::ArrayList<const UTF8Char *> *recpList = message->GetRecpList();
	UOSInt i = 0;
	UOSInt j = recpList->GetCount();
	while (i < j)
	{
		if (!conn->SendRcptTo(recpList->GetItem(i)))
		{
			DEL_CLASS(conn);
			return false;
		}
		i++;
	}
	const UInt8 *content = mstm.GetBuff(&i);
	Bool succ = conn->SendData(content, i);
	conn->SendQuit();
	DEL_CLASS(conn);
	return succ;
}
