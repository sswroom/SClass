#include "Stdafx.h"
#include "IO/MemoryStream.h"
#include "Net/Email/SMTPClient.h"
#include "Sync/SimpleThread.h"

Net::Email::SMTPClient::SMTPClient(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CStringNN host, UInt16 port, Net::Email::SMTPConn::ConnType connType, IO::Writer *logWriter, Data::Duration timeout)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->host = Text::String::New(host);
	this->port = port;
	this->connType = connType;
	this->logWriter = logWriter;
	this->authUser = 0;
	this->authPassword = 0;
	this->timeout = timeout;
}

Net::Email::SMTPClient::~SMTPClient()
{
	this->host->Release();
	OPTSTR_DEL(this->authUser);
	OPTSTR_DEL(this->authPassword);
}

void Net::Email::SMTPClient::SetPlainAuth(Text::CString userName, Text::CString password)
{
	OPTSTR_DEL(this->authUser);
	OPTSTR_DEL(this->authPassword);
	this->authUser = Text::String::NewOrNull(userName);
	this->authPassword = Text::String::NewOrNull(password);
}

Bool Net::Email::SMTPClient::Send(NotNullPtr<Net::Email::EmailMessage> message)
{
	if (!message->CompletedMessage())
	{
		return false;
	}
	IO::MemoryStream mstm;
	if (!message->WriteToStream(mstm))
	{
		return false;
	}
	Net::Email::SMTPConn conn(this->sockf, this->ssl, this->host->ToCString(), this->port, this->connType, this->logWriter, this->timeout);
	if (conn.IsError())
	{
		return false;
	}
	if (!conn.SendEHlo(CSTR("[127.0.0.1]")))
	{
		if (!conn.SendHelo(CSTR("[127.0.0.1]")))
		{
			return false;
		}
	}
	Sync::SimpleThread::Sleep(10);
	NotNullPtr<Text::String> user;
	NotNullPtr<Text::String> password;
	if (this->authUser.SetTo(user) && this->authPassword.SetTo(password))
	{
		if (!conn.SendAuth(user->ToCString(), password->ToCString()))
		{
			return false;
		}
	}
	NotNullPtr<Net::Email::EmailMessage::EmailAddress> addr;
	if (!message->GetFrom().SetTo(addr) || !conn.SendMailFrom(addr->addr->ToCString()))
	{
		return false;
	}
	NotNullPtr<const Data::ArrayListNN<Net::Email::EmailMessage::EmailAddress>> recpList = message->GetRecpList();
	Data::ArrayIterator<NotNullPtr<Net::Email::EmailMessage::EmailAddress>> it = recpList->Iterator();
	while (it.HasNext())
	{
		if (!conn.SendRcptTo(it.Next()->addr->ToCString()))
		{
			return false;
		}
	}
	const UInt8 *content = mstm.GetBuff();
	Bool succ = conn.SendData(content, (UOSInt)mstm.GetLength());
	conn.SendQuit();
	return succ;
}
