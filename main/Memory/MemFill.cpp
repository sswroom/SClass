#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

#define BUFFSIZE 104857600

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UInt8 buff[1021];
	UInt8 *buff2;
	Double t;
	OSInt i;
	Text::StringBuilderUTF8 sb;
	IO::ConsoleWriter console;
	Manage::HiResClock clk;
	buff2 = MemAllocA(UInt8, BUFFSIZE);

	clk.Start();
	i = 30000000;
	while (i-- > 0)
		MemFillB(buff, sizeof(buff), 0x80);
	t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.Append((const UTF8Char*)"t1 = ");
	Text::SBAppendF64(&sb, t);
	console.WriteLine(sb.ToString());

	clk.Start();
	i = 300;
	while (i-- > 0)
		MemFillB(buff2, BUFFSIZE, 0x80);
	t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.Append((const UTF8Char*)"t2 = ");
	Text::SBAppendF64(&sb, t);
	console.WriteLine(sb.ToString());

	clk.Start();
	i = 30000000;
	while (i-- > 0)
		MemClear(buff, sizeof(buff));
	t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.Append((const UTF8Char*)"z1 = ");
	Text::SBAppendF64(&sb, t);
	console.WriteLine(sb.ToString());

	clk.Start();
	i = 300;
	while (i-- > 0)
		MemClear(buff2, BUFFSIZE);
	t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.Append((const UTF8Char*)"z2 = ");
	Text::SBAppendF64(&sb, t);
	console.WriteLine(sb.ToString());

	clk.Start();
	i = 300;
	while (i-- > 0)
		MemClearAC(buff2, BUFFSIZE);
	t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.Append((const UTF8Char*)"z3 = ");
	Text::SBAppendF64(&sb, t);
	console.WriteLine(sb.ToString());

	clk.Start();
	i = 300;
	while (i-- > 0)
		MemClearANC(buff2, BUFFSIZE);
	t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.Append((const UTF8Char*)"z4 = ");
	Text::SBAppendF64(&sb, t);
	console.WriteLine(sb.ToString());

	MemFreeA(buff2);
	return 0;
}
