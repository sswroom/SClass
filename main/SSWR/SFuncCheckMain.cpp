#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/ByteTool.h"
#include "IO/ConsoleWriter.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter *console;
	NEW_CLASS(console, IO::ConsoleWriter());

	console->WriteLine((const UTF8Char*)"Default Color");
	console->SetTextColor(IO::ConsoleWriter::CC_RED, IO::ConsoleWriter::CC_BLACK);
	console->WriteLine((const UTF8Char*)"Red Text");
	console->ResetTextColor();
	console->WriteLine();

	UInt8 buff[64];
	console->WriteLine((const UTF8Char*)"ByteTool:");
	WriteInt16(&buff[1], 0x1234);
	if (buff[1] == 0x34 && buff[2] == 0x12)
	{
		console->WriteLine((const UTF8Char*)"WriteInt16: Valid");
	}
	else
	{
		console->SetTextColor(IO::ConsoleWriter::CC_RED, IO::ConsoleWriter::CC_BLACK);
		console->WriteLine((const UTF8Char*)"WriteInt16: Invalid");
		console->ResetTextColor();
	}
	WriteMInt16(&buff[1], 0x1234);
	if (buff[1] == 0x12 && buff[2] == 0x34)
	{
		console->WriteLine((const UTF8Char*)"WriteMInt16: Valid");
	}
	else
	{
		console->SetTextColor(IO::ConsoleWriter::CC_RED, IO::ConsoleWriter::CC_BLACK);
		console->WriteLine((const UTF8Char*)"WriteMInt16: Invalid");
		console->ResetTextColor();
	}

	WriteInt24(&buff[1], 0x123456);
	if (buff[1] == 0x56 && buff[2] == 0x34 && buff[3] == 0x12)
	{
		console->WriteLine((const UTF8Char*)"WriteInt24: Valid");
	}
	else
	{
		console->SetTextColor(IO::ConsoleWriter::CC_RED, IO::ConsoleWriter::CC_BLACK);
		console->WriteLine((const UTF8Char*)"WriteInt24: Invalid");
		console->ResetTextColor();
	}
	WriteMInt24(&buff[1], 0x123456);
	if (buff[1] == 0x12 && buff[2] == 0x34 && buff[3] == 0x56)
	{
		console->WriteLine((const UTF8Char*)"WriteMInt24: Valid");
	}
	else
	{
		console->SetTextColor(IO::ConsoleWriter::CC_RED, IO::ConsoleWriter::CC_BLACK);
		console->WriteLine((const UTF8Char*)"WriteMInt24: Invalid");
		console->ResetTextColor();
	}

	WriteInt32(&buff[1], 0x12345678);
	if (buff[1] == 0x78 && buff[2] == 0x56 && buff[3] == 0x34 && buff[4] == 0x12)
	{
		console->WriteLine((const UTF8Char*)"WriteInt32: Valid");
	}
	else
	{
		console->SetTextColor(IO::ConsoleWriter::CC_RED, IO::ConsoleWriter::CC_BLACK);
		console->WriteLine((const UTF8Char*)"WriteInt32: Invalid");
		console->ResetTextColor();
	}
	WriteMInt32(&buff[1], 0x12345678);
	if (buff[1] == 0x12 && buff[2] == 0x34 && buff[3] == 0x56 && buff[4] == 0x78)
	{
		console->WriteLine((const UTF8Char*)"WriteMInt32: Valid");
	}
	else
	{
		console->SetTextColor(IO::ConsoleWriter::CC_RED, IO::ConsoleWriter::CC_BLACK);
		console->WriteLine((const UTF8Char*)"WriteMInt32: Invalid");
		console->ResetTextColor();
	}

#if defined(HAS_INT64)
	WriteInt64(&buff[1], 0x1234567890123456LL);
	if (buff[1] == 0x56 && buff[2] == 0x34 && buff[3] == 0x12 && buff[4] == 0x90 && buff[5] == 0x78 && buff[6] == 0x56 && buff[7] == 0x34 && buff[8] == 0x12)
	{
		console->WriteLine((const UTF8Char*)"WriteInt64: Valid");
	}
	else
	{
		console->SetTextColor(IO::ConsoleWriter::CC_RED, IO::ConsoleWriter::CC_BLACK);
		console->WriteLine((const UTF8Char*)"WriteInt64: Invalid");
		console->ResetTextColor();
	}
	WriteMInt64(&buff[1], 0x1234567890123456LL);
	if (buff[1] == 0x12 && buff[2] == 0x34 && buff[3] == 0x56 && buff[4] == 0x78 && buff[5] == 0x90 && buff[6] == 0x12 && buff[7] == 0x34 && buff[8] == 0x56)
	{
		console->WriteLine((const UTF8Char*)"WriteMInt64: Valid");
	}
	else
	{
		console->SetTextColor(IO::ConsoleWriter::CC_RED, IO::ConsoleWriter::CC_BLACK);
		console->WriteLine((const UTF8Char*)"WriteMInt64: Invalid");
		console->ResetTextColor();
	}
#endif

	buff[1] = 0x12;
	buff[2] = 0x34;
	buff[3] = 0x56;
	buff[4] = 0x78;
	buff[5] = 0x90;
	buff[6] = 0x12;
	buff[7] = 0x34;
	buff[8] = 0x56;
	if (ReadInt16(&buff[1]) == 0x3412)
	{
		console->WriteLine((const UTF8Char*)"ReadInt16: Valid");
	}
	else
	{
		console->SetTextColor(IO::ConsoleWriter::CC_RED, IO::ConsoleWriter::CC_BLACK);
		console->WriteLine((const UTF8Char*)"ReadInt16: Invalid");
		console->ResetTextColor();
	}
	if (ReadMInt16(&buff[1]) == 0x1234)
	{
		console->WriteLine((const UTF8Char*)"ReadMInt16: Valid");
	}
	else
	{
		console->SetTextColor(IO::ConsoleWriter::CC_RED, IO::ConsoleWriter::CC_BLACK);
		console->WriteLine((const UTF8Char*)"ReadMInt16: Invalid");
		console->ResetTextColor();
	}
	if (ReadInt24(&buff[1]) == 0x563412)
	{
		console->WriteLine((const UTF8Char*)"ReadInt24: Valid");
	}
	else
	{
		console->SetTextColor(IO::ConsoleWriter::CC_RED, IO::ConsoleWriter::CC_BLACK);
		console->WriteLine((const UTF8Char*)"ReadInt24: Invalid");
		console->ResetTextColor();
	}
	if (ReadMInt24(&buff[1]) == 0x123456)
	{
		console->WriteLine((const UTF8Char*)"ReadMInt24: Valid");
	}
	else
	{
		console->SetTextColor(IO::ConsoleWriter::CC_RED, IO::ConsoleWriter::CC_BLACK);
		console->WriteLine((const UTF8Char*)"ReadMInt24: Invalid");
		console->ResetTextColor();
	}
	if (ReadInt32(&buff[1]) == 0x78563412)
	{
		console->WriteLine((const UTF8Char*)"ReadInt32: Valid");
	}
	else
	{
		console->SetTextColor(IO::ConsoleWriter::CC_RED, IO::ConsoleWriter::CC_BLACK);
		console->WriteLine((const UTF8Char*)"ReadInt32: Invalid");
		console->ResetTextColor();
	}
	if (ReadMInt32(&buff[1]) == 0x12345678)
	{
		console->WriteLine((const UTF8Char*)"ReadMInt32: Valid");
	}
	else
	{
		console->SetTextColor(IO::ConsoleWriter::CC_RED, IO::ConsoleWriter::CC_BLACK);
		console->WriteLine((const UTF8Char*)"ReadMInt32: Invalid");
		console->ResetTextColor();
	}
#if defined(HAS_INT64)
	if (ReadInt64(&buff[1]) == 0x5634129078563412LL)
	{
		console->WriteLine((const UTF8Char*)"ReadInt64: Valid");
	}
	else
	{
		console->SetTextColor(IO::ConsoleWriter::CC_RED, IO::ConsoleWriter::CC_BLACK);
		console->WriteLine((const UTF8Char*)"ReadInt64: Invalid");
		console->ResetTextColor();
	}
	if (ReadMInt64(&buff[1]) == 0x1234567890123456LL)
	{
		console->WriteLine((const UTF8Char*)"ReadMInt64: Valid");
	}
	else
	{
		console->SetTextColor(IO::ConsoleWriter::CC_RED, IO::ConsoleWriter::CC_BLACK);
		console->WriteLine((const UTF8Char*)"ReadMInt64: Invalid");
		console->ResetTextColor();
	}
#endif

	DEL_CLASS(console);
	return 0;
}
