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

void Net::WebServer::MemoryWebSession::SetValuePtr(const UTF8Char *name, UOSInt nameLen, void *val)
{
	this->items->PutC(name, nameLen, (Int64)val);
}

void Net::WebServer::MemoryWebSession::SetValueDbl(const UTF8Char *name, UOSInt nameLen, Double val)
{
	this->items->PutC(name, nameLen, *(Int64*)&val);
}

void Net::WebServer::MemoryWebSession::SetValueInt64(const UTF8Char *name, UOSInt nameLen, Int64 val)
{
	this->items->PutC(name, nameLen, val);
}

void Net::WebServer::MemoryWebSession::SetValueInt32(const UTF8Char *name, UOSInt nameLen, Int32 val)
{
	this->items->PutC(name, nameLen, val);
}

void *Net::WebServer::MemoryWebSession::GetValuePtr(const UTF8Char *name, UOSInt nameLen)
{
	return (void*)this->items->GetC(name, nameLen);
}

Double Net::WebServer::MemoryWebSession::GetValueDbl(const UTF8Char *name, UOSInt nameLen)
{
	Int64 ret = this->items->GetC(name, nameLen);
	return *(Double*)&ret;
}

Int64 Net::WebServer::MemoryWebSession::GetValueInt64(const UTF8Char *name, UOSInt nameLen)
{
	return this->items->GetC(name, nameLen);
}

Int32 Net::WebServer::MemoryWebSession::GetValueInt32(const UTF8Char *name, UOSInt nameLen)
{
	return (Int32)this->items->GetC(name, nameLen);
}
