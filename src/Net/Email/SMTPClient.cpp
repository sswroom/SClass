#include "Stdafx.h"
#include "IO/MemoryStream.h"
#include "Net/Email/SMTPClient.h"
#include "Sync/Thread.h"

Net::Email::SMTPClient::SMTPClient(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString host, UInt16 port, Net::Email::SMTPConn::ConnType connType, IO::Writer *logWriter)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->host = Text::String::New(host);
	this->port = port;
	this->connType = connType;
	this->logWriter = logWriter;
	this->authUser = 0;
	this->authPassword = 0;
}

Net::Email::SMTPClient::~SMTPClient()
{
	this->host->Release();
	SDEL_STRING(this->authUser);
	SDEL_STRING(this->authPassword);
}

void Net::Email::SMTPClient::SetPlainAuth(Text::CString userName, Text::CString password)
{
	SDEL_STRING(this->authUser);
	SDEL_STRING(this->authPassword);
	this->authUser = Text::String::NewOrNull(userName);
	this->authPassword = Text::String::NewOrNull(password);
}

Bool Net::Email::SMTPClient::Send(Net::Email::EmailMessage *message)
{
	if (!message->CompletedMessage())
	{
		return false;
	}
	IO::MemoryStream mstm(UTF8STRC("Net.Email.SMTPClient.mstm"));
	if (!message->WriteToStream(&mstm))
	{
		return false;
	}
	Net::Email::SMTPConn *conn;
	NEW_CLASS(conn, Net::Email::SMTPConn(this->sockf, this->ssl, this->host->ToCString(), this->port, this->connType, this->logWriter));
	if (conn->IsError())
	{
		DEL_CLASS(conn);
		return false;
	}
	if (!conn->SendEHlo(CSTR("[127.0.0.1]")))
	{
		if (!conn->SendHelo(CSTR("[127.0.0.1]")))
		{
			DEL_CLASS(conn);
			return false;
		}
	}
	Sync::Thread::Sleep(10);
	if (this->authUser && this->authPassword)
	{
		if (!conn->SendAuth(this->authUser->ToCString(), this->authPassword->ToCString()))
		{
			DEL_CLASS(conn);
			return false;
		}
	}
	if (!conn->SendMailFrom(message->GetFromAddr()->ToCString()))
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
