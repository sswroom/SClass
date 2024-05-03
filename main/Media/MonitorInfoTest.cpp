#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Media/MonitorInfo.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	Media::MonitorInfo *info;
	IO::ConsoleWriter console;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(info, Media::MonitorInfo(0));
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Name = "));
	sb.Append(info->GetName());
	console.WriteLine(sb.ToCString());
	if (info->GetDesc())
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Desc = "));
		sb.Append(info->GetDesc());
		console.WriteLine(sb.ToCString());
	}
	if (info->GetMonitorID())
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("MonitorID = "));
		sb.Append(info->GetMonitorID());
		console.WriteLine(sb.ToCString());
	}
	DEL_CLASS(info);
	return 0;
}
