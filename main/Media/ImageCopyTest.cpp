#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Media/ImageCopy.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

#define WIDTH 3840
#define HEIGHT 2160
#define LOOPCNT 100

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Media::ImageCopy *imgCopy;
	IO::ConsoleWriter *console;
	Text::StringBuilderUTF8 sb;
	Manage::HiResClock clk;
	UInt8 *imgBuff1;
	UInt8 *imgBuff2;
	OSInt i;
	NEW_CLASS(console, IO::ConsoleWriter());
	NEW_CLASS(imgCopy, Media::ImageCopy());
	imgBuff1 = MemAllocA(UInt8, WIDTH * HEIGHT * 4);
	imgBuff2 = MemAllocA(UInt8, WIDTH * HEIGHT * 4);
	clk.Start();
	i = LOOPCNT;
	while (i-- > 0)
	{
		imgCopy->Copy32(imgBuff1, WIDTH * 4, imgBuff2, WIDTH * 4, WIDTH, HEIGHT);
	}
	Double t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("t = "));
	Text::SBAppendF64(&sb, t);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	MemFreeA(imgBuff1);
	MemFreeA(imgBuff2);
	DEL_CLASS(imgCopy);
	DEL_CLASS(console);
	return 0;
}
