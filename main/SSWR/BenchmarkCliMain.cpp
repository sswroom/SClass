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
		return Text::StrConcat(Text::StrUInt64(sbuff, byteSize >> 30), (const UTF8Char*)"GB");
	}
	else if (byteSize >= 1048576)
	{
		return Text::StrConcat(Text::StrUInt64(sbuff, byteSize >> 20), (const UTF8Char*)"MB");
	}
	else if (byteSize >= 1024)
	{
		return Text::StrConcat(Text::StrUInt64(sbuff, byteSize >> 10), (const UTF8Char*)"KB");
	}
	else
	{
		return Text::StrConcat(Text::StrUInt64(sbuff, byteSize), (const UTF8Char*)"B");
	}
}


Int32 MyMain(Core::IProgControl *progCtrl)
{
	Manage::ExceptionRecorder *exHdlr;
	IO::ConsoleWriter *console;

	MemSetLogFile((const UTF8Char*)"Memory.log");
	NEW_CLASS(exHdlr, Manage::ExceptionRecorder((const UTF8Char*)"SBenchmarkCli.log", Manage::ExceptionRecorder::EA_CLOSE));
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
	sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Benchmark_");
	sptr2 = sysInfo.GetPlatformName(sptr);
	if (sptr2)
	{
		sptr = sptr2;
	}
	else
	{
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"Unknown");
	}
	Text::StrConcat(sptr, (const UTF8Char*)".txt");

	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(writer, IO::StreamWriter(fs, 65001));

	sb.ClearStr();
	sb.Append((const UTF8Char*)"Total Memory Size: ");
	memSize = sysInfo.GetTotalMemSize();
	ByteDisp(sbuff, memSize);
	sb.Append(sbuff);
	console->WriteLine(sb.ToString());


	sb.ClearStr();
	sb.Append((const UTF8Char*)"Platform: ");
	if (sysInfo.GetPlatformName(sbuff))
	{
		sb.Append(sbuff);
	}
	else
	{
		sb.Append((const UTF8Char*)"-");
	}
	writer->WriteLine(sb.ToString());
	console->WriteLine(sb.ToString());

	sb.ClearStr();
	sb.Append((const UTF8Char*)"CPU: ");
	if (cpuInfo.GetCPUName(sbuff))
	{
		sb.Append(sbuff);
	}
	else
	{
		sb.Append((const UTF8Char*)"-");
	}
	writer->WriteLine(sb.ToString());
	console->WriteLine(sb.ToString());

	Data::ArrayList<IO::SystemInfo::RAMInfo*> ramList;
	IO::SystemInfo::RAMInfo *ram;
	sysInfo.GetRAMInfo(&ramList);
	i = 0;
	j = ramList.GetCount();
	while (i < j)
	{
		ram = ramList.GetItem(i);
		sb.ClearStr();
		sb.Append((const UTF8Char*)"RAM: ");
		if (ram->deviceLocator)
		{
			sb.Append(ram->deviceLocator);
		}
		sb.Append((const UTF8Char*)"\t");
		if (ram->manufacturer)
		{
			sb.Append(ram->manufacturer);
		}
		sb.Append((const UTF8Char*)"\t");
		if (ram->partNo)
		{
			sb.Append(ram->partNo);
		}
		sb.Append((const UTF8Char*)"\t");
		if (ram->sn)
		{
			sb.Append(ram->sn);
		}
		sb.Append((const UTF8Char*)"\t");
		sb.AppendUOSInt(ram->defSpdMHz);
		sb.Append((const UTF8Char*)"\t");
		sb.AppendUOSInt(ram->confSpdMHz);
		sb.Append((const UTF8Char*)"\t");
		sb.AppendU32(ram->dataWidth);
		sb.Append((const UTF8Char*)"\t");
		sb.AppendU32(ram->totalWidth);
		sb.Append((const UTF8Char*)"\t");
		sb.AppendU64(ram->memorySize);
		writer->WriteLine(sb.ToString());
		console->WriteLine(sb.ToString());
		i++;
	}
	sysInfo.FreeRAMInfo(&ramList);

	writer->WriteLine((const UTF8Char*)"Result:");
	console->WriteLine((const UTF8Char*)"Result:");
	console->WriteLine((const UTF8Char*)"Type\tSize\tData Rate\tLoop Count\tUsed Time");

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
				sb.Append((const UTF8Char*)"Copy\t");
				sb.AppendUOSInt(currSize);
				sb.Append((const UTF8Char*)"\t");
				Text::SBAppendF64(&sb, rate);
				writer->WriteLine(sb.ToString());

				sb.ClearStr();
				sb.Append((const UTF8Char*)"Copy\t");
				ByteDisp(sbuff, currSize);
				sb.Append(sbuff);
				sb.Append((const UTF8Char*)"\t");
				Text::StrDoubleFmt(sbuff, rate, "0.0");
				sb.Append(sbuff);
				sb.Append((const UTF8Char*)"\t");
				sb.AppendUOSInt(loopCnt);
				sb.Append((const UTF8Char*)"\t");
				Text::SBAppendF64(&sb, t);
				console->WriteLine(sb.ToString());
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
				sb.Append((const UTF8Char*)"Write\t");
				sb.AppendUOSInt(currSize);
				sb.Append((const UTF8Char*)"\t");
				Text::SBAppendF64(&sb, rate);
				writer->WriteLine(sb.ToString());

				sb.ClearStr();
				sb.Append((const UTF8Char*)"Write\t");
				ByteDisp(sbuff, currSize);
				sb.Append(sbuff);
				sb.Append((const UTF8Char*)"\t");
				Text::StrDoubleFmt(sbuff, rate, "0.0");
				sb.Append(sbuff);
				sb.Append((const UTF8Char*)"\t");
				sb.AppendUOSInt(loopCnt);
				sb.Append((const UTF8Char*)"\t");
				Text::SBAppendF64(&sb, t);
				console->WriteLine(sb.ToString());
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
				sb.Append((const UTF8Char*)"Read\t");
				sb.AppendUOSInt(currSize);
				sb.Append((const UTF8Char*)"\t");
				Text::SBAppendF64(&sb, rate);
				writer->WriteLine(sb.ToString());

				sb.ClearStr();
				sb.Append((const UTF8Char*)"Read\t");
				ByteDisp(sbuff, currSize);
				sb.Append(sbuff);
				sb.Append((const UTF8Char*)"\t");
				Text::StrDoubleFmt(sbuff, rate, "0.0");
				sb.Append(sbuff);
				sb.Append((const UTF8Char*)"\t");
				sb.AppendUOSInt(loopCnt);
				sb.Append((const UTF8Char*)"\t");
				Text::SBAppendF64(&sb, t);
				console->WriteLine(sb.ToString());
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
