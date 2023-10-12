#include "Stdafx.h"
#include "Net/WebServer/NodeModuleHandler.h"

Net::WebServer::NodeModuleHandler::NodeModuleHandler(Text::CStringNN rootDir, UInt64 fileCacheSize) : HTTPDirectoryHandler(rootDir, false, fileCacheSize, false)
{
}

Net::WebServer::NodeModuleHandler::~NodeModuleHandler()
{
}

Bool Net::WebServer::NodeModuleHandler::FileValid(Text::CStringNN subReq)
{
	if (subReq.EndsWith(UTF8STRC(".ts")))
		return false;
	if (subReq.EndsWith(UTF8STRC("/package.json")))
		return false;
	if (subReq.EndsWith(UTF8STRC(".md")))
		return false;
	UOSInt i = subReq.LastIndexOf('/');
	if (subReq.IndexOf('.', i + 1) == INVALID_INDEX)
		return false;
	return true;
}
