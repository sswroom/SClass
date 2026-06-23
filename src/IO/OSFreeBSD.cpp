#include "Stdafx.h"
#include "IO/OS.h"
#include "Manage/Process.h"
#include "Text/StringBuilderUTF8.h"


UnsafeArrayOpt<UTF8Char> IO::OS::GetDistro(UnsafeArray<UTF8Char> sbuff)
{
	return Text::StrConcatC(sbuff, UTF8STRC("FreeBSD"));
}

UnsafeArrayOpt<UTF8Char> IO::OS::GetVersion(UnsafeArray<UTF8Char> sbuff)
{
	Text::StringBuilderUTF8 sb;
	Manage::Process::ExecuteProcess(CSTR("freebsd-version"), sb);
	while (sb.EndsWith('\r') || sb.EndsWith('\n'))
	{
		sb.RemoveChars(1);
	}
	return Text::StrConcatC(sbuff, sb.ToString(), sb.GetLength());
}
