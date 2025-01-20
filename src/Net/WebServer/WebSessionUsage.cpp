#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/WebServer/WebSessionUsage.h"

Net::WebServer::WebSessionUsage::WebSessionUsage()
{
	this->sess = 0;
}

Net::WebServer::WebSessionUsage::WebSessionUsage(Optional<Net::WebServer::WebSession> sess)
{
	this->sess = 0;
	this->Use(sess);
}

Net::WebServer::WebSessionUsage::~WebSessionUsage()
{
	this->EndUse();
}

void Net::WebServer::WebSessionUsage::Use(Optional<Net::WebServer::WebSession> sess)
{
	this->EndUse();
	this->sess = sess;
};

void Net::WebServer::WebSessionUsage::EndUse()
{
	NN<Net::WebServer::WebSession> sess;
	if (this->sess.SetTo(sess))
	{
		sess->EndUse();
		this->sess = 0;
	}
}

Optional<Net::WebServer::WebSession> Net::WebServer::WebSessionUsage::GetSess()
{
	return this->sess;
}
