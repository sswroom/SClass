#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/WebServer/IWebSessionManager.h"

Net::WebServer::IWebSessionManager::IWebSessionManager(SessionHandler delHdlr, AnyType delHdlrObj)
{
	this->delHdlr = delHdlr;
	this->delHdlrObj = delHdlrObj;
}

Net::WebServer::IWebSessionManager::~IWebSessionManager()
{
}
