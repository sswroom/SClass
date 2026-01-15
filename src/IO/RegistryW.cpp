#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Registry.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

#include <windows.h>

#ifdef _WIN32_WCE
#define RegCreateKeyW(key, name, outKey) RegCreateKeyEx(key, name, 0, 0, REG_OPTION_NON_VOLATILE, 0, 0, outKey, 0)
#endif

Optional<IO::Registry> IO::Registry::OpenSoftware(IO::Registry::RegistryUser usr, UnsafeArray<const WChar> compName, UnsafeArray<const WChar> appName)
{
	void *hand;
	WChar buff[256];
	UnsafeArray<WChar> wptr;
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

Optional<IO::Registry> IO::Registry::OpenSoftware(IO::Registry::RegistryUser usr, UnsafeArray<const WChar> compName)
{
	void *hand;
	WChar buff[256];
	UnsafeArray<WChar> wptr;
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


Optional<IO::Registry> IO::Registry::OpenLocalHardware()
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

Optional<IO::Registry> IO::Registry::OpenLocalSoftware(UnsafeArray<const WChar> softwareName)
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

void IO::Registry::CloseRegistry(NN<IO::Registry> reg)
{
	IO::Registry *preg = reg.Ptr();
	DEL_CLASS(preg);
}

IO::Registry::Registry(void *hand)
{
	this->clsData = (ClassData*)hand;
}

IO::Registry::~Registry()
{
	RegCloseKey((HKEY)this->clsData);
}

Optional<IO::Registry> IO::Registry::OpenSubReg(UnsafeArray<const WChar> name)
{
	void *newHand;
	if (RegCreateKeyW((HKEY)this->clsData, name.Ptr(), (HKEY*)&newHand) == 0)
	{
		IO::Registry *reg;
		NEW_CLASS(reg, IO::Registry(newHand));
		return reg;
	}
	return 0;
}

UnsafeArrayOpt<WChar> IO::Registry::GetSubReg(UnsafeArray<WChar> buff, UIntOS index)
{
	DWORD buffSize = 256;
	if (RegEnumKeyExW((HKEY)this->clsData, (DWORD)index, buff.Ptr(), &buffSize, 0, 0, 0, 0) == ERROR_SUCCESS)
		return &buff[Text::StrCharCnt(UnsafeArray<const WChar>(buff))];
	return 0;
}

void IO::Registry::SetValue(UnsafeArray<const WChar> name, Int32 value)
{
	RegSetValueExW((HKEY)this->clsData, name.Ptr(), 0, REG_DWORD, (LPBYTE)&value, 4);
}

void IO::Registry::SetValue(UnsafeArray<const WChar> name, UnsafeArrayOpt<const WChar> value)
{
	UnsafeArray<const WChar> nnvalue;
	if (!value.SetTo(nnvalue))
	{
		RegDeleteValueW((HKEY)this->clsData, name.Ptr());
	}
	else
	{
		RegSetValueExW((HKEY)this->clsData, name.Ptr(), 0, REG_SZ, (LPBYTE)nnvalue.Ptr(), (DWORD)(Text::StrCharCnt(nnvalue) * sizeof(WChar)));
	}
}

void IO::Registry::DelValue(UnsafeArray<const WChar> name)
{
	RegDeleteValueW((HKEY)this->clsData, name.Ptr());
}

Int32 IO::Registry::GetValueI32(UnsafeArray<const WChar> name)
{
	DWORD regType;
	BYTE buff[512];
	DWORD cbData = 512;
	if (RegQueryValueExW((HKEY)this->clsData, name.Ptr(), 0, &regType, buff, &cbData) == ERROR_SUCCESS)
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
			return Text::StrToInt32W(UnsafeArray<WChar>((WChar*)buff));
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

UnsafeArrayOpt<WChar> IO::Registry::GetValueStr(UnsafeArray<const WChar> name, UnsafeArray<WChar> buff)
{
	Int32 result;
	DWORD regType;
	DWORD cbData = 512;
	if ((result = RegQueryValueExW((HKEY)this->clsData, name.Ptr(), 0, &regType, (LPBYTE)buff.Ptr(), &cbData)) == ERROR_SUCCESS)
	{
		if (regType == REG_SZ)
		{
			if (buff[(UIntOS)(cbData >> 1) - 1])
			{
				buff[(UIntOS)cbData >> 1] = 0;
			}
			else
			{
				cbData -= 2;
			}
			return &buff[(UIntOS)cbData >> 1];
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

Bool IO::Registry::GetValueI32(UnsafeArray<const WChar> name, OutParam<Int32> value)
{
	DWORD regType;
	BYTE buff[512];
	DWORD cbData = 512;
	if (RegQueryValueExW((HKEY)this->clsData, name.Ptr(), 0, &regType, buff, &cbData) == ERROR_SUCCESS)
	{
		if (regType == REG_DWORD)
		{
			value.Set(*(Int32*)buff);
			return true;
		}
		else if (regType == REG_SZ)
		{
			if (*(WChar *)&buff[cbData - 2])
			{
				*(WChar*)&buff[cbData] = 0;
			}
			value.Set(Text::StrToInt32W(UnsafeArray<WChar>((WChar*)buff)));
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

UnsafeArrayOpt<WChar> IO::Registry::GetName(UnsafeArray<WChar> nameBuff, UIntOS index)
{
	Int32 result;
	UInt32 buffSize = 256;
	if ((result = RegEnumValueW((HKEY)this->clsData, (DWORD)index, nameBuff.Ptr(), (LPDWORD)&buffSize, 0, 0, 0, 0)) == ERROR_SUCCESS)
	{
		return &nameBuff[buffSize];
	}
	else
	{
		return 0;
	}
}
