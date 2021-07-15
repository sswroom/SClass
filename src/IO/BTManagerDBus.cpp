#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/BTManagerDBus.h"

IO::BTManagerDBus::BTManagerDBus()
{
	NEW_CLASS(this->dbusMgr, IO::DBusManager(IO::DBusManager::DBT_SYSTEM));
}

IO::BTManagerDBus::~BTManagerDBus()
{
	DEL_CLASS(this->dbusMgr);
}
