#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Media/MonitorInfo.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Media::MonitorInfo *info;
	IO::ConsoleWriter console;
	Text::StringBuilderUTF8 sb;
	NN<Text::String> s;
	NEW_CLASS(info, Media::MonitorInfo(nullptr));
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Name = "));
	sb.Append(info->GetName());
	console.WriteLine(sb.ToCString());
	if (info->GetDesc().SetTo(s))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Desc = "));
		sb.Append(s);
		console.WriteLine(sb.ToCString());
	}
	if (info->GetMonitorID().SetTo(s))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("MonitorID = "));
		sb.Append(s);
		console.WriteLine(sb.ToCString());
	}
	DEL_CLASS(info);
	return 0;
}
