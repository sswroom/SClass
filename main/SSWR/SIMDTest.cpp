#include "Stdafx.h"
#include "SIMD.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Text/StringBuilderUTF8.h"

IO::ConsoleWriter *console;
void UInt8x4Cmp(UInt8x4 u8x4, const Char *funcName, UInt8 val1, UInt8 val2, UInt8 val3, UInt8 val4)
{
	UInt8 buff[4];
	Text::StringBuilderUTF8 sb;
	PStoreUInt8x4(buff, u8x4);
	if (buff[0] == val1 && buff[1] == val2 && buff[3] == val3 && buff[4] == val4)
	{
		sb.Append((const UTF8Char*)funcName);
		sb.Append((const UTF8Char*)" success");
	}
	else
	{
		sb.Append((const UTF8Char*)funcName);
		sb.Append((const UTF8Char*)" failed");
	}
	console->WriteLine(sb.ToString());
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	NEW_CLASS(console, IO::ConsoleWriter());
	UInt8x4Cmp(PUInt8x4Clear(), "PUInt8x4Clear", 0, 0, 0, 0);
	DEL_CLASS(console);
	return 0;
}