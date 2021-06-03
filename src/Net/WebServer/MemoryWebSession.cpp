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
	UOSInt i;
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
	OSInt si = this->names->SortedIndexOf((const UTF8Char*)name);
	if (si >= 0)
	{
		this->vals->SetItem((UOSInt)si, (Int64)val);
	}
	else
	{
		UOSInt i = this->names->SortedInsert(Text::StrCopyNew((const UTF8Char*)name));
		this->vals->Insert(i, (Int64)val);
	}
}

void Net::WebServer::MemoryWebSession::SetValueDbl(const Char *name, Double val)
{
	OSInt si = this->names->SortedIndexOf((const UTF8Char*)name);
	if (si >= 0)
	{
		this->vals->SetItem((UOSInt)si, *(Int64*)&val);
	}
	else
	{
		UOSInt i = this->names->SortedInsert(Text::StrCopyNew((const UTF8Char*)name));
		this->vals->Insert(i, *(Int64*)&val);
	}
}

void Net::WebServer::MemoryWebSession::SetValueInt64(const Char *name, Int64 val)
{
	OSInt si = this->names->SortedIndexOf((const UTF8Char*)name);
	if (si >= 0)
	{
		this->vals->SetItem((UOSInt)si, val);
	}
	else
	{
		UOSInt i = this->names->SortedInsert(Text::StrCopyNew((const UTF8Char*)name));
		this->vals->Insert(i, val);
	}
}

void Net::WebServer::MemoryWebSession::SetValueInt32(const Char *name, Int32 val)
{
	OSInt si = this->names->SortedIndexOf((const UTF8Char*)name);
	if (si >= 0)
	{
		this->vals->SetItem((UOSInt)si, val);
	}
	else
	{
		UOSInt i = this->names->SortedInsert(Text::StrCopyNew((const UTF8Char*)name));
		this->vals->Insert(i, val);
	}
}

void *Net::WebServer::MemoryWebSession::GetValuePtr(const Char *name)
{
	OSInt si = this->names->SortedIndexOf((const UTF8Char*)name);
	if (si >= 0)
	{
		return (void*)this->vals->GetItem((UOSInt)si);
	}
	return 0;
}

Double Net::WebServer::MemoryWebSession::GetValueDbl(const Char *name)
{
	Int64 ret;
	OSInt si = this->names->SortedIndexOf((const UTF8Char*)name);
	if (si >= 0)
	{
		ret = this->vals->GetItem((UOSInt)si);
		return *(Double*)&ret;
	}
	return 0;
}

Int64 Net::WebServer::MemoryWebSession::GetValueInt64(const Char *name)
{
	OSInt si = this->names->SortedIndexOf((const UTF8Char*)name);
	if (si >= 0)
	{
		return this->vals->GetItem((UOSInt)si);
	}
	return 0;
}

Int32 Net::WebServer::MemoryWebSession::GetValueInt32(const Char *name)
{
	OSInt si = this->names->SortedIndexOf((const UTF8Char*)name);
	if (si >= 0)
	{
		return (Int32)this->vals->GetItem((UOSInt)si);
	}
	return 0;
}
