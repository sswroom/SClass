#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Media/MonitorInfo.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Media::MonitorInfo *info;
	IO::ConsoleWriter console;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(info, Media::MonitorInfo(0));
	if (info->GetName())
	{
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Name = ");
		sb.Append(info->GetName());
		console.WriteLine(sb.ToString());
	}
	if (info->GetDesc())
	{
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Desc = ");
		sb.Append(info->GetDesc());
		console.WriteLine(sb.ToString());
	}
	if (info->GetMonitorID())
	{
		sb.ClearStr();
		sb.Append((const UTF8Char*)"MonitorID = ");
		sb.Append(info->GetMonitorID());
		console.WriteLine(sb.ToString());
	}
	DEL_CLASS(info);
	return 0;
}
