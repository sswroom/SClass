#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Net/WebServer/MemoryWebSession.h"

Net::WebServer::MemoryWebSession::MemoryWebSession(Int64 sessId, Net::BrowserInfo::BrowserType browser, Manage::OSInfo::OSType os)
{
	this->browser = browser;
	this->os = os;
	this->sessId = sessId;
}

Net::WebServer::MemoryWebSession::~MemoryWebSession()
{
}

Bool Net::WebServer::MemoryWebSession::RequestValid(Net::BrowserInfo::BrowserType browser, Manage::OSInfo::OSType os)
{
	if (this->browser != Net::BrowserInfo::BT_UNKNOWN && this->browser != browser)
		return false;
	if (this->os != Manage::OSInfo::OT_UNKNOWN && this->os != os)
		return false;
	return true;
}

void Net::WebServer::MemoryWebSession::BeginUse()
{
	this->mut.Lock();
}

void Net::WebServer::MemoryWebSession::EndUse()
{
	this->mut.Unlock();
}

Int64 Net::WebServer::MemoryWebSession::GetSessId()
{
	return this->sessId;
}

void Net::WebServer::MemoryWebSession::SetValuePtr(Text::CStringNN name, void *val)
{
	this->items.PutC(name, (Int64)val);
}

void Net::WebServer::MemoryWebSession::SetValueDbl(Text::CStringNN name, Double val)
{
	this->items.PutC(name, *(Int64*)&val);
}

void Net::WebServer::MemoryWebSession::SetValueInt64(Text::CStringNN name, Int64 val)
{
	this->items.PutC(name, val);
}

void Net::WebServer::MemoryWebSession::SetValueInt32(Text::CStringNN name, Int32 val)
{
	this->items.PutC(name, val);
}

void *Net::WebServer::MemoryWebSession::GetValuePtr(Text::CStringNN name)
{
	return (void*)this->items.GetC(name);
}

Double Net::WebServer::MemoryWebSession::GetValueDbl(Text::CStringNN name)
{
	Int64 ret = this->items.GetC(name);
	return *(Double*)&ret;
}

Int64 Net::WebServer::MemoryWebSession::GetValueInt64(Text::CStringNN name)
{
	return this->items.GetC(name);
}

Int32 Net::WebServer::MemoryWebSession::GetValueInt32(Text::CStringNN name)
{
	return (Int32)this->items.GetC(name);
}
