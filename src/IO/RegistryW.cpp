#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Registry.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

#include <windows.h>

#ifdef _WIN32_WCE
#define RegCreateKeyW(key, name, outKey) RegCreateKeyEx(key, name, 0, 0, REG_OPTION_NON_VOLATILE, 0, 0, outKey, 0)
#endif

IO::Registry *IO::Registry::OpenSoftware(IO::Registry::RegistryUser usr, const WChar *compName, const WChar *appName)
{
	void *hand;
	WChar buff[256];
	WChar *wptr;
	IO::Registry *reg = 0;
	wptr = Text::StrConcat(buff, L"Software\\");
	wptr = Text::StrConcat(wptr, compName);
	wptr = Text::StrConcat(wptr, L"\\");
	wptr = Text::StrConcat(wptr, appName);
	if (usr == IO::Registry::REG_USER_ALL)
	{
		if (RegCreateKeyW(HKEY_LOCAL_MACHINE, buff, (HKEY*)&hand) == 0)
		{
			NEW_CLASS(reg, IO::Registry(hand));
		}
		return reg;
	}
	else if (usr == IO::Registry::REG_USER_THIS)
	{
		if (RegCreateKeyW(HKEY_CURRENT_USER, buff, (HKEY*)&hand) == 0)
		{
			NEW_CLASS(reg, IO::Registry(hand));
		}
		return reg;
	}
	else
	{
		return 0;
	}
}

IO::Registry *IO::Registry::OpenSoftware(IO::Registry::RegistryUser usr, const WChar *compName)
{
	void *hand;
	WChar buff[256];
	WChar *wptr;
	IO::Registry *reg = 0;
	wptr = Text::StrConcat(buff, L"Software\\");
	wptr = Text::StrConcat(wptr, compName);
	if (usr == IO::Registry::REG_USER_ALL)
	{
		if (RegCreateKeyW(HKEY_LOCAL_MACHINE, buff, (HKEY*)&hand) == 0)
		{
			NEW_CLASS(reg, IO::Registry(hand));
		}
		return reg;
	}
	else if (usr == IO::Registry::REG_USER_THIS)
	{
		if (RegCreateKeyW(HKEY_CURRENT_USER, buff, (HKEY*)&hand) == 0)
		{
			NEW_CLASS(reg, IO::Registry(hand));
		}
		return reg;
	}
	else
	{
		return 0;
	}
}


IO::Registry *IO::Registry::OpenLocalHardware()
{
	IO::Registry *reg = 0;
	void *hand;
	if (RegCreateKeyW(HKEY_LOCAL_MACHINE, L"HARDWARE", (HKEY*)&hand) == 0)
	{
		NEW_CLASS(reg, IO::Registry(hand));
		return reg;
	}
	else
	{
		return 0;
	}
}

IO::Registry *IO::Registry::OpenLocalSoftware(const WChar *softwareName)
{
	IO::Registry *reg = 0;
	WChar wbuff[512];
	void *hand;
	Text::StrConcat(Text::StrConcat(wbuff, L"SOFTWARE\\"), softwareName);
	if (RegCreateKeyW(HKEY_LOCAL_MACHINE, wbuff, (HKEY*)&hand) == 0)
	{
		NEW_CLASS(reg, IO::Registry(hand));
		return reg;
	}
	else
	{
		return 0;
	}
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
	RegCloseKey((HKEY)this->hand);
}

IO::Registry *IO::Registry::OpenSubReg(const WChar *name)
{
	void *newHand;
	if (RegCreateKeyW((HKEY)this->hand, name, (HKEY*)&newHand) == 0)
	{
		IO::Registry *reg;
		NEW_CLASS(reg, IO::Registry(newHand));
		return reg;
	}
	return 0;
}

WChar *IO::Registry::GetSubReg(WChar *buff, UOSInt index)
{
	DWORD buffSize = 256;
	if (RegEnumKeyExW((HKEY)this->hand, (DWORD)index, buff, &buffSize, 0, 0, 0, 0) == ERROR_SUCCESS)
		return &buff[Text::StrCharCnt(buff)];
	return 0;
}

void IO::Registry::SetValue(const WChar *name, Int32 value)
{
	RegSetValueExW((HKEY)this->hand, name, 0, REG_DWORD, (LPBYTE)&value, 4);
}

void IO::Registry::SetValue(const WChar *name, const WChar *value)
{
	if (value == 0)
	{
		RegDeleteValueW((HKEY)this->hand, name);
	}
	else
	{
		RegSetValueExW((HKEY)this->hand, name, 0, REG_SZ, (LPBYTE)value, (DWORD)(Text::StrCharCnt(value) * sizeof(WChar)));
	}
}

void IO::Registry::DelValue(const WChar *name)
{
	RegDeleteValueW((HKEY)this->hand, name);
}

Int32 IO::Registry::GetValueI32(const WChar *name)
{
	DWORD regType;
	BYTE buff[512];
	DWORD cbData = 512;
	if (RegQueryValueExW((HKEY)this->hand, name, 0, &regType, buff, &cbData) == ERROR_SUCCESS)
	{
		if (regType == REG_DWORD)
		{
			return *(Int32*)buff;
		}
		else if (regType == REG_SZ)
		{
			if (*(WChar *)&buff[cbData - 2])
			{
				*(WChar*)&buff[cbData] = 0;
			}
			return Text::StrToInt32((WChar*)buff);
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

WChar *IO::Registry::GetValueStr(const WChar *name, WChar *buff)
{
	Int32 result;
	DWORD regType;
	DWORD cbData = 512;
	if ((result = RegQueryValueExW((HKEY)this->hand, name, 0, &regType, (LPBYTE)buff, &cbData)) == ERROR_SUCCESS)
	{
		if (regType == REG_SZ)
		{
			if (buff[(cbData >> 1) - 1])
			{
				buff[cbData >> 1] = 0;
			}
			else
			{
				cbData -= 2;
			}
			return &buff[cbData >> 1];
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

Bool IO::Registry::GetValueI32(const WChar *name, Int32 *value)
{
	DWORD regType;
	BYTE buff[512];
	DWORD cbData = 512;
	if (RegQueryValueExW((HKEY)this->hand, name, 0, &regType, buff, &cbData) == ERROR_SUCCESS)
	{
		if (regType == REG_DWORD)
		{
			*value = *(Int32*)buff;
			return true;
		}
		else if (regType == REG_SZ)
		{
			if (*(WChar *)&buff[cbData - 2])
			{
				*(WChar*)&buff[cbData] = 0;
			}
			*value = Text::StrToInt32((WChar*)buff);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

WChar *IO::Registry::GetName(WChar *nameBuff, UOSInt index)
{
	Int32 result;
	UInt32 buffSize = 256;
	if ((result = RegEnumValueW((HKEY)this->hand, (DWORD)index, nameBuff, (LPDWORD)&buffSize, 0, 0, 0, 0)) == ERROR_SUCCESS)
	{
		return &nameBuff[buffSize];
	}
	else
	{
		return 0;
	}
}
