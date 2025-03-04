#include "Stdafx.h"
#include "Net/Email/EmailValidator.h"
#include "Net/Email/SMTPConn.h"

Net::Email::EmailValidator::EmailValidator(NN<Net::TCPClientFactory> clif, NN<IO::LogTool> log)
{
	Net::SocketUtil::AddressInfo dnsAddr;
	this->clif = clif;
	this->clif->GetSocketFactory()->GetDefDNS(dnsAddr);
	NEW_CLASSNN(this->dnsClient, Net::DNSClient(this->clif->GetSocketFactory(), dnsAddr, log));
}

Net::Email::EmailValidator::~EmailValidator()
{
	this->dnsClient.Delete();
}

Net::Email::EmailValidator::Status Net::Email::EmailValidator::Validate(Text::CStringNN emailAddr)
{
	Net::SocketUtil::AddressInfo addr;
	Net::Email::SMTPConn *conn;
	Text::CStringNN emailDomain;
	UOSInt i = emailAddr.IndexOf('@');
	UOSInt j;
	if (i == INVALID_INDEX || i == 0)
	{
		return S_INVALID_FORMAT;
	}
	emailDomain = emailAddr.Substring(i + 1);
	i = emailDomain.IndexOf('@');
	if (i != INVALID_INDEX)
	{
		return S_INVALID_FORMAT;
	}

	Text::String *emailSvr = 0;
	Data::ArrayListNN<Net::DNSClient::RequestAnswer> ansList;
	NN<Net::DNSClient::RequestAnswer> ans;
	this->dnsClient->GetByEmailDomainName(ansList, emailDomain);
	i = 0;
	j = ansList.GetCount();
	while (i < j)
	{
		ans = ansList.GetItemNoCheck(i);
		if (ans->recType == 15)
		{
			emailSvr = ans->rd->Clone().Ptr();
			break;
		}
		j++;
	}
	Net::DNSClient::FreeAnswers(ansList);
	if (emailSvr == 0)
	{
		return S_DOMAIN_NOT_RESOLVED;
	}

	if (!this->clif->GetSocketFactory()->DNSResolveIP(emailSvr->ToCString(), addr))
	{
		emailSvr->Release();
		return S_DOMAIN_NOT_RESOLVED;
	}
	NEW_CLASS(conn, Net::Email::SMTPConn(this->clif, 0, emailSvr->ToCString(), 25, Net::Email::SMTPConn::ConnType::Plain, 0, 60000));
	emailSvr->Release();
	if (conn->IsError())
	{
		DEL_CLASS(conn);
		return S_CONN_ERROR;
	}
	if (!conn->SendHelo(CSTR("[127.0.0.1]")))
	{
		conn->SendQuit();
		DEL_CLASS(conn);
		return S_COMM_ERROR;
	}
	if (!conn->SendMailFrom(CSTR("sswroom@yahoo.com")))
	{
		conn->SendQuit();
		DEL_CLASS(conn);
		return S_FROM_NOT_ACCEPT;
	}
	if (!conn->SendRcptTo(emailAddr))
	{
		conn->SendQuit();
		DEL_CLASS(conn);
		return S_NO_SUCH_ADDR;
	}
	conn->SendQuit();
	DEL_CLASS(conn);
	return S_VALID;
}

Text::CStringNN Net::Email::EmailValidator::StatusGetName(Status status)
{
	switch (status)
	{
	case S_VALID:
		return CSTR("Email Address Valid");
	case S_INVALID_FORMAT:
		return CSTR("Invalid Format");
	case S_NO_SUCH_ADDR:
		return CSTR("No Such Address");
	case S_FROM_NOT_ACCEPT:
		return CSTR("From Address Not Accepted");
	case S_CONN_ERROR:
		return CSTR("Connect Error");
	case S_COMM_ERROR:
		return CSTR("Communication Error");
	case S_DOMAIN_NOT_RESOLVED:
		return CSTR("Domain not resolved");
	default:
		return CSTR("Unknown");
	}
}
