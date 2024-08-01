#include "Stdafx.h"
#include "Net/Email/AWSEmailClient.h"
#include "Sync/Interlocked.h"
#include <aws/core/Aws.h>
#include <aws/email/SESClient.h>
#include <aws/email/model/SendEmailRequest.h>
#include <aws/email/model/Destination.h>
#include <aws/email/model/Body.h>

#define VERBOSE

Int32 Net::Email::AWSEmailClient::initCnt = 0;

class Net::Email::AWSEmailClient::ClassData
{
public:
	Aws::SDKOptions options;
	Aws::Client::ClientConfiguration clientConfiguration;
};

Net::Email::AWSEmailClient::AWSEmailClient(Text::CStringNN region)
{
	NEW_CLASSNN(this->clsData, ClassData());
	if (Sync::Interlocked::IncrementI32(initCnt) == 1)
	{
		Aws::InitAPI(this->clsData->options);
	}
	this->clsData->clientConfiguration.region = (const Char*)region.v.Ptr();
}

Net::Email::AWSEmailClient::~AWSEmailClient()
{
	if (Sync::Interlocked::DecrementI32(initCnt) == 0)
	{
		Aws::ShutdownAPI(this->clsData->options);
	}
	this->clsData.Delete();
}

void Net::Email::AWSEmailClient::SetProxy(Text::CStringNN proxyHost, UInt16 port, Text::CString username, Text::CString password)
{
	this->clsData->clientConfiguration.proxyHost = (const Char*)proxyHost.v.Ptr();
	this->clsData->clientConfiguration.proxyPort = port;
	Text::CStringNN proxyUser;
	Text::CStringNN proxyPwd;
	if (username.SetTo(proxyUser) && password.SetTo(proxyPwd))
	{
		this->clsData->clientConfiguration.proxyUserName = (const Char*)proxyUser.v.Ptr();
		this->clsData->clientConfiguration.proxyPassword = (const Char*)proxyPwd.v.Ptr();
	}
}

Bool Net::Email::AWSEmailClient::Send(NN<Net::Email::EmailMessage> message)
{
	NN<Net::Email::EmailMessage::EmailAddress> fromAddr;
	Text::CStringNN subject;
	if (!message->GetFrom().SetTo(fromAddr) || !message->GetSubject().SetTo(subject))
	{
		return false;
	}
	Aws::SES::SESClient sesClient(this->clsData->clientConfiguration);
	Aws::SES::Model::Destination destination;
	NN<const Data::ArrayListNN<Net::Email::EmailMessage::EmailAddress>> recpList = message->GetRecpList();
	NN<Net::Email::EmailMessage::EmailAddress> addr;
	UOSInt i = 0;
	UOSInt j = recpList->GetCount();
	while (i < j)
	{
		addr = recpList->GetItemNoCheck(i);
		switch (addr->type)
		{
		case Net::Email::EmailMessage::RecipientType::To:
			destination.AddToAddresses((const Char*)addr->addr->v.Ptr());
			break;
		case Net::Email::EmailMessage::RecipientType::Cc:
			destination.AddCcAddresses((const Char*)addr->addr->v.Ptr());
			break;
		case Net::Email::EmailMessage::RecipientType::Bcc:
			destination.AddBccAddresses((const Char*)addr->addr->v.Ptr());
			break;
		case Net::Email::EmailMessage::RecipientType::From:
			break;
		}
		i++;
	}

	Aws::SES::Model::Body messageBody;
	NN<Text::String> contentType;
	UnsafeArray<UInt8> content;
	UOSInt contentLen;
	if (!message->GetContentType().SetTo(contentType) || !message->GetContent(contentLen).SetTo(content))
	{
		return false;
	}
	if (contentType->Equals(CSTR("text/html")))
	{
		messageBody.SetHtml(Aws::SES::Model::Content().WithCharset("UTF-8").WithData((const Char*)content.Ptr()));
	}
	else
	{
		messageBody.SetText(Aws::SES::Model::Content().WithCharset("UTF-8").WithData((const Char*)content.Ptr()));
	}
	Aws::SES::Model::Message msg;
    msg.SetBody(messageBody);
    msg.SetSubject(Aws::SES::Model::Content().WithCharset("UTF-8").WithData((const Char*)subject.v.Ptr()));

    Aws::SES::Model::SendEmailRequest sendEmailRequest;
    sendEmailRequest.SetDestination(destination);
    sendEmailRequest.SetMessage(msg);
	sendEmailRequest.SetSource((const Char*)fromAddr->addr->v.Ptr());
    Aws::SES::Model::SendEmailOutcome outcome = sesClient.SendEmail(sendEmailRequest);
	if (outcome.IsSuccess())
	{
		return true;
	}
	else
	{
#if defined(VERBOSE)
		printf("AWS error: %s\r\n", outcome.GetError().GetMessage().c_str());
#endif
		return false;
	}
}
