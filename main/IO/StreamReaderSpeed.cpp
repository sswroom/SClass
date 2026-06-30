#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/StreamReader.h"
#include "Manage/HiResClock.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UTF8Char sbuff[1024];
	NN<IO::StreamReader> reader;
	NN<IO::FileStream> fs;
	NN<Manage::HiResClock> clk;
	Double t1;
	NN<IO::ConsoleWriter> console;
	Int32 cnt = 0;
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;

	NEW_CLASSNN(fs, IO::FileStream(CSTR("psc20090526.log"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASSNN(reader, IO::StreamReader(fs));
	NEW_CLASSNN(clk, Manage::HiResClock());

	while (reader->ReadLine(sbuff, 1021).SetTo(sptr))
	{
		sptr2 = reader->GetLastLineBreak(sptr);
		if (*sptr == 0)
		{
			t1 = 0;
		}
		if (sptr == sbuff)
		{
			t1 = 0;
		}
		cnt++;
	}
	t1 = clk->GetTimeDiff();
	NEW_CLASSNN(console, IO::ConsoleWriter());
	sptr = Text::StrDouble(Text::StrConcatC(sbuff, UTF8STRC("Time = ")), t1);
	console->WriteLine(CSTRP(sbuff, sptr));
	sptr = Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("Count = ")), cnt);
	console->WriteLine(CSTRP(sbuff, sptr));
	console.Delete();

	clk.Delete();
	reader.Delete();
	fs.Delete();
	return 0;
}
