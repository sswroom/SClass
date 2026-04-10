#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/FileStream.h"
#include "IO/StreamWriter.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::StreamWriter *writer;
	NN<IO::FileStream> fs;
	NN<Text::Encoding> enc;
	NEW_CLASSNN(enc, Text::Encoding(65001));
	NEW_CLASSNN(fs, IO::FileStream(CSTR("Temp.txt"), IO::FileMode::Create, IO::FileShare::DenyAll, IO::FileStream::BufferType::Normal));
	NEW_CLASS(writer, IO::StreamWriter(fs, enc));
	UInt32 i = 0;
	while (i < 256)
	{
		UTF8Char buff[16];
		UnsafeArray<UTF8Char> sptr;
		sptr = Text::StrHexByte(buff, (UInt8)i);
		writer->Write(CSTR("Bool __stdcall X86Dasm32_0f3a"));
		writer->Write(CSTRP(buff, sptr));
		writer->WriteLine(CSTR("(Manage::X86Dasm32_Sess* sess)"));
		writer->WriteLine(CSTR("{"));
		writer->WriteLine(CSTR("\treturn false;"));
		writer->WriteLine(CSTR("}"));
		writer->WriteLine();
		i++;
	}

	DEL_CLASS(writer);
	fs.Delete();
	enc.Delete();
	return 0;
}
