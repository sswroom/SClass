#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/OS.h"
#include "IO/Path.h"
#include "IO/UnixConfigFile.h"
#include "Manage/Process.h"
#include "Text/MyString.h"
#include "Text/UTF8Reader.h"

UTF8Char *IO::OS::GetDistro(UTF8Char *sbuff)
{
	if (IO::Path::GetPathType(CSTR("/bin/nvram")) == IO::Path::PathType::File)
	{
		Text::StringBuilderUTF8 sb;
		Manage::Process::ExecuteProcess(CSTR("nvram get productid"), &sb);
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
			Text::String *s = cfg->GetValue(CSTR("NAME"));
			if (s)
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
		Text::UTF8Reader *reader;
		UOSInt i;
		UOSInt j;
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream(CSTR("/etc/release"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		line[0] = 0;
		reader->ReadLine(line, 512);
		DEL_CLASS(reader);
		DEL_CLASS(fs);
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
		IO::FileStream *fs;
		Text::UTF8Reader *reader;
		Text::StringBuilderUTF8 sb;
		UTF8Char *ret = 0;

		NEW_CLASS(fs, IO::FileStream(CSTR("/usr/sbin/ENG/stringlist_ENG.txt"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		if (!fs->IsError())
		{
			NEW_CLASS(reader, Text::UTF8Reader(fs));
			sb.ClearStr();
			while (reader->ReadLine(&sb, 512))
			{
				if (sb.StartsWith(UTF8STRC("##0018")))
				{
					sb.ClearStr();
					reader->ReadLine(&sb, 512);
					sb.ClearStr();
					reader->ReadLine(&sb, 512);
					sb.ClearStr();
					if (reader->ReadLine(&sb, 512))
					{
						ret = Text::StrConcatC(sbuff, sb.ToString(), sb.GetLength());
					}
				}
				sb.ClearStr();
			}
			DEL_CLASS(reader);
		}
		DEL_CLASS(fs);
		if (ret)
			return ret;
	}

	return 0;
}

UTF8Char *IO::OS::GetVersion(UTF8Char *sbuff)
{
	if (IO::Path::GetPathType(CSTR("/bin/nvram")) == IO::Path::PathType::File)
	{
		Text::StringBuilderUTF8 sb;
		Manage::Process::ExecuteProcess(CSTR("nvram get firmver"), &sb);
		while (sb.EndsWith('\r') || sb.EndsWith('\n'))
		{
			sb.RemoveChars(1);
		}
		if (sb.GetLength() > 0)
		{
			return Text::StrConcatC(sbuff, sb.ToString(), sb.GetLength());
		}

		Manage::Process::ExecuteProcess(CSTR("nvram get FirmwareVersion"), &sb);
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
		UTF8Char *ret = 0;
		Text::UTF8Reader *reader;
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream(CSTR("/etc/debian_version"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		if (!fs->IsError())
		{
			NEW_CLASS(reader, Text::UTF8Reader(fs));
			ret = reader->ReadLine(sbuff, 128);
			DEL_CLASS(reader);
		}
		DEL_CLASS(fs);
		return ret;
	}
	if (IO::Path::GetPathType(CSTR("/etc/os-release")) == IO::Path::PathType::File)
	{
		IO::ConfigFile *cfg = IO::UnixConfigFile::Parse(CSTR("/etc/os-release"));
		if (cfg)
		{
			Text::String *s = cfg->GetValue(CSTR("VERSION"));
			if (s)
			{
				sbuff = s->ConcatTo(sbuff);
			}
			s = cfg->GetValue(CSTR("BUILD_ID"));
			if (s)
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
		UTF8Char *ret = 0;
		Text::UTF8Reader *reader;
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream(CSTR("/etc/openwrt_version"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		if (!fs->IsError())
		{
			NEW_CLASS(reader, Text::UTF8Reader(fs));
			ret = reader->ReadLine(sbuff, 128);
			DEL_CLASS(reader);
		}
		DEL_CLASS(fs);
		return ret;
	}
	if (IO::Path::GetPathType(CSTR("/sbin/getcfg")) == IO::Path::PathType::File)
	{
		Text::StringBuilderUTF8 sb;
		Manage::Process::ExecuteProcess(CSTR("getcfg system version"), &sb);
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
		Text::String *s;
		if (cfg)
		{
			s = cfg->GetValue(CSTR("productversion"));
			if (s)
			{
				sbuff = s->ConcatTo(sbuff);
			}
			s = cfg->GetValue(CSTR("buildnumber"));
			if (s)
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
		Text::UTF8Reader *reader;
		UOSInt i;
		UOSInt j;
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream(CSTR("/etc/release"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		line[0] = 0;
		reader->ReadLine(line, 512);
		DEL_CLASS(reader);
		DEL_CLASS(fs);
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
		Text::UTF8Reader *reader;
		UTF8Char *sptr;
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream(CSTR("/etc/version.txt"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		sptr = reader->ReadLine(sbuff, 512);
		DEL_CLASS(reader);
		DEL_CLASS(fs);

		if (sptr && sptr != sbuff)
		{
			return sptr;
		}
	}
	if (IO::Path::GetPathType(CSTR("/etc/mlinux-version")) == IO::Path::PathType::File) //mLinux
	{
		Text::UTF8Reader *reader;
		UTF8Char *sptr;
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream(CSTR("/etc/mlinux-version"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		sptr = reader->ReadLine(sbuff, 512);
		DEL_CLASS(reader);
		DEL_CLASS(fs);

		if (sptr && Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("mLinux ")))
		{
			return Text::StrConcatC(sbuff, &sbuff[7], (UOSInt)(sptr - &sbuff[7]));
		}
	}
	return 0;
}
