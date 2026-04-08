#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Hash/CRC32.h"
#include "IO/ActiveStreamReader.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "Manage/HiResClock.h"
#include "Text/MyString.h"

Crypto::Hash::HashAlgorithm *hash;
IO::ConsoleWriter *console;
UInt64 readSize;

void __stdcall OnDataHdlr(Data::ByteArrayR buff, AnyType userData)
{
	readSize += buff.GetSize();
	hash->Calc(buff.Ptr(), buff.GetSize());
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NEW_CLASS(console, IO::ConsoleWriter());
	UIntOS argc;
	UnsafeArray<UnsafeArray<UTF8Char>> argv = progCtrl->GetCommandLines(progCtrl, argc);
	console->WriteLine(CSTR("CRC32 Calculation Test"));
	if (argc < 2)
	{
		console->Write(CSTR("Usage: "));
		console->Write(Text::CStringNN::FromPtr(argv[0]));
		console->WriteLine(CSTR(" [File]"));
	}
	else
	{
		Manage::HiResClock *clk;
		NN<IO::FileStream> fs;
		IO::ActiveStreamReader *reader;
		UInt8 hashVal[16];
		UTF8Char sbuff[40];
		UnsafeArray<UTF8Char> sptr;
		NEW_CLASS(hash, Crypto::Hash::CRC32());
		NEW_CLASS(clk, Manage::HiResClock());

		readSize = 0;
		NEW_CLASSNN(fs, IO::FileStream(Text::CStringNN::FromPtr(argv[1]), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		if (fs->IsError())
		{
			console->WriteLine(CSTR("Error in opening file"));
		}
		else
		{
			Double t1;
			Double t2;
			NEW_CLASS(reader, IO::ActiveStreamReader(OnDataHdlr, 0, 1048576));
			t1 = clk->GetTimeDiff();
			reader->ReadStream(fs, nullptr);
			t2 = clk->GetTimeDiff();
			DEL_CLASS(reader);

			sptr = Text::StrUInt64(sbuff, readSize);
			console->Write(CSTR("File Size: "));
			console->WriteLine(CSTRP(sbuff, sptr));

			console->Write(CSTR("CRC32: "));
			hash->GetValue(hashVal);
			sptr = Text::StrHexVal32(sbuff, ReadMUInt32(hashVal));
			console->WriteLine(CSTRP(sbuff, sptr));

			sptr = Text::StrDouble(sbuff, t2 - t1);
			console->Write(CSTR("Time Used: "));
			console->Write(CSTRP(sbuff, sptr));
			console->WriteLine(CSTR(" seconds"));

			sptr = Text::StrDouble(sbuff, (Double)readSize / 1048576.0 / (t2 - t1));
			console->Write(CSTR("Data Rate: "));
			console->Write(CSTRP(sbuff, sptr));
			console->WriteLine(CSTR(" MiB/s"));
		}
		fs.Delete();

		DEL_CLASS(clk);
		DEL_CLASS(hash);
	}
	DEL_CLASS(console);
	return 0;
}
