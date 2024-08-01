#include "Stdafx.h"
#include "Net/Email/AWSEmailClient.h"
Int32 Net::Email::AWSEmailClient::initCnt = 0;

class Net::Email::AWSEmailClient::ClassData
{
};

Net::Email::AWSEmailClient::AWSEmailClient(Text::CStringNN region)
{
	NEW_CLASSNN(this->clsData, ClassData());
}

Net::Email::AWSEmailClient::~AWSEmailClient()
{
	this->clsData.Delete();
}

void Net::Email::AWSEmailClient::SetProxy(Text::CStringNN proxyHost, UInt16 port, Text::CString username, Text::CString password)
{
}

Bool Net::Email::AWSEmailClient::Send(NN<Net::Email::EmailMessage> message)
{
	return false;
}
