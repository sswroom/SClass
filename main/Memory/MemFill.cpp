#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

#define BUFFSIZE 104857600

Int32 MyMain(NN<Core::ProgControl> progCtrl)
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
	sb.AppendC(UTF8STRC("t1 = "));
	sb.AppendDouble(t);
	console.WriteLine(sb.ToCString());

	clk.Start();
	i = 300;
	while (i-- > 0)
		MemFillB(buff2, BUFFSIZE, 0x80);
	t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("t2 = "));
	sb.AppendDouble(t);
	console.WriteLine(sb.ToCString());

	clk.Start();
	i = 30000000;
	while (i-- > 0)
		MemClear(buff, sizeof(buff));
	t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("z1 = "));
	sb.AppendDouble(t);
	console.WriteLine(sb.ToCString());

	clk.Start();
	i = 300;
	while (i-- > 0)
		MemClear(buff2, BUFFSIZE);
	t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("z2 = "));
	sb.AppendDouble(t);
	console.WriteLine(sb.ToCString());

	clk.Start();
	i = 300;
	while (i-- > 0)
		MemClearAC(buff2, BUFFSIZE);
	t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("z3 = "));
	sb.AppendDouble(t);
	console.WriteLine(sb.ToCString());

	clk.Start();
	i = 300;
	while (i-- > 0)
		MemClearANC(buff2, BUFFSIZE);
	t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("z4 = "));
	sb.AppendDouble(t);
	console.WriteLine(sb.ToCString());

	MemFreeA(buff2);
	return 0;
}
