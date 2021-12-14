#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Net/WebServer/MemoryWebSession.h"

Net::WebServer::MemoryWebSession::MemoryWebSession(Int64 sessId, Net::BrowserInfo::BrowserType browser, Manage::OSInfo::OSType os)
{
	this->browser = browser;
	this->os = os;
	NEW_CLASS(this->items, Data::FastStringMap<Int64>());
	NEW_CLASS(this->mut, Sync::Mutex());
	this->sessId = sessId;
}

Net::WebServer::MemoryWebSession::~MemoryWebSession()
{
	DEL_CLASS(this->mut);
	DEL_CLASS(this->items);
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
	this->mut->Lock();
}

void Net::WebServer::MemoryWebSession::EndUse()
{
	this->mut->Unlock();
}

Int64 Net::WebServer::MemoryWebSession::GetSessId()
{
	return this->sessId;
}

void Net::WebServer::MemoryWebSession::SetValuePtr(const Char *name, void *val)
{
	this->items->Put((const UTF8Char*)name, (Int64)val);
}

void Net::WebServer::MemoryWebSession::SetValueDbl(const Char *name, Double val)
{
	this->items->Put((const UTF8Char*)name, *(Int64*)&val);
}

void Net::WebServer::MemoryWebSession::SetValueInt64(const Char *name, Int64 val)
{
	this->items->Put((const UTF8Char*)name, val);
}

void Net::WebServer::MemoryWebSession::SetValueInt32(const Char *name, Int32 val)
{
	this->items->Put((const UTF8Char*)name, val);
}

void *Net::WebServer::MemoryWebSession::GetValuePtr(const Char *name)
{
	return (void*)this->items->Get((const UTF8Char*)name);
}

Double Net::WebServer::MemoryWebSession::GetValueDbl(const Char *name)
{
	Int64 ret = this->items->Get((const UTF8Char*)name);;
	return *(Double*)&ret;
}

Int64 Net::WebServer::MemoryWebSession::GetValueInt64(const Char *name)
{
	return this->items->Get((const UTF8Char*)name);
}

Int32 Net::WebServer::MemoryWebSession::GetValueInt32(const Char *name)
{
	return (Int32)this->items->Get((const UTF8Char*)name);
}
