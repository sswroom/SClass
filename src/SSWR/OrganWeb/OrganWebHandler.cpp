#include "Stdafx.h"
#include "SSWR/OrganWeb/OrganWebBookController.h"
#include "SSWR/OrganWeb/OrganWebHandler.h"
#include "SSWR/OrganWeb/OrganWebMainController.h"
#include "SSWR/OrganWeb/OrganWebPhotoController.h"
#include "SSWR/OrganWeb/OrganWebPOIController.h"

Bool __stdcall SSWR::OrganWeb::OrganWebHandler::OnSessionDel(NotNullPtr<Net::WebServer::IWebSession> sess, AnyType userObj)
{
	Data::DateTime *t;
	Data::ArrayListInt32 *pickObjs;
	t = (Data::DateTime *)sess->GetValuePtr(CSTR("LastUseTime"));
	pickObjs = (Data::ArrayListInt32*)sess->GetValuePtr(CSTR("PickObjs"));
	DEL_CLASS(t);
	DEL_CLASS(pickObjs);
	return false;
}

Bool __stdcall SSWR::OrganWeb::OrganWebHandler::OnSessionCheck(NotNullPtr<Net::WebServer::IWebSession> sess, AnyType userObj)
{
	NotNullPtr<Data::DateTime> t;
	Data::DateTime currTime;
	if (t.Set((Data::DateTime*)sess->GetValuePtr(CSTR("LastUseTime"))))
	{
		currTime.SetCurrTimeUTC();
		if (currTime.DiffMS(t) >= 1800000)
			return true;
	}
	return false;
}

SSWR::OrganWeb::OrganWebHandler::OrganWebHandler(OrganWebEnv *env, UInt32 scnSize, Text::CStringNN rootDir) : WebControllerHandler(rootDir)
{
	this->env = env;
	this->scnSize = scnSize;
	NEW_CLASS(this->sessMgr, Net::WebServer::MemoryWebSessionManager(CSTR("/"), OnSessionDel, this, 30000, OnSessionCheck, this, CSTR("OrganSessId")));

	this->AddController(NEW_CLASS_D(OrganWebMainController(this->sessMgr, env, scnSize)));
	this->AddController(NEW_CLASS_D(OrganWebPhotoController(this->sessMgr, env, scnSize)));
	this->AddController(NEW_CLASS_D(OrganWebPOIController(this->sessMgr, env, scnSize)));
	this->AddController(NEW_CLASS_D(OrganWebBookController(this->sessMgr, env, scnSize)));
}

SSWR::OrganWeb::OrganWebHandler::~OrganWebHandler()
{
	SDEL_CLASS(this->sessMgr);
}
