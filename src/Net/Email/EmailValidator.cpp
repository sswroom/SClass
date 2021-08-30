#include "Stdafx.h"
#include "Net/Email/EmailValidator.h"
#include "Net/Email/SMTPConn.h"

Net::Email::EmailValidator::EmailValidator(Net::SocketFactory *sockf)
{
	Net::SocketUtil::AddressInfo dnsAddr;
	this->sockf = sockf;
	this->sockf->GetDefDNS(&dnsAddr);
	NEW_CLASS(this->dnsClient, Net::DNSClient(this->sockf, &dnsAddr));
}

Net::Email::EmailValidator::~EmailValidator()
{
	DEL_CLASS(this->dnsClient);
}

Net::Email::EmailValidator::Status Net::Email::EmailValidator::Validate(const UTF8Char *emailAddr)
{
	Net::SocketUtil::AddressInfo addr;
	Net::Email::SMTPConn *conn;
	const UTF8Char *emailDomain;
	UOSInt i = Text::StrIndexOf(emailAddr, '@');
	UOSInt j;
	if (i == INVALID_INDEX || i == 0)
	{
		return S_INVALID_FORMAT;
	}
	emailDomain = &emailAddr[i + 1];
	i = Text::StrIndexOf(emailDomain, '@');
	if (i != INVALID_INDEX)
	{
		return S_INVALID_FORMAT;
	}

	const UTF8Char *emailSvr = 0;
	Data::ArrayList<Net::DNSClient::RequestAnswer*> ansList;
	Net::DNSClient::RequestAnswer *ans;
	this->dnsClient->GetByEmailDomainName(&ansList, emailDomain);
	i = 0;
	j = ansList.GetCount();
	while (i < j)
	{
		ans = ansList.GetItem(i);
		if (ans->recType == 15)
		{
			emailSvr = Text::StrCopyNew(ans->rd);
			break;
		}
		j++;
	}
	Net::DNSClient::FreeAnswers(&ansList);
	if (emailSvr == 0)
	{
		return S_DOMAIN_NOT_RESOLVED;
	}

	if (!this->sockf->DNSResolveIP(emailSvr, &addr))
	{
		Text::StrDelNew(emailSvr);
		return S_DOMAIN_NOT_RESOLVED;
	}
	NEW_CLASS(conn, Net::Email::SMTPConn(this->sockf, 0, emailSvr, 25, 0));
	Text::StrDelNew(emailSvr);
	if (conn->IsError())
	{
		DEL_CLASS(conn);
		return S_CONN_ERROR;
	}
	if (!conn->SendHelo((const UTF8Char*)"[127.0.0.1]"))
	{
		conn->SendQuit();
		DEL_CLASS(conn);
		return S_COMM_ERROR;
	}
	if (!conn->SendMailFrom((const UTF8Char*)"sswroom@yahoo.com"))
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

const UTF8Char *Net::Email::EmailValidator::StatusGetName(Status status)
{
	switch (status)
	{
	case S_VALID:
		return (const UTF8Char*)"Email Address Valid";
	case S_INVALID_FORMAT:
		return (const UTF8Char*)"Invalid Format";
	case S_NO_SUCH_ADDR:
		return (const UTF8Char*)"No Such Address";
	case S_FROM_NOT_ACCEPT:
		return (const UTF8Char*)"From Address Not Accepted";
	case S_CONN_ERROR:
		return (const UTF8Char*)"Connect Error";
	case S_COMM_ERROR:
		return (const UTF8Char*)"Communication Error";
	case S_DOMAIN_NOT_RESOLVED:
		return (const UTF8Char*)"Domain not resolved";
	default:
		return (const UTF8Char*)"Unknown";
	}
}
