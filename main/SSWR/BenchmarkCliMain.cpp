#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/StreamWriter.h"
#include "IO/SystemInfo.h"
#include "Manage/Benchmark.h"
#include "Manage/CPUInfo.h"
#include "Manage/ExceptionRecorder.h"
#include "Manage/HiResClock.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

UTF8Char *ByteDisp(UTF8Char *sbuff, UInt64 byteSize)
{
	if (byteSize >= 1073741824)
	{
		return Text::StrConcatC(Text::StrUInt64(sbuff, byteSize >> 30), UTF8STRC("GB"));
	}
	else if (byteSize >= 1048576)
	{
		return Text::StrConcatC(Text::StrUInt64(sbuff, byteSize >> 20), UTF8STRC("MB"));
	}
	else if (byteSize >= 1024)
	{
		return Text::StrConcatC(Text::StrUInt64(sbuff, byteSize >> 10), UTF8STRC("KB"));
	}
	else
	{
		return Text::StrConcatC(Text::StrUInt64(sbuff, byteSize), UTF8STRC("B"));
	}
}


Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Manage::ExceptionRecorder *exHdlr;
	IO::ConsoleWriter *console;

	MemSetLogFile(UTF8STRC("Memory.log"));
	NEW_CLASS(exHdlr, Manage::ExceptionRecorder(CSTR("SBenchmarkCli.log"), Manage::ExceptionRecorder::EA_CLOSE));
	NEW_CLASS(console, IO::ConsoleWriter());

	IO::SystemInfo sysInfo;
	Manage::CPUInfo cpuInfo;
	IO::FileStream *fs;
	IO::StreamWriter *writer;
	UInt64 memSize;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UOSInt i;
	UOSInt j;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("Benchmark_"));
	sptr2 = sysInfo.GetPlatformName(sptr);
	if (sptr2)
	{
		sptr = sptr2;
	}
	else
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("Unknown"));
	}
	sptr = Text::StrConcatC(sptr, UTF8STRC(".txt"));

	NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(writer, IO::StreamWriter(fs, 65001));

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Total Memory Size: "));
	memSize = sysInfo.GetTotalMemSize();
	sptr = ByteDisp(sbuff, memSize);
	sb.AppendP(sbuff, sptr);
	console->WriteLineC(sb.ToString(), sb.GetLength());


	sb.ClearStr();
	sb.AppendC(UTF8STRC("Platform: "));
	if ((sptr = sysInfo.GetPlatformName(sbuff)) != 0)
	{
		sb.AppendP(sbuff, sptr);
	}
	else
	{
		sb.AppendC(UTF8STRC("-"));
	}
	writer->WriteLineC(sb.ToString(), sb.GetLength());
	console->WriteLineC(sb.ToString(), sb.GetLength());

	sb.ClearStr();
	sb.AppendC(UTF8STRC("CPU: "));
	if ((sptr = cpuInfo.GetCPUName(sbuff)) != 0)
	{
		sb.AppendP(sbuff, sptr);
	}
	else
	{
		sb.AppendC(UTF8STRC("-"));
	}
	writer->WriteLineC(sb.ToString(), sb.GetLength());
	console->WriteLineC(sb.ToString(), sb.GetLength());

	Data::ArrayList<IO::SystemInfo::RAMInfo*> ramList;
	IO::SystemInfo::RAMInfo *ram;
	sysInfo.GetRAMInfo(&ramList);
	i = 0;
	j = ramList.GetCount();
	while (i < j)
	{
		ram = ramList.GetItem(i);
		sb.ClearStr();
		sb.AppendC(UTF8STRC("RAM: "));
		if (ram->deviceLocator)
		{
			sb.Append(ram->deviceLocator);
		}
		sb.AppendC(UTF8STRC("\t"));
		if (ram->manufacturer)
		{
			sb.Append(ram->manufacturer);
		}
		sb.AppendC(UTF8STRC("\t"));
		if (ram->partNo)
		{
			sb.Append(ram->partNo);
		}
		sb.AppendC(UTF8STRC("\t"));
		if (ram->sn)
		{
			sb.Append(ram->sn);
		}
		sb.AppendC(UTF8STRC("\t"));
		sb.AppendUOSInt(ram->defSpdMHz);
		sb.AppendC(UTF8STRC("\t"));
		sb.AppendUOSInt(ram->confSpdMHz);
		sb.AppendC(UTF8STRC("\t"));
		sb.AppendU32(ram->dataWidth);
		sb.AppendC(UTF8STRC("\t"));
		sb.AppendU32(ram->totalWidth);
		sb.AppendC(UTF8STRC("\t"));
		sb.AppendU64(ram->memorySize);
		writer->WriteLineC(sb.ToString(), sb.GetLength());
		console->WriteLineC(sb.ToString(), sb.GetLength());
		i++;
	}
	sysInfo.FreeRAMInfo(&ramList);

	writer->WriteLineC(UTF8STRC("Result:"));
	console->WriteLineC(UTF8STRC("Result:"));
	console->WriteLineC(UTF8STRC("Type\tSize\tData Rate\tLoop Count\tUsed Time"));

	Double oriT;
	Double t;
	Manage::HiResClock clk;
	UInt8 *buff1;
	UInt8 *buff2;
	UOSInt loopCnt;
	UOSInt currSize;
	UOSInt startSize = 128;
	UOSInt buffSize = 64 << 20;
	if (buffSize > (memSize >> 1))
	{
		buffSize = (UOSInt)(memSize >> 1);
	}

	buff1 = MemAllocA64(UInt8, buffSize);
	buff2 = MemAllocA64(UInt8, buffSize);
	clk.Start();
	oriT = clk.GetTimeDiff();

	currSize = startSize;
	while (currSize <= buffSize)
	{
		loopCnt = 1;
		while (true)
		{
			clk.Start();
			Benchmark_MemCopyTest(buff1, buff2, currSize, loopCnt);
			t = clk.GetTimeDiff() - oriT;
			if (t < 0.05)
			{
				loopCnt = loopCnt << 4;
			}
			else if (t >= 1)
			{
				Double rate = (Double)(currSize * (UInt64)loopCnt) / t * 2.0;

				sb.ClearStr();
				sb.AppendC(UTF8STRC("Copy\t"));
				sb.AppendUOSInt(currSize);
				sb.AppendC(UTF8STRC("\t"));
				Text::SBAppendF64(&sb, rate);
				writer->WriteLineC(sb.ToString(), sb.GetLength());

				sb.ClearStr();
				sb.AppendC(UTF8STRC("Copy\t"));
				sptr = ByteDisp(sbuff, currSize);
				sb.AppendP(sbuff, sptr);
				sb.AppendC(UTF8STRC("\t"));
				sptr = Text::StrDoubleFmt(sbuff, rate, "0.0");
				sb.AppendP(sbuff, sptr);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendUOSInt(loopCnt);
				sb.AppendC(UTF8STRC("\t"));
				Text::SBAppendF64(&sb, t);
				console->WriteLineC(sb.ToString(), sb.GetLength());
				break;
			}
			else
			{
				loopCnt = loopCnt << 1;
			}
		}
		currSize = currSize << 1;
	}

	currSize = startSize;
	while (currSize <= buffSize)
	{
		loopCnt = 1;
		while (true)
		{
			clk.Start();
			Benchmark_MemWriteTest(buff1, buff2, currSize, loopCnt);
			t = clk.GetTimeDiff() - oriT;
			if (t < 0.05)
			{
				loopCnt = loopCnt << 4;
			}
			else if (t >= 1)
			{
				Double rate = (Double)(currSize * (UInt64)loopCnt) / t;

				sb.ClearStr();
				sb.AppendC(UTF8STRC("Write\t"));
				sb.AppendUOSInt(currSize);
				sb.AppendC(UTF8STRC("\t"));
				Text::SBAppendF64(&sb, rate);
				writer->WriteLineC(sb.ToString(), sb.GetLength());

				sb.ClearStr();
				sb.AppendC(UTF8STRC("Write\t"));
				sptr = ByteDisp(sbuff, currSize);
				sb.AppendP(sbuff, sptr);
				sb.AppendC(UTF8STRC("\t"));
				sptr = Text::StrDoubleFmt(sbuff, rate, "0.0");
				sb.AppendP(sbuff, sptr);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendUOSInt(loopCnt);
				sb.AppendC(UTF8STRC("\t"));
				Text::SBAppendF64(&sb, t);
				console->WriteLineC(sb.ToString(), sb.GetLength());
				break;
			}
			else
			{
				loopCnt = loopCnt << 1;
			}
		}
		currSize = currSize << 1;
	}

	currSize = startSize;
	while (currSize <= buffSize)
	{
		loopCnt = 1;
		while (true)
		{
			clk.Start();
			Benchmark_MemReadTest(buff1, buff2, currSize, loopCnt);
			t = clk.GetTimeDiff() - oriT;
			if (t < 0.05)
			{
				loopCnt = loopCnt << 4;
			}
			else if (t >= 1)
			{
				Double rate = (Double)(currSize * (UInt64)loopCnt) / t;

				sb.ClearStr();
				sb.AppendC(UTF8STRC("Read\t"));
				sb.AppendUOSInt(currSize);
				sb.AppendC(UTF8STRC("\t"));
				Text::SBAppendF64(&sb, rate);
				writer->WriteLineC(sb.ToString(), sb.GetLength());

				sb.ClearStr();
				sb.AppendC(UTF8STRC("Read\t"));
				sptr = ByteDisp(sbuff, currSize);
				sb.AppendP(sbuff, sptr);
				sb.AppendC(UTF8STRC("\t"));
				sptr = Text::StrDoubleFmt(sbuff, rate, "0.0");
				sb.AppendP(sbuff, sptr);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendUOSInt(loopCnt);
				sb.AppendC(UTF8STRC("\t"));
				Text::SBAppendF64(&sb, t);
				console->WriteLineC(sb.ToString(), sb.GetLength());
				break;
			}
			else
			{
				loopCnt = loopCnt << 1;
			}
		}
		currSize = currSize << 1;
	}

	MemFreeA64(buff1);
	MemFreeA64(buff2);

	DEL_CLASS(writer);
	DEL_CLASS(fs);


	DEL_CLASS(console);
	DEL_CLASS(exHdlr);
	return 0;
}
