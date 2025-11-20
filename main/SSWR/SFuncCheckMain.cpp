#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Core/ByteTool_C.h"
#include "IO/ConsoleWriter.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::ConsoleWriter *console;
	NEW_CLASS(console, IO::ConsoleWriter());

	console->WriteLine(CSTR("Default Color"));
	console->SetTextColor(Text::StandardColor::Red);
	console->WriteLine(CSTR("Red Text"));
	console->ResetTextColor();
	console->WriteLine();

	UInt8 buff[64];
	console->WriteLine(CSTR("ByteTool:"));
	WriteInt16(&buff[1], 0x1234);
	if (buff[1] == 0x34 && buff[2] == 0x12)
	{
		console->WriteLine(CSTR("WriteInt16: Valid"));
	}
	else
	{
		console->SetTextColor(Text::StandardColor::Red);
		console->WriteLine(CSTR("WriteInt16: Invalid"));
		console->ResetTextColor();
	}
	WriteMInt16(&buff[1], 0x1234);
	if (buff[1] == 0x12 && buff[2] == 0x34)
	{
		console->WriteLine(CSTR("WriteMInt16: Valid"));
	}
	else
	{
		console->SetTextColor(Text::StandardColor::Red);
		console->WriteLine(CSTR("WriteMInt16: Invalid"));
		console->ResetTextColor();
	}

	WriteInt24(&buff[1], 0x123456);
	if (buff[1] == 0x56 && buff[2] == 0x34 && buff[3] == 0x12)
	{
		console->WriteLine(CSTR("WriteInt24: Valid"));
	}
	else
	{
		console->SetTextColor(Text::StandardColor::Red);
		console->WriteLine(CSTR("WriteInt24: Invalid"));
		console->ResetTextColor();
	}
	WriteMInt24(&buff[1], 0x123456);
	if (buff[1] == 0x12 && buff[2] == 0x34 && buff[3] == 0x56)
	{
		console->WriteLine(CSTR("WriteMInt24: Valid"));
	}
	else
	{
		console->SetTextColor(Text::StandardColor::Red);
		console->WriteLine(CSTR("WriteMInt24: Invalid"));
		console->ResetTextColor();
	}

	WriteInt32(&buff[1], 0x12345678);
	if (buff[1] == 0x78 && buff[2] == 0x56 && buff[3] == 0x34 && buff[4] == 0x12)
	{
		console->WriteLine(CSTR("WriteInt32: Valid"));
	}
	else
	{
		console->SetTextColor(Text::StandardColor::Red);
		console->WriteLine(CSTR("WriteInt32: Invalid"));
		console->ResetTextColor();
	}
	WriteMInt32(&buff[1], 0x12345678);
	if (buff[1] == 0x12 && buff[2] == 0x34 && buff[3] == 0x56 && buff[4] == 0x78)
	{
		console->WriteLine(CSTR("WriteMInt32: Valid"));
	}
	else
	{
		console->SetTextColor(Text::StandardColor::Red);
		console->WriteLine(CSTR("WriteMInt32: Invalid"));
		console->ResetTextColor();
	}

#if defined(HAS_INT64)
	WriteInt64(&buff[1], 0x1234567890123456LL);
	if (buff[1] == 0x56 && buff[2] == 0x34 && buff[3] == 0x12 && buff[4] == 0x90 && buff[5] == 0x78 && buff[6] == 0x56 && buff[7] == 0x34 && buff[8] == 0x12)
	{
		console->WriteLine(CSTR("WriteInt64: Valid"));
	}
	else
	{
		console->SetTextColor(Text::StandardColor::Red);
		console->WriteLine(CSTR("WriteInt64: Invalid"));
		console->ResetTextColor();
	}
	WriteMInt64(&buff[1], 0x1234567890123456LL);
	if (buff[1] == 0x12 && buff[2] == 0x34 && buff[3] == 0x56 && buff[4] == 0x78 && buff[5] == 0x90 && buff[6] == 0x12 && buff[7] == 0x34 && buff[8] == 0x56)
	{
		console->WriteLine(CSTR("WriteMInt64: Valid"));
	}
	else
	{
		console->SetTextColor(Text::StandardColor::Red);
		console->WriteLine(CSTR("WriteMInt64: Invalid"));
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
		console->WriteLine(CSTR("ReadInt16: Valid"));
	}
	else
	{
		console->SetTextColor(Text::StandardColor::Red);
		console->WriteLine(CSTR("ReadInt16: Invalid"));
		console->ResetTextColor();
	}
	if (ReadMInt16(&buff[1]) == 0x1234)
	{
		console->WriteLine(CSTR("ReadMInt16: Valid"));
	}
	else
	{
		console->SetTextColor(Text::StandardColor::Red);
		console->WriteLine(CSTR("ReadMInt16: Invalid"));
		console->ResetTextColor();
	}
	if (ReadInt24(&buff[1]) == 0x563412)
	{
		console->WriteLine(CSTR("ReadInt24: Valid"));
	}
	else
	{
		console->SetTextColor(Text::StandardColor::Red);
		console->WriteLine(CSTR("ReadInt24: Invalid"));
		console->ResetTextColor();
	}
	if (ReadMInt24(&buff[1]) == 0x123456)
	{
		console->WriteLine(CSTR("ReadMInt24: Valid"));
	}
	else
	{
		console->SetTextColor(Text::StandardColor::Red);
		console->WriteLine(CSTR("ReadMInt24: Invalid"));
		console->ResetTextColor();
	}
	if (ReadInt32(&buff[1]) == 0x78563412)
	{
		console->WriteLine(CSTR("ReadInt32: Valid"));
	}
	else
	{
		console->SetTextColor(Text::StandardColor::Red);
		console->WriteLine(CSTR("ReadInt32: Invalid"));
		console->ResetTextColor();
	}
	if (ReadMInt32(&buff[1]) == 0x12345678)
	{
		console->WriteLine(CSTR("ReadMInt32: Valid"));
	}
	else
	{
		console->SetTextColor(Text::StandardColor::Red);
		console->WriteLine(CSTR("ReadMInt32: Invalid"));
		console->ResetTextColor();
	}
#if defined(HAS_INT64)
	if (ReadInt64(&buff[1]) == 0x5634129078563412LL)
	{
		console->WriteLine(CSTR("ReadInt64: Valid"));
	}
	else
	{
		console->SetTextColor(Text::StandardColor::Red);
		console->WriteLine(CSTR("ReadInt64: Invalid"));
		console->ResetTextColor();
	}
	if (ReadMInt64(&buff[1]) == 0x1234567890123456LL)
	{
		console->WriteLine(CSTR("ReadMInt64: Valid"));
	}
	else
	{
		console->SetTextColor(Text::StandardColor::Red);
		console->WriteLine(CSTR("ReadMInt64: Invalid"));
		console->ResetTextColor();
	}
#endif

	DEL_CLASS(console);
	return 0;
}
