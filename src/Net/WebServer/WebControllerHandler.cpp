#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/WebServer/WebControllerHandler.h"

Net::WebServer::WebControllerHandler::~WebControllerHandler()
{
	NN<WebController> ctrl;
	UIntOS i = this->ctrlList.GetCount();
	while (i-- > 0)
	{
		ctrl = this->ctrlList.GetItemNoCheck(i);
		ctrl.Delete();
	}
}

Bool Net::WebServer::WebControllerHandler::ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq)
{
	UIntOS i = 0;
	UIntOS j = this->ctrlList.GetCount();
	while (i < j)
	{
		if (this->ctrlList.GetItemNoCheck(i)->ProcessRequest(req, resp, subReq))
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

Net::WebServer::WebControllerHandler::WebControllerHandler(Text::CStringNN rootDir) : HTTPDirectoryHandler(rootDir, false, 0, false)
{
}

void Net::WebServer::WebControllerHandler::AddController(NN<WebController> ctrl)
{
	this->ctrlList.Add(ctrl);
}
