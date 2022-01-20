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
	if (IO::Path::GetPathType((const UTF8Char*)"/bin/nvram") == IO::Path::PathType::File)
	{
		Text::StringBuilderUTF8 sb;
		Manage::Process::ExecuteProcess(UTF8STRC("nvram get productid"), &sb);
		while (sb.EndsWith('\r') || sb.EndsWith('\n'))
		{
			sb.RemoveChars(1);
		}
		if (sb.GetLength() > 0)
		{
			return Text::StrConcatC(sbuff, sb.ToString(), sb.GetLength());
		}
	}
	if (IO::Path::GetPathType((const UTF8Char*)"/etc/os-release") == IO::Path::PathType::File)
	{
		IO::ConfigFile *cfg = IO::UnixConfigFile::Parse((const UTF8Char*)"/etc/os-release");
		if (cfg)
		{
			Text::String *s = cfg->GetValue(UTF8STRC("NAME"));
			if (s)
			{
				sbuff = s->ConcatTo(sbuff);
			}
			DEL_CLASS(cfg);
			return sbuff;
		}
	}
	if (IO::Path::GetPathType((const UTF8Char*)"/sbin/getcfg") == IO::Path::PathType::File)
	{
		return Text::StrConcatC(sbuff, UTF8STRC("DSM"));
	}
	if (IO::Path::GetPathType((const UTF8Char*)"/etc/VERSION") == IO::Path::PathType::File)
	{
		return Text::StrConcatC(sbuff, UTF8STRC("DSM"));
	}
	if (IO::Path::GetPathType((const UTF8Char*)"/etc/openwrt_release") == IO::Path::PathType::File)
	{
		return Text::StrConcatC(sbuff, UTF8STRC("OpenWRT"));
	}
	if (IO::Path::GetPathType((const UTF8Char*)"/etc/br-version") == IO::Path::PathType::File)
	{
		return Text::StrConcatC(sbuff, UTF8STRC("Bovine"));
	}
	if (IO::Path::GetPathType((const UTF8Char*)"/etc/mlinux-version") == IO::Path::PathType::File)
	{
		return Text::StrConcatC(sbuff, UTF8STRC("mLinux"));
	}
	if (IO::Path::GetPathType((const UTF8Char*)"/etc/release") == IO::Path::PathType::File) //Eurotech
	{
		UTF8Char line[512];
		Text::UTF8Reader *reader;
		UOSInt i;
		UOSInt j;
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/etc/release", IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
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
	if (IO::Path::GetPathType((const UTF8Char*)"/usr/sbin/ENG/stringlist_ENG.txt") == IO::Path::PathType::File)
	{
		IO::FileStream *fs;
		Text::UTF8Reader *reader;
		Text::StringBuilderUTF8 sb;
		UTF8Char *ret = 0;

		NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/usr/sbin/ENG/stringlist_ENG.txt", IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		if (!fs->IsError())
		{
			NEW_CLASS(reader, Text::UTF8Reader(fs));
			sb.ClearStr();
			while (reader->ReadLine(&sb, 512))
			{
				if (sb.StartsWith((const UTF8Char*)"##0018"))
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
	if (IO::Path::GetPathType((const UTF8Char*)"/bin/nvram") == IO::Path::PathType::File)
	{
		Text::StringBuilderUTF8 sb;
		Manage::Process::ExecuteProcess(UTF8STRC("nvram get firmver"), &sb);
		while (sb.EndsWith('\r') || sb.EndsWith('\n'))
		{
			sb.RemoveChars(1);
		}
		if (sb.GetLength() > 0)
		{
			return Text::StrConcatC(sbuff, sb.ToString(), sb.GetLength());
		}

		Manage::Process::ExecuteProcess(UTF8STRC("nvram get FirmwareVersion"), &sb);
		while (sb.EndsWith('\r') || sb.EndsWith('\n'))
		{
			sb.RemoveChars(1);
		}
		if (sb.GetLength() > 0)
		{
			return Text::StrConcatC(sbuff, sb.ToString(), sb.GetLength());
		}
	}
	if (IO::Path::GetPathType((const UTF8Char*)"/etc/debian_version") == IO::Path::PathType::File)
	{
		UTF8Char *ret = 0;
		Text::UTF8Reader *reader;
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/etc/debian_version", IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		if (!fs->IsError())
		{
			NEW_CLASS(reader, Text::UTF8Reader(fs));
			ret = reader->ReadLine(sbuff, 128);
			DEL_CLASS(reader);
		}
		DEL_CLASS(fs);
		return ret;
	}
	if (IO::Path::GetPathType((const UTF8Char*)"/etc/os-release") == IO::Path::PathType::File)
	{
		IO::ConfigFile *cfg = IO::UnixConfigFile::Parse((const UTF8Char*)"/etc/os-release");
		if (cfg)
		{
			Text::String *s = cfg->GetValue(UTF8STRC("VERSION"));
			if (s)
			{
				sbuff = s->ConcatTo(sbuff);
			}
			s = cfg->GetValue(UTF8STRC("BUILD_ID"));
			if (s)
			{
				*sbuff++ = ' ';
				sbuff = s->ConcatTo(sbuff);
			}
			DEL_CLASS(cfg);
			return sbuff;
		}
	}
	if (IO::Path::GetPathType((const UTF8Char*)"/etc/openwrt_version") == IO::Path::PathType::File)
	{
		UTF8Char *ret = 0;
		Text::UTF8Reader *reader;
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/etc/openwrt_version", IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		if (!fs->IsError())
		{
			NEW_CLASS(reader, Text::UTF8Reader(fs));
			ret = reader->ReadLine(sbuff, 128);
			DEL_CLASS(reader);
		}
		DEL_CLASS(fs);
		return ret;
	}
	if (IO::Path::GetPathType((const UTF8Char*)"/sbin/getcfg") == IO::Path::PathType::File)
	{
		Text::StringBuilderUTF8 sb;
		Manage::Process::ExecuteProcess(UTF8STRC("getcfg system version"), &sb);
		if (sb.GetLength() > 0)
		{
			while (sb.EndsWith('\r') || sb.EndsWith('\n'))
			{
				sb.RemoveChars(1);
			}
			return Text::StrConcatC(sbuff, sb.ToString(), sb.GetLength());
		}
	}
	if (IO::Path::GetPathType((const UTF8Char*)"/etc/VERSION") == IO::Path::PathType::File)
	{
		IO::ConfigFile *cfg = IO::UnixConfigFile::Parse((const UTF8Char*)"/etc/VERSION");
		Text::String *s;
		if (cfg)
		{
			s = cfg->GetValue(UTF8STRC("productversion"));
			if (s)
			{
				sbuff = s->ConcatTo(sbuff);
			}
			s = cfg->GetValue(UTF8STRC("buildnumber"));
			if (s)
			{
				sbuff = s->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("-")));
			}
			DEL_CLASS(cfg);
			return sbuff;
		}
	}
	if (IO::Path::GetPathType((const UTF8Char*)"/etc/release") == IO::Path::PathType::File) //Eurotech
	{
		UTF8Char line[512];
		Text::UTF8Reader *reader;
		UOSInt i;
		UOSInt j;
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/etc/release", IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
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
	if (IO::Path::GetPathType((const UTF8Char*)"/etc/version.txt") == IO::Path::PathType::File) //Bovine
	{
		Text::UTF8Reader *reader;
		UTF8Char *sptr;
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/etc/version.txt", IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		sptr = reader->ReadLine(sbuff, 512);
		DEL_CLASS(reader);
		DEL_CLASS(fs);

		if (sptr && sptr != sbuff)
		{
			return sptr;
		}
	}
	if (IO::Path::GetPathType((const UTF8Char*)"/etc/mlinux-version") == IO::Path::PathType::File) //mLinux
	{
		Text::UTF8Reader *reader;
		UTF8Char *sptr;
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/etc/mlinux-version", IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		sptr = reader->ReadLine(sbuff, 512);
		DEL_CLASS(reader);
		DEL_CLASS(fs);

		if (sptr && Text::StrStartsWith(sbuff, (const UTF8Char*)"mLinux "))
		{
			return Text::StrConcat(sbuff, &sbuff[7]);
		}
	}
	return 0;
}
