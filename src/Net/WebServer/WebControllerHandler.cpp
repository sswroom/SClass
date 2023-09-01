#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/WebServer/WebControllerHandler.h"

Net::WebServer::WebControllerHandler::~WebControllerHandler()
{
	WebController *ctrl;
	UOSInt i = this->ctrlList.GetCount();
	while (i-- > 0)
	{
		ctrl = this->ctrlList.GetItem(i);
		DEL_CLASS(ctrl);
	}
}

Bool Net::WebServer::WebControllerHandler::ProcessRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq)
{
	UOSInt i = 0;
	UOSInt j = this->ctrlList.GetCount();
	while (i < j)
	{
		if (this->ctrlList.GetItem(i)->ProcessRequest(req, resp, subReq))
			return true;
		i++;
	}
	if (this->DoRequest(req, resp, subReq))
	{
		return true;
	}
	if (this->rootDir->leng > 0)
	{
		return this->DoFileRequest(req, resp, subReq);
	}
	return false;
}

Net::WebServer::WebControllerHandler::WebControllerHandler() : HTTPDirectoryHandler(CSTR(""), false, 0, false)
{
}

Net::WebServer::WebControllerHandler::WebControllerHandler(Text::CString rootDir) : HTTPDirectoryHandler(rootDir, false, 0, false)
{
}

void Net::WebServer::WebControllerHandler::AddController(WebController *ctrl)
{
	this->ctrlList.Add(ctrl);
}
