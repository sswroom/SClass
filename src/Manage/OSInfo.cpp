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
	default:
		return (const UTF8Char*)"OT_UNKNOWN";
	}
}

void Manage::OSInfo::GetCommonName(Text::StringBuilderUTF *sb, OSType osType, const UTF8Char *osVer)
{
	if (osType == Manage::OSInfo::OT_DARWIN)
	{
		if (osVer == 0)
		{
			sb->Append((const UTF8Char*)"Mac OS");
		}
		else if (Text::StrEquals(osVer, (const UTF8Char*)"18.7.0"))
		{
			sb->Append((const UTF8Char*)"Mac OS 10.14.6");
		}
		else
		{
			sb->Append((const UTF8Char*)"Mac OS (Darwin ");
			sb->Append(osVer);
			sb->AppendChar(')', 1);
		}
	}
	else if (osType == Manage::OSInfo::OT_MACOS)
	{
		if (osVer == 0)
		{
			sb->Append((const UTF8Char*)"Mac OS");
		}
		else
		{
			sb->Append((const UTF8Char*)"Mac OS ");
			sb->Append(osVer);
		}
	}
	else if (osType == OT_WINDOWS_NT)
	{
		if (osVer == 0)
		{
			sb->Append((const UTF8Char*)"Windows NT (32-bit)");
		}
		else if (Text::StrEquals(osVer, (const UTF8Char*)"5.0"))
		{
			sb->Append((const UTF8Char*)"Windows 2000");
		}
		else if (Text::StrEquals(osVer, (const UTF8Char*)"5.1"))
		{
			sb->Append((const UTF8Char*)"Windows XP");
		}
		else if (Text::StrEquals(osVer, (const UTF8Char*)"6.0"))
		{
			sb->Append((const UTF8Char*)"Windows Vista");
		}
		else if (Text::StrEquals(osVer, (const UTF8Char*)"6.1"))
		{
			sb->Append((const UTF8Char*)"Windows 7 (32-bit)");
		}
		else if (Text::StrEquals(osVer, (const UTF8Char*)"6.2"))
		{
			sb->Append((const UTF8Char*)"Windows 8 (32-bit)");
		}
		else if (Text::StrEquals(osVer, (const UTF8Char*)"6.3"))
		{
			sb->Append((const UTF8Char*)"Windows 8.1 (32-bit)");
		}
		else if (Text::StrEquals(osVer, (const UTF8Char*)"10.0"))
		{
			sb->Append((const UTF8Char*)"Windows 10 (32-bit)");
		}
		else
		{
			sb->Append((const UTF8Char*)"Windows NT ");
			sb->Append(osVer);
			sb->Append((const UTF8Char*)" (32-bit)");
		}
	}
	else if (osType == OT_WINDOWS_CE)
	{
		if (osVer == 0)
		{
			sb->Append((const UTF8Char*)"Windows CE");
		}
		else
		{
			sb->Append((const UTF8Char*)"Windows CE ");
			sb->Append(osVer);
		}
	}
	else if (osType == OT_WINDOWS_SVR)
	{
		if (osVer == 0)
		{
			sb->Append((const UTF8Char*)"Windows Server");
		}
		else
		{
			sb->Append((const UTF8Char*)"Windows ");
			sb->Append(osVer);
			sb->Append((const UTF8Char*)" Server");
		}
	}
	else if (osType == OT_LINUX_X86_64)
	{
		if (osVer == 0)
		{
			sb->Append((const UTF8Char*)"Linux x86_64");
		}
		else
		{
			sb->Append((const UTF8Char*)"Linux x86_64 ");
			sb->Append(osVer);
		}
	}
	else if (osType == OT_LINUX_I686)
	{
		if (osVer == 0)
		{
			sb->Append((const UTF8Char*)"Linux i686");
		}
		else
		{
			sb->Append((const UTF8Char*)"Linux i686 ");
			sb->Append(osVer);
		}
	}
	else if (osType == OT_ANDROID)
	{
		if (osVer == 0)
		{
			sb->Append((const UTF8Char*)"Android");
		}
		else
		{
			sb->Append((const UTF8Char*)"Android ");
			sb->Append(osVer);
		}
	}
	else if (osType == OT_IPAD)
	{
		if (osVer == 0)
		{
			sb->Append((const UTF8Char*)"iPad");
		}
		else
		{
			sb->Append((const UTF8Char*)"iPad ");
			sb->Append(osVer);
		}
	}
	else if (osType == OT_IPHONE)
	{
		if (osVer == 0)
		{
			sb->Append((const UTF8Char*)"iPhone");
		}
		else
		{
			sb->Append((const UTF8Char*)"iPhone ");
			sb->Append(osVer);
		}
	}
	else if (osType == OT_WINDOWS_NT64)
	{
		if (osVer == 0)
		{
			sb->Append((const UTF8Char*)"Windows NT (64-bit)");
		}
		else if (Text::StrEquals(osVer, (const UTF8Char*)"5.2"))
		{
			sb->Append((const UTF8Char*)"Windows XP x64 Edition");
		}
		else if (Text::StrEquals(osVer, (const UTF8Char*)"6.1"))
		{
			sb->Append((const UTF8Char*)"Windows 7 (64-bit)");
		}
		else if (Text::StrEquals(osVer, (const UTF8Char*)"6.2"))
		{
			sb->Append((const UTF8Char*)"Windows 8 (64-bit)");
		}
		else if (Text::StrEquals(osVer, (const UTF8Char*)"6.3"))
		{
			sb->Append((const UTF8Char*)"Windows 8.1 (64-bit)");
		}
		else if (Text::StrEquals(osVer, (const UTF8Char*)"10.0"))
		{
			sb->Append((const UTF8Char*)"Windows 10 (64-bit)");
		}
		else
		{
			sb->Append((const UTF8Char*)"Windows NT ");
			sb->Append(osVer);
			sb->Append((const UTF8Char*)" (64-bit)");
		}
	}
	else if (osType == OT_CHROMEOS)
	{
		sb->Append((const UTF8Char*)"Chrome OS");
		if (osVer)
		{
			sb->AppendChar(' ', 1);
			sb->Append(osVer);
		}
	}
	else if (osType == OT_NETCAST)
	{
		sb->Append((const UTF8Char*)"webOS");
		if (osVer)
		{
			sb->AppendChar(' ', 1);
			sb->Append(osVer);
		}
	}
	else
	{
		sb->Append((const UTF8Char*)"Unknown OS");
		if (osVer)
		{
			sb->AppendChar(' ', 1);
			sb->Append(osVer);
		}
	}
}
