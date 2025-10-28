#include "Stdafx.h"
#include "SSWR/OrganWeb/OrganWebBookController.h"
#include "SSWR/OrganWeb/OrganWebHandler.h"
#include "SSWR/OrganWeb/OrganWebMainController.h"
#include "SSWR/OrganWeb/OrganWebPhotoController.h"
#include "SSWR/OrganWeb/OrganWebPOIController.h"

Bool __stdcall SSWR::OrganWeb::OrganWebHandler::OnSessionDel(NN<Net::WebServer::WebSession> sess, AnyType userObj)
{
	NN<Data::DateTime> t;
	NN<Data::ArrayListInt32> pickObjs;
	t = sess->GetValuePtr(CSTR("LastUseTime")).GetNN<Data::DateTime>();
	pickObjs = sess->GetValuePtr(CSTR("PickObjs")).GetNN<Data::ArrayListInt32>();
	t.Delete();
	pickObjs.Delete();
	return false;
}

Bool __stdcall SSWR::OrganWeb::OrganWebHandler::OnSessionCheck(NN<Net::WebServer::WebSession> sess, AnyType userObj)
{
	NN<Data::DateTime> t;
	Data::DateTime currTime;
	if (sess->GetValuePtr(CSTR("LastUseTime")).GetOpt<Data::DateTime>().SetTo(t))
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
