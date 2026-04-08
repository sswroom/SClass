#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/FileStream.h"
#include "IO/StreamReader.h"

void TestFile(Text::CStringNN fileName)
{
	UTF8Char lineBuff[1024];

	IO::FileStream fstm(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	IO::StreamReader rdr(fstm);
	while (rdr.ReadLine(lineBuff, 1023).NotNull())
	{
	}
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	TestFile(CSTR("MapLayer_eng.txt"));
//	TestFile(CSTR("MapLayer_tch.txt"));
//	TestFile(CSTR("MapLayer_sch.txt"));
	return 0;
}
