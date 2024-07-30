#include "Stdafx.h"
#include "Net/Email/POP3Client.h"

Net::Email::POP3Client::POP3Client(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN host, UInt16 port, Net::Email::POP3Conn::ConnType connType, IO::Writer *logWriter, Text::CStringNN username, Text::CStringNN password, Data::Duration timeout) : conn(clif, ssl, host, port, connType, logWriter, timeout)
{
	this->loggedIn = false;
	this->msgCnt = 0;
	this->msgSize = 0;	

	if (!this->conn.IsError())
	{
		if (this->conn.SendUser(username) == Net::Email::POP3Conn::ResultStatus::Success && this->conn.SendPass(password) == Net::Email::POP3Conn::ResultStatus::Success)
		{
			this->loggedIn = true;
			this->conn.SendStat(&this->msgCnt, &this->msgSize);
		}
	}
}

Net::Email::POP3Client::~POP3Client()
{

}

Bool Net::Email::POP3Client::IsError()
{
	return this->conn.IsError() || !this->loggedIn;
}

UOSInt Net::Email::POP3Client::GetMessageCount()
{
	return this->msgCnt;
}

UOSInt Net::Email::POP3Client::GetMessageSize()
{
	return this->msgSize;
}

Bool Net::Email::POP3Client::ReadMessageAsString(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	if (!this->loggedIn)
		return false;
	return this->conn.SendRetr(index, sb) == Net::Email::POP3Conn::ResultStatus::Success;
}

Bool Net::Email::POP3Client::DeleteMessage(UOSInt index)
{
	if (!this->loggedIn)
		return false;
	return this->conn.SendDele(index) == Net::Email::POP3Conn::ResultStatus::Success;
}
