#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "IO/Registry.h"

IO::Registry *IO::Registry::OpenSoftware(IO::Registry::RegistryUser usr, const WChar *compName, const WChar *appName)
{
	return 0;
}

IO::Registry *IO::Registry::OpenSoftware(IO::Registry::RegistryUser usr, const WChar *compName)
{
	return 0;
}


IO::Registry *IO::Registry::OpenLocalHardware()
{
	return 0;
}

IO::Registry *IO::Registry::OpenLocalSoftware(const WChar *softwareName)
{
	return 0;
}

void IO::Registry::CloseRegistry(IO::Registry *reg)
{
	DEL_CLASS(reg);
}

IO::Registry::Registry(void *hand)
{
	this->hand = hand;
}

IO::Registry::~Registry()
{
}

IO::Registry *IO::Registry::OpenSubReg(const WChar *name)
{
	return 0;
}

WChar *IO::Registry::GetSubReg(WChar *buff, UOSInt index)
{
	return 0;
}

void IO::Registry::SetValue(const WChar *name, Int32 value)
{
}

void IO::Registry::SetValue(const WChar *name, const WChar *value)
{
}

void IO::Registry::DelValue(const WChar *name)
{
}

Int32 IO::Registry::GetValueI32(const WChar *name)
{
	return 0;
}

WChar *IO::Registry::GetValueStr(const WChar *name, WChar *buff)
{
	return 0;
}

Bool IO::Registry::GetValueI32(const WChar *name, Int32 *value)
{
	return false;
}

WChar *IO::Registry::GetName(WChar *nameBuff, UOSInt index)
{
	return 0;
}
