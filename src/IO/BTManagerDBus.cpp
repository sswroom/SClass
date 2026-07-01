#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/BTManagerDBus.h"

IO::BTManagerDBus::BTManagerDBus()
{
	this->client = nullptr;
	NEW_CLASSNN(this->dbusMgr, IO::DBusManager(IO::DBusManager::DBT_SYSTEM, 0));
	if (!this->dbusMgr->IsError())
	{
		this->dbusMgr->AttachObjectManager();
	}
}

IO::BTManagerDBus::~BTManagerDBus()
{
	this->client.Delete();
	this->dbusMgr.Delete();
}
