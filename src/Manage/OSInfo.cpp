#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/OSInfo.h"

Text::CStringNN Manage::OSInfo::GetName(OSType osType)
{
	switch (osType)
	{
	case OT_WINDOWS_NT:
		return CSTR("Windows NT 32-bit");
	case OT_WINDOWS_CE:
		return CSTR("Windows CE");
	case OT_WINDOWS_SVR:
		return CSTR("Windows Server");
	case OT_LINUX_X86_64:
		return CSTR("Linux x86_64");
	case OT_ANDROID:
		return CSTR("Android");
	case OT_IPAD:
		return CSTR("iPad");
	case OT_IPHONE:
		return CSTR("iPhone");
	case OT_DARWIN:
		return CSTR("Darwin");
	case OT_MACOS:
		return CSTR("Mac OS");
	case OT_WINDOWS_NT64:
		return CSTR("Windows NT 64-bit");
	case OT_CHROMEOS:
		return CSTR("Chrome OS");
	case OT_LINUX_I686:
		return CSTR("Linux i686");
	case OT_NETCAST:
		return CSTR("webOS");
	case OT_UNKNOWN:
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN Manage::OSInfo::GetDefName(OSType osType)
{
	switch (osType)
	{
	case OT_WINDOWS_NT:
		return CSTR("OT_WINDOWS_NT");
	case OT_WINDOWS_CE:
		return CSTR("OT_WINDOWS_CE");
	case OT_WINDOWS_SVR:
		return CSTR("OT_WINDOWS_SVR");
	case OT_LINUX_X86_64:
		return CSTR("OT_LINUX_X86_64");
	case OT_ANDROID:
		return CSTR("OT_ANDROID");
	case OT_IPAD:
		return CSTR("OT_IPAD");
	case OT_IPHONE:
		return CSTR("OT_IPHONE");
	case OT_DARWIN:
		return CSTR("OT_DARWIN");
	case OT_MACOS:
		return CSTR("OT_MACOS");
	case OT_WINDOWS_NT64:
		return CSTR("OT_WINDOWS_NT64");
	case OT_CHROMEOS:
		return CSTR("OT_CHROMEOS");
	case OT_LINUX_I686:
		return CSTR("OT_LINUX_I686");
	case OT_NETCAST:
		return CSTR("OT_NETCAST");
	case OT_UNKNOWN:
	default:
		return CSTR("OT_UNKNOWN");
	}
}

void Manage::OSInfo::GetCommonName(NN<Text::StringBuilderUTF8> sb, OSType osType, Text::CString osVer)
{
	Text::CStringNN nns;
	if (osType == Manage::OSInfo::OT_DARWIN)
	{
		if (!osVer.SetTo(nns) || nns.leng == 0)
		{
			sb->AppendC(UTF8STRC("Mac OS"));
		}
		else if (nns.Equals(UTF8STRC("18.7.0")))
		{
			sb->AppendC(UTF8STRC("Mac OS 10.14.6"));
		}
		else
		{
			sb->AppendC(UTF8STRC("Mac OS (Darwin "));
			sb->Append(nns);
			sb->AppendUTF8Char(')');
		}
	}
	else if (osType == Manage::OSInfo::OT_MACOS)
	{
		if (!osVer.SetTo(nns) || nns.leng == 0)
		{
			sb->AppendC(UTF8STRC("Mac OS"));
		}
		else
		{
			sb->AppendC(UTF8STRC("Mac OS "));
			sb->Append(nns);
		}
	}
	else if (osType == OT_WINDOWS_NT)
	{
		if (!osVer.SetTo(nns) || nns.leng == 0)
		{
			sb->AppendC(UTF8STRC("Windows NT (32-bit)"));
		}
		else if (nns.Equals(UTF8STRC("5.0")))
		{
			sb->AppendC(UTF8STRC("Windows 2000"));
		}
		else if (nns.Equals(UTF8STRC("5.1")))
		{
			sb->AppendC(UTF8STRC("Windows XP"));
		}
		else if (nns.Equals(UTF8STRC("6.0")))
		{
			sb->AppendC(UTF8STRC("Windows Vista"));
		}
		else if (nns.Equals(UTF8STRC("6.1")))
		{
			sb->AppendC(UTF8STRC("Windows 7 (32-bit)"));
		}
		else if (nns.Equals(UTF8STRC("6.2")))
		{
			sb->AppendC(UTF8STRC("Windows 8 (32-bit)"));
		}
		else if (nns.Equals(UTF8STRC("6.3")))
		{
			sb->AppendC(UTF8STRC("Windows 8.1 (32-bit)"));
		}
		else if (nns.Equals(UTF8STRC("10.0")))
		{
			sb->AppendC(UTF8STRC("Windows 10 (32-bit)"));
		}
		else
		{
			sb->AppendC(UTF8STRC("Windows NT "));
			sb->Append(nns);
			sb->AppendC(UTF8STRC(" (32-bit)"));
		}
	}
	else if (osType == OT_WINDOWS_CE)
	{
		if (!osVer.SetTo(nns) || nns.leng == 0)
		{
			sb->AppendC(UTF8STRC("Windows CE"));
		}
		else
		{
			sb->AppendC(UTF8STRC("Windows CE "));
			sb->Append(nns);
		}
	}
	else if (osType == OT_WINDOWS_SVR)
	{
		if (!osVer.SetTo(nns) || nns.leng == 0)
		{
			sb->AppendC(UTF8STRC("Windows Server"));
		}
		else
		{
			sb->AppendC(UTF8STRC("Windows "));
			sb->Append(nns);
			sb->AppendC(UTF8STRC(" Server"));
		}
	}
	else if (osType == OT_LINUX_X86_64)
	{
		if (!osVer.SetTo(nns) || nns.leng == 0)
		{
			sb->AppendC(UTF8STRC("Linux x86_64"));
		}
		else
		{
			sb->AppendC(UTF8STRC("Linux x86_64 "));
			sb->Append(nns);
		}
	}
	else if (osType == OT_LINUX_I686)
	{
		if (!osVer.SetTo(nns) || nns.leng == 0)
		{
			sb->AppendC(UTF8STRC("Linux i686"));
		}
		else
		{
			sb->AppendC(UTF8STRC("Linux i686 "));
			sb->Append(nns);
		}
	}
	else if (osType == OT_ANDROID)
	{
		if (!osVer.SetTo(nns) || nns.leng == 0)
		{
			sb->AppendC(UTF8STRC("Android"));
		}
		else
		{
			sb->AppendC(UTF8STRC("Android "));
			sb->Append(nns);
		}
	}
	else if (osType == OT_IPAD)
	{
		if (!osVer.SetTo(nns) || nns.leng == 0)
		{
			sb->AppendC(UTF8STRC("iPad"));
		}
		else
		{
			sb->AppendC(UTF8STRC("iPad "));
			sb->Append(nns);
		}
	}
	else if (osType == OT_IPHONE)
	{
		if (!osVer.SetTo(nns) || nns.leng == 0)
		{
			sb->AppendC(UTF8STRC("iPhone"));
		}
		else
		{
			sb->AppendC(UTF8STRC("iPhone "));
			sb->Append(nns);
		}
	}
	else if (osType == OT_WINDOWS_NT64)
	{
		if (!osVer.SetTo(nns) || nns.leng == 0)
		{
			sb->AppendC(UTF8STRC("Windows NT (64-bit)"));
		}
		else if (nns.Equals(UTF8STRC("5.2")))
		{
			sb->AppendC(UTF8STRC("Windows XP x64 Edition"));
		}
		else if (nns.Equals(UTF8STRC("6.1")))
		{
			sb->AppendC(UTF8STRC("Windows 7 (64-bit)"));
		}
		else if (nns.Equals(UTF8STRC("6.2")))
		{
			sb->AppendC(UTF8STRC("Windows 8 (64-bit)"));
		}
		else if (nns.Equals(UTF8STRC("6.3")))
		{
			sb->AppendC(UTF8STRC("Windows 8.1 (64-bit)"));
		}
		else if (nns.Equals(UTF8STRC("10.0")))
		{
			sb->AppendC(UTF8STRC("Windows 10 (64-bit)"));
		}
		else
		{
			sb->AppendC(UTF8STRC("Windows NT "));
			sb->Append(nns);
			sb->AppendC(UTF8STRC(" (64-bit)"));
		}
	}
	else if (osType == OT_CHROMEOS)
	{
		sb->AppendC(UTF8STRC("Chrome OS"));
		if (osVer.SetTo(nns) && nns.leng > 0)
		{
			sb->AppendUTF8Char(' ');
			sb->Append(nns);
		}
	}
	else if (osType == OT_NETCAST)
	{
		sb->AppendC(UTF8STRC("webOS"));
		if (osVer.SetTo(nns) && nns.leng > 0)
		{
			sb->AppendUTF8Char(' ');
			sb->Append(nns);
		}
	}
	else
	{
		sb->AppendC(UTF8STRC("Unknown OS"));
		if (osVer.SetTo(nns) && nns.leng > 0)
		{
			sb->AppendUTF8Char(' ');
			sb->Append(nns);
		}
	}
}
