#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/LogZipper.h"
#include "IO/Path.h"
#include "Manage/HiResClock.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	UIntOS cmdCnt;
	UnsafeArray<UnsafeArray<UTF8Char>> cmdLines =	progCtrl->GetCommandLines(progCtrl, cmdCnt);
	if (cmdCnt != 2)
	{
		console.WriteLine(CSTR("Usage: LogZipper <LogDir>"));
		return 1;
	}
	Text::CStringNN logDir = Text::CStringNN::FromPtr(cmdLines[1]);
	if (IO::Path::GetPathType(logDir) != IO::Path::PathType::Directory)
	{
		console.WriteLine(CSTR("Invalid Log Dir"));
		return 1;
	}
	Manage::HiResClock clk;
	Bool succ = IO::LogZipper::ZipLogs(logDir);
	Double t = clk.GetTimeDiff();
	Text::StringBuilderUTF8 sb;
	if (succ)
	{
		sb.Append(CSTR("Success, Time taken: "));
		sb.AppendDouble(t);
		sb.Append(CSTR(" seconds"));
		console.WriteLine(sb.ToCString());
		return 0;
	}
	else
	{
		console.WriteLine(CSTR("Failed"));
		return 2;
	}
}