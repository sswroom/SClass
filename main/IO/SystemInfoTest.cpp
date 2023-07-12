#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/SystemInfo.h"
#include "Manage/CPUInfoDetail.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	IO::SystemInfo *sysInfo;
	Manage::CPUInfoDetail *cpuInfo;
	IO::ConsoleWriter console;
	Double temp;
	UOSInt i;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[256];
	UTF8Char *sptr;

	NEW_CLASS(sysInfo, IO::SystemInfo());
	NEW_CLASS(cpuInfo, Manage::CPUInfoDetail());
	sb.ClearStr();
	sb.AppendC(UTF8STRC("CPU Name = "));
	if ((sptr = cpuInfo->GetCPUName(sbuff)) != 0)
	{
		sb.AppendP(sbuff, sptr);
	}
	else
	{
		sb.AppendC(UTF8STRC("Unknown"));
	}
	console.WriteLineC(sb.ToString(), sb.GetLength());

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Platform Name = "));
	if ((sptr = sysInfo->GetPlatformName(sbuff)) != 0)
	{
		sb.AppendP(sbuff, sptr);
	}
	else
	{
		sb.AppendC(UTF8STRC("Unknown"));
	}
	console.WriteLineC(sb.ToString(), sb.GetLength());

	i = 0;
	while (cpuInfo->GetCPUTemp(i, &temp))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("CPU Temp "));
		sb.AppendUOSInt(i);
		sb.AppendC(UTF8STRC(" = "));
		Text::SBAppendF64(&sb, temp);
		console.WriteLineC(sb.ToString(), sb.GetLength());
		i++;
	}
	if (i == 0)
	{
		console.WriteLineC(UTF8STRC("Error in reading CPU temperature"));
	}
	DEL_CLASS(cpuInfo);
	DEL_CLASS(sysInfo);
	return 0;
}
