#include "Stdafx.h"
#include "SSWR/OrganWeb/OrganWebBookController.h"
#include "SSWR/OrganWeb/OrganWebHandler.h"
#include "SSWR/OrganWeb/OrganWebMainController.h"
#include "SSWR/OrganWeb/OrganWebPhotoController.h"
#include "SSWR/OrganWeb/OrganWebPOIController.h"

Bool __stdcall SSWR::OrganWeb::OrganWebHandler::OnSessionDel(NN<Net::WebServer::IWebSession> sess, AnyType userObj)
{
	Data::DateTime *t;
	Data::ArrayListInt32 *pickObjs;
	t = (Data::DateTime *)sess->GetValuePtr(CSTR("LastUseTime"));
	pickObjs = (Data::ArrayListInt32*)sess->GetValuePtr(CSTR("PickObjs"));
	DEL_CLASS(t);
	DEL_CLASS(pickObjs);
	return false;
}

Bool __stdcall SSWR::OrganWeb::OrganWebHandler::OnSessionCheck(NN<Net::WebServer::IWebSession> sess, AnyType userObj)
{
	NN<Data::DateTime> t;
	Data::DateTime currTime;
	if (t.Set((Data::DateTime*)sess->GetValuePtr(CSTR("LastUseTime"))))
	{
		currTime.SetCurrTimeUTC();
		if (currTime.DiffMS(t) >= 1800000)
			return true;
	}
	return false;
}

#define AddCtrl(class) NEW_CLASSNN(ctrl, class); this->AddController(ctrl);
SSWR::OrganWeb::OrganWebHandler::OrganWebHandler(NN<OrganWebEnv> env, UInt32 scnSize, Text::CStringNN rootDir) : WebControllerHandler(rootDir)
{
	this->env = env;
	this->scnSize = scnSize;
	NEW_CLASSNN(this->sessMgr, Net::WebServer::MemoryWebSessionManager(CSTR("/"), OnSessionDel, this, 30000, OnSessionCheck, this, CSTR("OrganSessId")));

	NN<Net::WebServer::WebController> ctrl;
	AddCtrl(OrganWebMainController(this->sessMgr, env, scnSize));
	AddCtrl(OrganWebPhotoController(this->sessMgr, env, scnSize));
	AddCtrl(OrganWebPOIController(this->sessMgr, env, scnSize));
	AddCtrl(OrganWebBookController(this->sessMgr, env, scnSize));
}

SSWR::OrganWeb::OrganWebHandler::~OrganWebHandler()
{
	this->sessMgr.Delete();
}
