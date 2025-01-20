#include "Stdafx.h"
#include "MyMemory.h"
#include "SSWR/OrganWeb/OrganWebSession.h"

SSWR::OrganWeb::OrganWebSession::OrganWebSession()
{
	this->sess = 0;
}

SSWR::OrganWeb::OrganWebSession::OrganWebSession(Optional<Net::WebServer::WebSession> sess)
{
	this->sess = 0;
	this->Use(sess);
}

SSWR::OrganWeb::OrganWebSession::~OrganWebSession()
{
	this->EndUse();
}

void SSWR::OrganWeb::OrganWebSession::Use(Optional<Net::WebServer::WebSession> sess)
{
	this->EndUse();
	this->sess = sess;
};

void SSWR::OrganWeb::OrganWebSession::EndUse()
{
	NN<Net::WebServer::WebSession> sess;
	if (this->sess.SetTo(sess))
	{
		sess->EndUse();
		this->sess = 0;
	}
}

Optional<Net::WebServer::WebSession> SSWR::OrganWeb::OrganWebSession::GetSess()
{
	return this->sess;
}

void SSWR::OrganWeb::OrganWebSession::SetPickObjType(Int32 pickObjType)
{
	NN<Net::WebServer::WebSession> sess;
	if (this->sess.SetTo(sess))
	{
		sess->SetValueInt32(CSTR("PickObjType"), pickObjType);
	}
}
