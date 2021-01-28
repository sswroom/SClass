#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Net/WebServer/MemoryWebSession.h"

Net::WebServer::MemoryWebSession::MemoryWebSession(Int64 sessId, Net::BrowserInfo::BrowserType browser, Manage::OSInfo::OSType os)
{
	this->browser = browser;
	this->os = os;
	NEW_CLASS(this->names, Data::ArrayListStrUTF8());
	NEW_CLASS(this->vals, Data::ArrayListInt64());
	NEW_CLASS(this->mut, Sync::Mutex());
	this->sessId = sessId;
}

Net::WebServer::MemoryWebSession::~MemoryWebSession()
{
	OSInt i;
	DEL_CLASS(this->mut);
	DEL_CLASS(this->vals);
	i = this->names->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->names->GetItem(i));
	}
	DEL_CLASS(this->names);
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
	OSInt i = this->names->SortedIndexOf((const UTF8Char*)name);
	if (i >= 0)
	{
		this->vals->SetItem(i, (Int64)val);
	}
	else
	{
		i = this->names->SortedInsert(Text::StrCopyNew((const UTF8Char*)name));
		this->vals->Insert(i, (Int64)val);
	}
}

void Net::WebServer::MemoryWebSession::SetValueDbl(const Char *name, Double val)
{
	OSInt i = this->names->SortedIndexOf((const UTF8Char*)name);
	if (i >= 0)
	{
		this->vals->SetItem(i, *(Int64*)&val);
	}
	else
	{
		i = this->names->SortedInsert(Text::StrCopyNew((const UTF8Char*)name));
		this->vals->Insert(i, *(Int64*)&val);
	}
}

void Net::WebServer::MemoryWebSession::SetValueInt64(const Char *name, Int64 val)
{
	OSInt i = this->names->SortedIndexOf((const UTF8Char*)name);
	if (i >= 0)
	{
		this->vals->SetItem(i, val);
	}
	else
	{
		i = this->names->SortedInsert(Text::StrCopyNew((const UTF8Char*)name));
		this->vals->Insert(i, val);
	}
}

void Net::WebServer::MemoryWebSession::SetValueInt32(const Char *name, Int32 val)
{
	OSInt i = this->names->SortedIndexOf((const UTF8Char*)name);
	if (i >= 0)
	{
		this->vals->SetItem(i, val);
	}
	else
	{
		i = this->names->SortedInsert(Text::StrCopyNew((const UTF8Char*)name));
		this->vals->Insert(i, val);
	}
}

void *Net::WebServer::MemoryWebSession::GetValuePtr(const Char *name)
{
	OSInt i = this->names->SortedIndexOf((const UTF8Char*)name);
	if (i >= 0)
	{
		return (void*)this->vals->GetItem(i);
	}
	return 0;
}

Double Net::WebServer::MemoryWebSession::GetValueDbl(const Char *name)
{
	Int64 ret;
	OSInt i = this->names->SortedIndexOf((const UTF8Char*)name);
	if (i >= 0)
	{
		ret = this->vals->GetItem(i);
		return *(Double*)&ret;
	}
	return 0;
}

Int64 Net::WebServer::MemoryWebSession::GetValueInt64(const Char *name)
{
	OSInt i = this->names->SortedIndexOf((const UTF8Char*)name);
	if (i >= 0)
	{
		return this->vals->GetItem(i);
	}
	return 0;
}

Int32 Net::WebServer::MemoryWebSession::GetValueInt32(const Char *name)
{
	OSInt i = this->names->SortedIndexOf((const UTF8Char*)name);
	if (i >= 0)
	{
		return (Int32)this->vals->GetItem(i);
	}
	return 0;
}
