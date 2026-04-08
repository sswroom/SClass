#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/BitReaderLSB.h"
#include "IO/BitReaderMSB.h"
#include "IO/BitWriterLSB.h"
#include "IO/BitWriterMSB.h"
#include "IO/ConsoleWriter.h"
#include "IO/MemoryStream.h"
#include "Manage/HiResClock.h"
#include "Text/StringBuilderUTF8.h"


Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UInt32 code;
	IO::ConsoleWriter console;
	NN<IO::MemoryStream> mstm;
	IO::BitReader *reader;
	IO::BitWriter *writer;
	Double t1;
	Double t2;
	Bool succ = true;
	Manage::HiResClock *clk;

	UInt32 i;
	UInt32 endCode = 262144;
	UInt32 codeSize;
	UInt32 nextSizeCode;

	NEW_CLASS(clk, Manage::HiResClock());
	NEW_CLASSNN(mstm, IO::MemoryStream(1048576));
	clk->Start();
	NEW_CLASS(writer, IO::BitWriterMSB(mstm));
	i = 0;
	nextSizeCode = 2;
	codeSize = 1;
	while (nextSizeCode <= endCode)
	{
		while (i < nextSizeCode)
		{
			writer->WriteBits(i, codeSize);
			i++;
		}
		nextSizeCode = nextSizeCode << 1;
		codeSize++;
	}
	DEL_CLASS(writer);
	t1 = clk->GetTimeDiff();

	mstm->SeekFromBeginning(0);
	clk->Start();
	NEW_CLASS(reader, IO::BitReaderMSB(mstm));
	i = 0;
	nextSizeCode = 2;
	codeSize = 1;
	while (nextSizeCode <= endCode)
	{
		while (i < nextSizeCode)
		{
			if (!reader->ReadBits(code, codeSize))
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("Error in reading code "));
				sb.AppendU32(i);
				console.WriteLine(sb.ToCString());
				succ = false;
				break;
			}
			if (code != i)
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("Code mismatch "));
				sb.AppendU32(i);
				sb.AppendC(UTF8STRC(" != "));
				sb.AppendU32(code);
				console.WriteLine(sb.ToCString());
				succ = false;
				break;
			}
			i++;
		}
		if (!succ)
		{
			break;
		}
		nextSizeCode = nextSizeCode << 1;
		codeSize++;
	}
	t2 = clk->GetTimeDiff();

	if (succ)
	{
		console.WriteLine(CSTR("Success, all codes are the same"));
	}
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Time to write = "));
		sb.AppendDouble(t1);
		console.WriteLine(sb.ToCString());
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Time to read = "));
		sb.AppendDouble(t2);
		console.WriteLine(sb.ToCString());
	}

	DEL_CLASS(reader);
	mstm.Delete();
	return 0;
}
