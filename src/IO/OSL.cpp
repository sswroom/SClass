#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/OS.h"
#include "IO/Path.h"
#include "IO/UnixConfigFile.h"
#include "Manage/Process.h"
#include "Text/MyString.h"
#include "Text/UTF8Reader.h"

UnsafeArrayOpt<UTF8Char> IO::OS::GetDistro(UnsafeArray<UTF8Char> sbuff)
{
	if (IO::Path::GetPathType(CSTR("/bin/nvram")) == IO::Path::PathType::File)
	{
		Text::StringBuilderUTF8 sb;
		Manage::Process::ExecuteProcess(CSTR("nvram get productid"), sb);
		while (sb.EndsWith('\r') || sb.EndsWith('\n'))
		{
			sb.RemoveChars(1);
		}
		if (sb.GetLength() > 0)
		{
			return Text::StrConcatC(sbuff, sb.ToString(), sb.GetLength());
		}
	}
	if (IO::Path::GetPathType(CSTR("/etc/os-release")) == IO::Path::PathType::File)
	{
		IO::ConfigFile *cfg = IO::UnixConfigFile::Parse(CSTR("/etc/os-release"));
		if (cfg)
		{
			NN<Text::String> s;
			if (cfg->GetValue(CSTR("NAME")).SetTo(s))
			{
				sbuff = s->ConcatTo(sbuff);
			}
			DEL_CLASS(cfg);
			return sbuff;
		}
	}
	if (IO::Path::GetPathType(CSTR("/sbin/getcfg")) == IO::Path::PathType::File)
	{
		return Text::StrConcatC(sbuff, UTF8STRC("DSM"));
	}
	if (IO::Path::GetPathType(CSTR("/etc/VERSION")) == IO::Path::PathType::File)
	{
		return Text::StrConcatC(sbuff, UTF8STRC("DSM"));
	}
	if (IO::Path::GetPathType(CSTR("/etc/openwrt_release")) == IO::Path::PathType::File)
	{
		return Text::StrConcatC(sbuff, UTF8STRC("OpenWRT"));
	}
	if (IO::Path::GetPathType(CSTR("/etc/br-version")) == IO::Path::PathType::File)
	{
		return Text::StrConcatC(sbuff, UTF8STRC("Bovine"));
	}
	if (IO::Path::GetPathType(CSTR("/etc/mlinux-version")) == IO::Path::PathType::File)
	{
		return Text::StrConcatC(sbuff, UTF8STRC("mLinux"));
	}
	if (IO::Path::GetPathType(CSTR("/etc/release")) == IO::Path::PathType::File) //Eurotech
	{
		UTF8Char line[512];
		UOSInt i;
		UOSInt j;
		{
			IO::FileStream fs(CSTR("/etc/release"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			Text::UTF8Reader reader(fs);
			line[0] = 0;
			reader.ReadLine(line, 512);
		}
		i = INVALID_INDEX;
		while (true)
		{
			j = Text::StrIndexOfChar(&line[i + 1], ' ');
			if (j != INVALID_INDEX)
			{
				i = i + j + 1;
				if (line[i + 1] >= 0x30 && line[i + 1] <= 0x39 && line[i + 2] == '.')
				{
					line[i] = 0;
					return Text::StrConcat(sbuff, line);
				}
			}
			else
			{
				break;
			}
		}
	}
	if (IO::Path::GetPathType(CSTR("/usr/sbin/ENG/stringlist_ENG.txt")) == IO::Path::PathType::File)
	{
		Text::StringBuilderUTF8 sb;
		UnsafeArrayOpt<UTF8Char> ret = 0;

		IO::FileStream fs(CSTR("/usr/sbin/ENG/stringlist_ENG.txt"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!fs.IsError())
		{
			Text::UTF8Reader reader(fs);
			sb.ClearStr();
			while (reader.ReadLine(sb, 512))
			{
				if (sb.StartsWith(UTF8STRC("##0018")))
				{
					sb.ClearStr();
					reader.ReadLine(sb, 512);
					sb.ClearStr();
					reader.ReadLine(sb, 512);
					sb.ClearStr();
					if (reader.ReadLine(sb, 512))
					{
						ret = Text::StrConcatC(sbuff, sb.ToString(), sb.GetLength());
					}
				}
				sb.ClearStr();
			}
		}
		UnsafeArray<UTF8Char> nnret;
		if (ret.SetTo(nnret))
			return nnret;
	}

	return 0;
}

UnsafeArrayOpt<UTF8Char> IO::OS::GetVersion(UnsafeArray<UTF8Char> sbuff)
{
	if (IO::Path::GetPathType(CSTR("/bin/nvram")) == IO::Path::PathType::File)
	{
		Text::StringBuilderUTF8 sb;
		Manage::Process::ExecuteProcess(CSTR("nvram get firmver"), sb);
		while (sb.EndsWith('\r') || sb.EndsWith('\n'))
		{
			sb.RemoveChars(1);
		}
		if (sb.GetLength() > 0)
		{
			return Text::StrConcatC(sbuff, sb.ToString(), sb.GetLength());
		}

		Manage::Process::ExecuteProcess(CSTR("nvram get FirmwareVersion"), sb);
		while (sb.EndsWith('\r') || sb.EndsWith('\n'))
		{
			sb.RemoveChars(1);
		}
		if (sb.GetLength() > 0)
		{
			return Text::StrConcatC(sbuff, sb.ToString(), sb.GetLength());
		}
	}
	if (IO::Path::GetPathType(CSTR("/etc/debian_version")) == IO::Path::PathType::File)
	{
		UnsafeArrayOpt<UTF8Char> ret = 0;
		IO::FileStream fs(CSTR("/etc/debian_version"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!fs.IsError())
		{
			Text::UTF8Reader reader(fs);
			ret = reader.ReadLine(sbuff, 128);
		}
		return ret;
	}
	if (IO::Path::GetPathType(CSTR("/etc/os-release")) == IO::Path::PathType::File)
	{
		IO::ConfigFile *cfg = IO::UnixConfigFile::Parse(CSTR("/etc/os-release"));
		if (cfg)
		{
			NN<Text::String> s;
			if (cfg->GetValue(CSTR("VERSION")).SetTo(s))
			{
				sbuff = s->ConcatTo(sbuff);
			}
			if (cfg->GetValue(CSTR("BUILD_ID")).SetTo(s))
			{
				*sbuff++ = ' ';
				sbuff = s->ConcatTo(sbuff);
			}
			DEL_CLASS(cfg);
			return sbuff;
		}
	}
	if (IO::Path::GetPathType(CSTR("/etc/openwrt_version")) == IO::Path::PathType::File)
	{
		UnsafeArrayOpt<UTF8Char> ret = 0;
		IO::FileStream fs(CSTR("/etc/openwrt_version"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!fs.IsError())
		{
			Text::UTF8Reader reader(fs);
			ret = reader.ReadLine(sbuff, 128);
		}
		return ret;
	}
	if (IO::Path::GetPathType(CSTR("/sbin/getcfg")) == IO::Path::PathType::File)
	{
		Text::StringBuilderUTF8 sb;
		Manage::Process::ExecuteProcess(CSTR("getcfg system version"), sb);
		if (sb.GetLength() > 0)
		{
			while (sb.EndsWith('\r') || sb.EndsWith('\n'))
			{
				sb.RemoveChars(1);
			}
			return Text::StrConcatC(sbuff, sb.ToString(), sb.GetLength());
		}
	}
	if (IO::Path::GetPathType(CSTR("/etc/VERSION")) == IO::Path::PathType::File)
	{
		IO::ConfigFile *cfg = IO::UnixConfigFile::Parse(CSTR("/etc/VERSION"));
		NN<Text::String> s;
		if (cfg)
		{
			if (cfg->GetValue(CSTR("productversion")).SetTo(s))
			{
				sbuff = s->ConcatTo(sbuff);
			}
			if (cfg->GetValue(CSTR("buildnumber")).SetTo(s))
			{
				sbuff = s->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("-")));
			}
			DEL_CLASS(cfg);
			return sbuff;
		}
	}
	if (IO::Path::GetPathType(CSTR("/etc/release")) == IO::Path::PathType::File) //Eurotech
	{
		UTF8Char line[512];
		UOSInt i;
		UOSInt j;
		IO::FileStream fs(CSTR("/etc/release"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		Text::UTF8Reader reader(fs);
		line[0] = 0;
		reader.ReadLine(line, 512);
		i = INVALID_INDEX;
		while (true)
		{
			j = Text::StrIndexOfChar(&line[i + 1], ' ');
			if (j != INVALID_INDEX)
			{
				i = i + j + 1;
				if (line[i + 1] >= 0x30 && line[i + 1] <= 0x39 && line[i + 2] == '.')
				{
					j = Text::StrIndexOfChar(&line[i + 1], ' ');
					if (j != INVALID_INDEX)
					{
						line[i + 1 + j] = 0;
					}
					return Text::StrConcat(sbuff, &line[i + 1]);
				}
			}
			else
			{
				break;
			}
		}
	}
	if (IO::Path::GetPathType(CSTR("/etc/version.txt")) == IO::Path::PathType::File) //Bovine
	{
		UnsafeArrayOpt<UTF8Char> sptr;
		UnsafeArray<UTF8Char> nnsptr;
		IO::FileStream fs(CSTR("/etc/version.txt"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		Text::UTF8Reader reader(fs);
		sptr = reader.ReadLine(sbuff, 512);

		if (sptr.SetTo(nnsptr) && nnsptr != sbuff)
		{
			return nnsptr;
		}
	}
	if (IO::Path::GetPathType(CSTR("/etc/mlinux-version")) == IO::Path::PathType::File) //mLinux
	{
		UnsafeArrayOpt<UTF8Char> sptr;
		UnsafeArray<UTF8Char> nnsptr;
		IO::FileStream fs(CSTR("/etc/mlinux-version"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		Text::UTF8Reader reader(fs);
		sptr = reader.ReadLine(sbuff, 512);

		if (sptr.SetTo(nnsptr) && Text::StrStartsWithC(sbuff, (UOSInt)(nnsptr - sbuff), UTF8STRC("mLinux ")))
		{
			return Text::StrConcatC(sbuff, &sbuff[7], (UOSInt)(nnsptr - &sbuff[7]));
		}
	}
	return 0;
}
