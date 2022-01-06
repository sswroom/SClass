#include "Stdafx.h"
#include "IO/OS.h"
#include "Manage/Process.h"
#include "Text/StringBuilderUTF8.h"


UTF8Char *IO::OS::GetDistro(UTF8Char *sbuff)
{
	return Text::StrConcatC(sbuff, UTF8STRC("FreeBSD"));
}

UTF8Char *IO::OS::GetVersion(UTF8Char *sbuff)
{
	Text::StringBuilderUTF8 sb;
	Manage::Process::ExecuteProcess((const UTF8Char*)"freebsd-version", &sb);
	while (sb.EndsWith('\r') || sb.EndsWith('\n'))
	{
		sb.RemoveChars(1);
	}
	return Text::StrConcatC(sbuff, sb.ToString(), sb.GetLength());
}
