#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/BTManagerDBus.h"

IO::BTManagerDBus::BTManagerDBus()
{
	this->client = 0;
	NEW_CLASS(this->dbusMgr, IO::DBusManager(IO::DBusManager::DBT_SYSTEM, 0));
	if (!this->dbusMgr->IsError())
	{
		this->dbusMgr->AttachObjectManager();
	}
}

IO::BTManagerDBus::~BTManagerDBus()
{
	SDEL_CLASS(this->client);
	DEL_CLASS(this->dbusMgr);
}
