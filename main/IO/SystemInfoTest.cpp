#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/SystemInfo.h"
#include "Manage/CPUInfoDetail.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::SystemInfo *sysInfo;
	Manage::CPUInfoDetail *cpuInfo;
	IO::ConsoleWriter console;
	Double temp;
	UOSInt i;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;

	NEW_CLASS(sysInfo, IO::SystemInfo());
	NEW_CLASS(cpuInfo, Manage::CPUInfoDetail());
	sb.ClearStr();
	sb.AppendC(UTF8STRC("CPU Name = "));
	if (cpuInfo->GetCPUName(sbuff).SetTo(sptr))
	{
		sb.AppendP(sbuff, sptr);
	}
	else
	{
		sb.AppendC(UTF8STRC("Unknown"));
	}
	console.WriteLine(sb.ToCString());

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Platform Name = "));
	if (sysInfo->GetPlatformName(sbuff).SetTo(sptr))
	{
		sb.AppendP(sbuff, sptr);
	}
	else
	{
		sb.AppendC(UTF8STRC("Unknown"));
	}
	console.WriteLine(sb.ToCString());

	i = 0;
	while (cpuInfo->GetCPUTemp(i, temp))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("CPU Temp "));
		sb.AppendUOSInt(i);
		sb.AppendC(UTF8STRC(" = "));
		sb.AppendDouble(temp);
		console.WriteLine(sb.ToCString());
		i++;
	}
	if (i == 0)
	{
		console.WriteLine(CSTR("Error in reading CPU temperature"));
	}
	DEL_CLASS(cpuInfo);
	DEL_CLASS(sysInfo);
	return 0;
}
