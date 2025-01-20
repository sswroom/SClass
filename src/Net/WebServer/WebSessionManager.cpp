#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/WebServer/WebSessionManager.h"

Net::WebServer::WebSessionManager::WebSessionManager(SessionHandler delHdlr, AnyType delHdlrObj)
{
	this->delHdlr = delHdlr;
	this->delHdlrObj = delHdlrObj;
}

Net::WebServer::WebSessionManager::~WebSessionManager()
{
}
