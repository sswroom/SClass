#include "Stdafx.h"
#include "Net/Email/POP3Client.h"

Net::Email::POP3Client::POP3Client(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString host, UInt16 port, Net::Email::POP3Conn::ConnType connType, IO::Writer *logWriter, Text::CString username, Text::CString password) : conn(sockf, ssl, host, port, connType, logWriter)
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

Bool Net::Email::POP3Client::ReadMessageAsString(UOSInt index, Text::StringBuilderUTF8 *sb)
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
