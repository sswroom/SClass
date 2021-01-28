#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/SystemInfo.h"
#include "Manage/CPUInfoDetail.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::SystemInfo *sysInfo;
	Manage::CPUInfoDetail *cpuInfo;
	IO::ConsoleWriter console;
	Double temp;
	OSInt i;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[256];

	NEW_CLASS(sysInfo, IO::SystemInfo());
	NEW_CLASS(cpuInfo, Manage::CPUInfoDetail());
	sb.ClearStr();
	sb.Append((const UTF8Char*)"CPU Name = ");
	if (cpuInfo->GetCPUName(sbuff))
	{
		sb.Append(sbuff);
	}
	else
	{
		sb.Append((const UTF8Char*)"Unknown");
	}
	console.WriteLine(sb.ToString());

	sb.ClearStr();
	sb.Append((const UTF8Char*)"Platform Name = ");
	if (sysInfo->GetPlatformName(sbuff))
	{
		sb.Append(sbuff);
	}
	else
	{
		sb.Append((const UTF8Char*)"Unknown");
	}
	console.WriteLine(sb.ToString());

	i = 0;
	while (cpuInfo->GetCPUTemp(i, &temp))
	{
		sb.ClearStr();
		sb.Append((const UTF8Char*)"CPU Temp ");
		sb.AppendOSInt(i);
		sb.Append((const UTF8Char*)" = ");
		Text::SBAppendF64(&sb, temp);
		console.WriteLine(sb.ToString());
		i++;
	}
	if (i == 0)
	{
		console.WriteLine((const UTF8Char*)"Error in reading CPU temperature");
	}
	DEL_CLASS(cpuInfo);
	DEL_CLASS(sysInfo);
	return 0;
}
