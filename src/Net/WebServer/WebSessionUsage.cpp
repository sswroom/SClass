#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/WebServer/WebSessionUsage.h"

Net::WebServer::WebSessionUsage::WebSessionUsage(Net::WebServer::IWebSession *sess)
{
	this->sess = sess;
}

Net::WebServer::WebSessionUsage::~WebSessionUsage()
{
	this->EndUse();
}

void Net::WebServer::WebSessionUsage::EndUse()
{
	if (this->sess)
	{
		this->sess->EndUse();
		this->sess = 0;
	}
}

Net::WebServer::IWebSession *Net::WebServer::WebSessionUsage::GetSess()
{
	return this->sess;
}
