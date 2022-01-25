#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/OSInfo.h"

const UTF8Char *Manage::OSInfo::GetName(OSType osType)
{
	switch (osType)
	{
	case OT_WINDOWS_NT:
		return (const UTF8Char*)"Windows NT 32-bit";
	case OT_WINDOWS_CE:
		return (const UTF8Char*)"Windows CE";
	case OT_WINDOWS_SVR:
		return (const UTF8Char*)"Windows Server";
	case OT_LINUX_X86_64:
		return (const UTF8Char*)"Linux x86_64";
	case OT_ANDROID:
		return (const UTF8Char*)"Android";
	case OT_IPAD:
		return (const UTF8Char*)"iPad";
	case OT_IPHONE:
		return (const UTF8Char*)"iPhone";
	case OT_DARWIN:
		return (const UTF8Char*)"Darwin";
	case OT_MACOS:
		return (const UTF8Char*)"Mac OS";
	case OT_WINDOWS_NT64:
		return (const UTF8Char*)"Windows NT 64-bit";
	case OT_CHROMEOS:
		return (const UTF8Char*)"Chrome OS";
	case OT_LINUX_I686:
		return (const UTF8Char*)"Linux i686";
	case OT_NETCAST:
		return (const UTF8Char*)"webOS";
	case OT_UNKNOWN:
	default:
		return (const UTF8Char*)"Unknown";
	}
}

const UTF8Char *Manage::OSInfo::GetDefName(OSType osType)
{
	switch (osType)
	{
	case OT_WINDOWS_NT:
		return (const UTF8Char*)"OT_WINDOWS_NT";
	case OT_WINDOWS_CE:
		return (const UTF8Char*)"OT_WINDOWS_CE";
	case OT_WINDOWS_SVR:
		return (const UTF8Char*)"OT_WINDOWS_SVR";
	case OT_LINUX_X86_64:
		return (const UTF8Char*)"OT_LINUX_X86_64";
	case OT_ANDROID:
		return (const UTF8Char*)"OT_ANDROID";
	case OT_IPAD:
		return (const UTF8Char*)"OT_IPAD";
	case OT_IPHONE:
		return (const UTF8Char*)"OT_IPHONE";
	case OT_DARWIN:
		return (const UTF8Char*)"OT_DARWIN";
	case OT_MACOS:
		return (const UTF8Char*)"OT_MACOS";
	case OT_WINDOWS_NT64:
		return (const UTF8Char*)"OT_WINDOWS_NT64";
	case OT_CHROMEOS:
		return (const UTF8Char*)"OT_CHROMEOS";
	case OT_LINUX_I686:
		return (const UTF8Char*)"OT_LINUX_I686";
	case OT_NETCAST:
		return (const UTF8Char*)"OT_NETCAST";
	case OT_UNKNOWN:
	default:
		return (const UTF8Char*)"OT_UNKNOWN";
	}
}

void Manage::OSInfo::GetCommonName(Text::StringBuilderUTF8 *sb, OSType osType, const UTF8Char *osVer)
{
	UOSInt osVerLen;
	if (osVer)
		osVerLen = Text::StrCharCnt(osVer);
	else
		osVerLen = 0;
	if (osType == Manage::OSInfo::OT_DARWIN)
	{
		if (osVer == 0)
		{
			sb->AppendC(UTF8STRC("Mac OS"));
		}
		else if (Text::StrEqualsC(osVer, osVerLen, UTF8STRC("18.7.0")))
		{
			sb->AppendC(UTF8STRC("Mac OS 10.14.6"));
		}
		else
		{
			sb->AppendC(UTF8STRC("Mac OS (Darwin "));
			sb->AppendC(osVer, osVerLen);
			sb->AppendChar(')', 1);
		}
	}
	else if (osType == Manage::OSInfo::OT_MACOS)
	{
		if (osVer == 0)
		{
			sb->AppendC(UTF8STRC("Mac OS"));
		}
		else
		{
			sb->AppendC(UTF8STRC("Mac OS "));
			sb->AppendC(osVer, osVerLen);
		}
	}
	else if (osType == OT_WINDOWS_NT)
	{
		if (osVer == 0)
		{
			sb->AppendC(UTF8STRC("Windows NT (32-bit)"));
		}
		else if (Text::StrEqualsC(osVer, osVerLen, UTF8STRC("5.0")))
		{
			sb->AppendC(UTF8STRC("Windows 2000"));
		}
		else if (Text::StrEqualsC(osVer, osVerLen, UTF8STRC("5.1")))
		{
			sb->AppendC(UTF8STRC("Windows XP"));
		}
		else if (Text::StrEqualsC(osVer, osVerLen, UTF8STRC("6.0")))
		{
			sb->AppendC(UTF8STRC("Windows Vista"));
		}
		else if (Text::StrEqualsC(osVer, osVerLen, UTF8STRC("6.1")))
		{
			sb->AppendC(UTF8STRC("Windows 7 (32-bit)"));
		}
		else if (Text::StrEqualsC(osVer, osVerLen, UTF8STRC("6.2")))
		{
			sb->AppendC(UTF8STRC("Windows 8 (32-bit)"));
		}
		else if (Text::StrEqualsC(osVer, osVerLen, UTF8STRC("6.3")))
		{
			sb->AppendC(UTF8STRC("Windows 8.1 (32-bit)"));
		}
		else if (Text::StrEqualsC(osVer, osVerLen, UTF8STRC("10.0")))
		{
			sb->AppendC(UTF8STRC("Windows 10 (32-bit)"));
		}
		else
		{
			sb->AppendC(UTF8STRC("Windows NT "));
			sb->AppendC(osVer, osVerLen);
			sb->AppendC(UTF8STRC(" (32-bit)"));
		}
	}
	else if (osType == OT_WINDOWS_CE)
	{
		if (osVer == 0)
		{
			sb->AppendC(UTF8STRC("Windows CE"));
		}
		else
		{
			sb->AppendC(UTF8STRC("Windows CE "));
			sb->AppendC(osVer, osVerLen);
		}
	}
	else if (osType == OT_WINDOWS_SVR)
	{
		if (osVer == 0)
		{
			sb->AppendC(UTF8STRC("Windows Server"));
		}
		else
		{
			sb->AppendC(UTF8STRC("Windows "));
			sb->AppendC(osVer, osVerLen);
			sb->AppendC(UTF8STRC(" Server"));
		}
	}
	else if (osType == OT_LINUX_X86_64)
	{
		if (osVer == 0)
		{
			sb->AppendC(UTF8STRC("Linux x86_64"));
		}
		else
		{
			sb->AppendC(UTF8STRC("Linux x86_64 "));
			sb->AppendC(osVer, osVerLen);
		}
	}
	else if (osType == OT_LINUX_I686)
	{
		if (osVer == 0)
		{
			sb->AppendC(UTF8STRC("Linux i686"));
		}
		else
		{
			sb->AppendC(UTF8STRC("Linux i686 "));
			sb->AppendC(osVer, osVerLen);
		}
	}
	else if (osType == OT_ANDROID)
	{
		if (osVer == 0)
		{
			sb->AppendC(UTF8STRC("Android"));
		}
		else
		{
			sb->AppendC(UTF8STRC("Android "));
			sb->AppendC(osVer, osVerLen);
		}
	}
	else if (osType == OT_IPAD)
	{
		if (osVer == 0)
		{
			sb->AppendC(UTF8STRC("iPad"));
		}
		else
		{
			sb->AppendC(UTF8STRC("iPad "));
			sb->AppendC(osVer, osVerLen);
		}
	}
	else if (osType == OT_IPHONE)
	{
		if (osVer == 0)
		{
			sb->AppendC(UTF8STRC("iPhone"));
		}
		else
		{
			sb->AppendC(UTF8STRC("iPhone "));
			sb->AppendC(osVer, osVerLen);
		}
	}
	else if (osType == OT_WINDOWS_NT64)
	{
		if (osVer == 0)
		{
			sb->AppendC(UTF8STRC("Windows NT (64-bit)"));
		}
		else if (Text::StrEqualsC(osVer, osVerLen, UTF8STRC("5.2")))
		{
			sb->AppendC(UTF8STRC("Windows XP x64 Edition"));
		}
		else if (Text::StrEqualsC(osVer, osVerLen, UTF8STRC("6.1")))
		{
			sb->AppendC(UTF8STRC("Windows 7 (64-bit)"));
		}
		else if (Text::StrEqualsC(osVer, osVerLen, UTF8STRC("6.2")))
		{
			sb->AppendC(UTF8STRC("Windows 8 (64-bit)"));
		}
		else if (Text::StrEqualsC(osVer, osVerLen, UTF8STRC("6.3")))
		{
			sb->AppendC(UTF8STRC("Windows 8.1 (64-bit)"));
		}
		else if (Text::StrEqualsC(osVer, osVerLen, UTF8STRC("10.0")))
		{
			sb->AppendC(UTF8STRC("Windows 10 (64-bit)"));
		}
		else
		{
			sb->AppendC(UTF8STRC("Windows NT "));
			sb->AppendC(osVer, osVerLen);
			sb->AppendC(UTF8STRC(" (64-bit)"));
		}
	}
	else if (osType == OT_CHROMEOS)
	{
		sb->AppendC(UTF8STRC("Chrome OS"));
		if (osVer)
		{
			sb->AppendChar(' ', 1);
			sb->AppendC(osVer, osVerLen);
		}
	}
	else if (osType == OT_NETCAST)
	{
		sb->AppendC(UTF8STRC("webOS"));
		if (osVer)
		{
			sb->AppendChar(' ', 1);
			sb->AppendC(osVer, osVerLen);
		}
	}
	else
	{
		sb->AppendC(UTF8STRC("Unknown OS"));
		if (osVer)
		{
			sb->AppendChar(' ', 1);
			sb->AppendC(osVer, osVerLen);
		}
	}
}
