#include "Stdafx.h"
#include "MyMemory.h"
#include "SSWR/OrganWeb/OrganWebSession.h"

SSWR::OrganWeb::OrganWebSession::OrganWebSession()
{
	this->sess = 0;
}

SSWR::OrganWeb::OrganWebSession::OrganWebSession(Optional<Net::WebServer::IWebSession> sess)
{
	this->sess = 0;
	this->Use(sess);
}

SSWR::OrganWeb::OrganWebSession::~OrganWebSession()
{
	this->EndUse();
}

void SSWR::OrganWeb::OrganWebSession::Use(Optional<Net::WebServer::IWebSession> sess)
{
	this->EndUse();
	this->sess = sess;
};

void SSWR::OrganWeb::OrganWebSession::EndUse()
{
	NN<Net::WebServer::IWebSession> sess;
	if (this->sess.SetTo(sess))
	{
		sess->EndUse();
		this->sess = 0;
	}
}

Optional<Net::WebServer::IWebSession> SSWR::OrganWeb::OrganWebSession::GetSess()
{
	return this->sess;
}

void SSWR::OrganWeb::OrganWebSession::SetPickObjType(Int32 pickObjType)
{
	NN<Net::WebServer::IWebSession> sess;
	if (this->sess.SetTo(sess))
	{
		sess->SetValueInt32(CSTR("PickObjType"), pickObjType);
	}
}
