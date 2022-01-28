#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/HashCreator.h"
#include "Core/Core.h"
#include "Data/ArrayListInt32.h"
#include "Data/ByteTool.h"
#include "Data/RandomMT19937.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "Data/Sort/BitonicSortC.h"
#include "Data/Sort/BubbleSort.h"
#include "Data/Sort/ByteCountingSort.h"
#include "Data/Sort/InsertionSort.h"
//#include "Data/Sort/QuickBubbleSort.h"
#include "Exporter/TIFFExporter.h"
#include "IO/BuildTime.h"
#include "IO/Console.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/OS.h"
#include "IO/Path.h"
#include "IO/SystemInfo.h"
#include "Manage/Benchmark.h"
#include "Manage/CPUDB.h"
#include "Manage/CPUInfoDetail.h"
#include "Manage/ExceptionRecorder.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Math/Unit/Distance.h"
#include "Media/DDCReader.h"
#include "Media/EDID.h"
#include "Media/ImageList.h"
#include "Media/CS/CSRGB16_LRGBC.h"
#include "Media/ImageGen/RingsImageGen.h"
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "Media/Resizer/LanczosResizer16_C8.h"
#include "Media/Resizer/LanczosResizerH8_8.h"
//#include "Media/Resizer/LanczosResizerH16_8.h"
#include "Media/Resizer/LanczosResizerLR_C16.h"
#include "Media/Resizer/LanczosResizerLR_C32.h"
#include "Media/Resizer/NearestNeighbourResizer32_32.h"
#include "Media/Resizer/NearestNeighbourResizer64_64.h"
#include "Net/HTTPClient.h"
#include "Net/OSSocketFactory.h"
#include "Sync/Event.h"
#include "Sync/Interlocked.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"

//#define EXPORT_IMAGE 1

#define TEST_THREAD 1
#define TEST_IMP 1
#define TEST_HASH 1
#define TEST_SORT 1
#define TEST_MEMORY 1
#if !defined(NO_SWAP)
#define TEST_IMAGE 1
#endif

Manage::HiResClock *clk;
Sync::Mutex *mut;
Sync::Event *threadEvt;
Sync::Event *mainEvt;
Double threadT;

UInt32 __stdcall TestThread(void *userObj)
{
	threadT = clk->GetTimeDiff();
	clk->Start();
	mainEvt->Set();
	Sync::MutexUsage mutUsage(mut);
	Sync::Thread::Sleep(500);
	clk->Start();
	mutUsage.EndUse();
	return 0;
}

Double HashTestSpeed(Crypto::Hash::IHash *hash)
{
	UInt8 hashVal[64];
	UInt8 *testBlock = MemAllocA(UInt8, 1048576);
	OSInt i;
	clk->Start();
	i = 1024;
	while (i-- > 0)
	{
		hash->Calc(testBlock, 1048576);
	}
	hash->GetValue(hashVal);
	Double t = 1024.0 * 1048576.0 / clk->GetTimeDiff();
	MemFreeA(testBlock);
	return t;
}

UTF8Char *ByteDisp(UTF8Char *sbuff, UInt64 byteSize)
{
	if (byteSize >= 1073741824)
	{
		return Text::StrConcatC(Text::StrDoubleFmt(sbuff, (Double)byteSize / 1073741824.0, "0.#"), UTF8STRC("GB"));
	}
	else if (byteSize >= 1048576)
	{
		return Text::StrConcatC(Text::StrDoubleFmt(sbuff, (Double)byteSize / 1048576.0, "0.#"), UTF8STRC("MB"));
	}
	else if (byteSize >= 1024)
	{
		return Text::StrConcatC(Text::StrDoubleFmt(sbuff, (Double)byteSize / 1024.0, "0.#"), UTF8STRC("KB"));
	}
	else
	{
		return Text::StrConcatC(Text::StrUInt64(sbuff, byteSize), UTF8STRC("B"));
	}
}

UInt32 EmptyThread(void *userObj)
{
	return 0;
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Manage::ExceptionRecorder *exHdlr;
	IO::ConsoleWriter *console;

	IO::FileStream *fs;
#if defined(EXPORT_IMAGE)
	IO::FileStream *efs;
	const UTF8Char *csptr;
#endif
	Text::UTF8Writer *writer;
	IO::SystemInfo sysInfo;
	UInt64 memSize;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[512];
	UTF8Char u8buff[256];
	UTF16Char u16buff[32];
	UTF32Char u32buff[32];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	Double t;
	UOSInt threadCnt = Sync::Thread::GetThreadCnt();

	MemSetLogFile((const UTF8Char*)"Memory.log");
	NEW_CLASS(exHdlr, Manage::ExceptionRecorder((const UTF8Char*)"SBench.log", Manage::ExceptionRecorder::EA_CLOSE));
	NEW_CLASS(console, IO::ConsoleWriter());
	NEW_CLASS(clk, Manage::HiResClock());
	NEW_CLASS(mut, Sync::Mutex());
	NEW_CLASS(threadEvt, Sync::Event(true, (const UTF8Char*)"SBenchMain.threadEvt"));
	NEW_CLASS(mainEvt, Sync::Event(true, (const UTF8Char*)"SBenchMain.mainEvt"));

	IO::Path::GetProcessFileName(sbuff);
	IO::Path::ReplaceExt(sbuff, (const UTF8Char*)"txt");
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(writer, Text::UTF8Writer(fs));

	console->WriteLineC(UTF8STRC("SBench Result:"));
	writer->WriteLineC(UTF8STRC("SBench Result:"));
	console->WriteLineC(UTF8STRC("Computer Info:"));
	writer->WriteLineC(UTF8STRC("Computer Info:"));
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
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	{
		Manage::CPUInfoDetail cpuInfo;
		sb.ClearStr();
		sb.AppendC(UTF8STRC("CPU: "));
		if ((sptr = cpuInfo.GetCPUName(u8buff)) != 0)
		{
			sb.AppendP(u8buff, sptr);
		}
		else
		{
			sb.AppendC(UTF8STRC("-"));
		}
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());

		const Manage::CPUDB::CPUSpec *cpuSpec = 0;
		sb.ClearStr();
		sb.AppendC(UTF8STRC("CPU Model: "));
		if (cpuInfo.GetCPUModel().v)
		{
			sb.Append(cpuInfo.GetCPUModel());
			cpuSpec = Manage::CPUDB::GetCPUSpec(cpuInfo.GetCPUModel());
		}
		else
		{
			sb.AppendC(UTF8STRC("-"));
		}
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());

		sb.ClearStr();
		sb.AppendC(UTF8STRC("CPU Brand Name: "));
		if (cpuSpec)
		{
			sb.Append(Manage::CPUVendor::GetBrandName(cpuSpec->brand));
			sb.AppendC(UTF8STRC(" "));
			sb.AppendSlow((const UTF8Char*)cpuSpec->name);
		}
		else
		{
			sb.AppendC(UTF8STRC("-"));
		}
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());

#if defined(CPU_X86_32) || defined(CPU_X86_64)
		sb.ClearStr();
		sb.AppendC(UTF8STRC("CPU FamilyId = "));
		sb.AppendI32(cpuInfo.GetFamilyId());
		sb.AppendC(UTF8STRC(", ModelId = "));
		sb.AppendI32(cpuInfo.GetModelId());
		sb.AppendC(UTF8STRC(", Stepping = "));
		sb.AppendI32(cpuInfo.GetStepping());
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());
#endif
	}

	sb.ClearStr();
	sb.AppendC(UTF8STRC("CPU Thread Count: "));
	sb.AppendUOSInt(threadCnt);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());
	sb.ClearStr();
	sb.AppendC(UTF8STRC("OS: "));
	if ((sptr = IO::OS::GetDistro(sbuff)) != 0)
	{
		sb.AppendP(sbuff, sptr);
	}
	else
	{
		sb.AppendC(UTF8STRC("Unknown"));
	}
	if ((sptr = IO::OS::GetVersion(sbuff)) != 0)
	{
		sb.AppendC(UTF8STRC(" "));
		sb.AppendP(sbuff, sptr);
	}
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());
	{
		Data::DateTime dt;
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Build Time: "));
		IO::BuildTime::GetBuildTime(&dt);
		dt.ToUTCTime();
		sb.AppendDate(&dt);
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());

		dt.SetCurrTimeUTC();
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Benchmark Time: "));
		sb.AppendDate(&dt);
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());
	}

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Total Memory Size: "));
	memSize = sysInfo.GetTotalMemSize();
	sptr = ByteDisp(sbuff, memSize);
	sb.AppendP(sbuff, sptr);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Total Usable Memory Size: "));
	memSize = sysInfo.GetTotalUsableMemSize();
	sptr = ByteDisp(sbuff, memSize);
	sb.AppendP(sbuff, sptr);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	{
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
				sb.AppendSlow(ram->deviceLocator);
			}
			sb.AppendC(UTF8STRC("\t"));
			if (ram->manufacturer)
			{
				sb.AppendSlow(ram->manufacturer);
			}
			sb.AppendC(UTF8STRC("\t"));
			if (ram->partNo)
			{
				sb.AppendSlow(ram->partNo);
			}
			sb.AppendC(UTF8STRC("\t"));
			if (ram->sn)
			{
				sb.AppendSlow(ram->sn);
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
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());
			i++;
		}
		sysInfo.FreeRAMInfo(&ramList);
	}

	{
		Data::ArrayList<Media::DDCReader *> readerList;
		Media::DDCReader *reader;
		Media::DDCReader::CreateDDCReaders(&readerList);
		UOSInt edidSize;
		UInt8 *edid;
		i = 0;
		j = readerList.GetCount();
		while (i < j)
		{
			reader = readerList.GetItem(i);
			edid = reader->GetEDID(&edidSize);
			if (edid)
			{
				Media::EDID::EDIDInfo edidInfo;
				if (Media::EDID::Parse(edid, &edidInfo))
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Monitor "));
					sb.AppendUOSInt(i);
					sb.AppendC(UTF8STRC(" - "));
					sb.AppendSlow(edidInfo.monitorName);
					sb.AppendC(UTF8STRC(" ("));
					sb.AppendSlow(edidInfo.vendorName);
					sb.AppendC(UTF8STRC(" "));
					sb.AppendHex16(edidInfo.productCode);
					sb.AppendC(UTF8STRC(")"));
					console->WriteLineC(sb.ToString(), sb.GetLength());
					writer->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Monitor Manufacture: Year "));
					sb.AppendI32(edidInfo.yearOfManu);
					sb.AppendC(UTF8STRC(" Week "));
					sb.AppendU32(edidInfo.weekOfManu);
					console->WriteLineC(sb.ToString(), sb.GetLength());
					writer->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Monitor Size: "));
					Text::SBAppendF64(&sb, Math::Unit::Distance::Convert(Math::Unit::Distance::DU_CENTIMETER, Math::Unit::Distance::DU_INCH, Math_Sqrt(edidInfo.dispPhysicalW * edidInfo.dispPhysicalW + edidInfo.dispPhysicalH * edidInfo.dispPhysicalH)));
					sb.AppendC(UTF8STRC("\" ("));
					sb.AppendU32(edidInfo.dispPhysicalW);
					sb.AppendC(UTF8STRC(" x "));
					sb.AppendU32(edidInfo.dispPhysicalH);
					sb.AppendC(UTF8STRC("cm)"));
					console->WriteLineC(sb.ToString(), sb.GetLength());
					writer->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Monitor White: x = "));
					Text::SBAppendF64(&sb, edidInfo.wx);
					sb.AppendC(UTF8STRC(", y = "));
					Text::SBAppendF64(&sb, edidInfo.wy);
					console->WriteLineC(sb.ToString(), sb.GetLength());
					writer->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Monitor Red x = "));
					Text::SBAppendF64(&sb, edidInfo.rx);
					sb.AppendC(UTF8STRC(", y = "));
					Text::SBAppendF64(&sb, edidInfo.ry);
					console->WriteLineC(sb.ToString(), sb.GetLength());
					writer->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Monitor Green: x = "));
					Text::SBAppendF64(&sb, edidInfo.gx);
					sb.AppendC(UTF8STRC(", y = "));
					Text::SBAppendF64(&sb, edidInfo.gy);
					console->WriteLineC(sb.ToString(), sb.GetLength());
					writer->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Monitor Blue: x = "));
					Text::SBAppendF64(&sb, edidInfo.bx);
					sb.AppendC(UTF8STRC(", y = "));
					Text::SBAppendF64(&sb, edidInfo.by);
					console->WriteLineC(sb.ToString(), sb.GetLength());
					writer->WriteLineC(sb.ToString(), sb.GetLength());
				}
			}
			DEL_CLASS(reader);
			i++;
		}
	}

//-------------------------------------------------------------------------------
	console->WriteLine();
	writer->WriteLine();
	console->WriteLineC(UTF8STRC("API Performance:"));
	writer->WriteLineC(UTF8STRC("API Performance:"));

	clk->Start();
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Check Time: "));
	Text::SBAppendF64(&sb, t);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	clk->Start();
	i = 1000;
	while (i-- > 0)
	{
		Sync::Thread::GetThreadId();
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("GetThreadId: "));
	Text::SBAppendF64(&sb, t / 1000.0);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	clk->Start();
	i = 1000;
	while (i-- > 0)
	{
		Sync::MutexUsage mutUsage(mut);
		mutUsage.EndUse();
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Mutex Lock Unlock: "));
	Text::SBAppendF64(&sb, t / 1000.0);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	Int32 tmpVal = 0;
	clk->Start();
	i = 1000;
	while (i-- > 0)
	{
		Sync::Interlocked::Increment(&tmpVal);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Interlocked Increment: "));
	Text::SBAppendF64(&sb, t / 1000.0);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	clk->Start();
	i = 1000;
	while (i-- > 0)
	{
		mainEvt->Set();
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Event.Set: "));
	Text::SBAppendF64(&sb, t / 1000.0);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	mainEvt->Clear();
	clk->Start();
	mainEvt->Wait(100);
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Event.Wait(100): "));
	Text::SBAppendF64(&sb, t);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());

#if defined(TEST_THREAD)
	threadT = 0;
	clk->Start();
	Sync::Thread::Create(TestThread, 0);
	mainEvt->Wait(1000);
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Thread Create: "));
	Text::SBAppendF64(&sb, threadT);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Event Wake: "));
	Text::SBAppendF64(&sb, t);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	Sync::Thread::Sleep(100);
	Sync::MutexUsage mutUsage(mut);
	t = clk->GetTimeDiff();
	mutUsage.EndUse();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Mutex Lock Release: "));
	Text::SBAppendF64(&sb, t);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());
#endif

	UInt8 **tmpArr = MemAlloc(UInt8*, 1000);
	i = 1000;
	while (i-- > 0)
	{
		tmpArr[i] = MemAllocA(UInt8, 16);
	}
	i = 1000;
	while (i-- > 0)
	{
		MemFreeA(tmpArr[i]);
	}

	clk->Start();
	i = 1000;
	while (i-- > 0)
	{
		tmpArr[i] = MemAlloc(UInt8, 16);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("MemAlloc: "));
	Text::SBAppendF64(&sb, t / 1000.0);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	clk->Start();
	i = 1000;
	while (i-- > 0)
	{
		MemFree(tmpArr[i]);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("MemFree: "));
	Text::SBAppendF64(&sb, t / 1000.0);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	clk->Start();
	i = 1000;
	while (i-- > 0)
	{
		tmpArr[i] = MemAllocA(UInt8, 16);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("MemAllocA: "));
	Text::SBAppendF64(&sb, t / 1000.0);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	clk->Start();
	i = 1000;
	while (i-- > 0)
	{
		MemFreeA(tmpArr[i]);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("MemFreeA: "));
	Text::SBAppendF64(&sb, t / 1000.0);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	MemFree(tmpArr);

//-------------------------------------------------------------------------------
	console->WriteLine();
	writer->WriteLine();
	console->WriteLineC(UTF8STRC("Implementation Performance:"));
	writer->WriteLineC(UTF8STRC("Implementation Performance:"));

	Int32 i32V;
	Int64 i64V;

#if defined(TEST_IMP)
	clk->Start();
	i32V = 100000000;
	while (i32V-- > 0)
	{
		Text::StrInt32(u8buff, i32V);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("StrInt32 (UTF8 0 - 99999999): "));
	Text::SBAppendF64(&sb, t);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	clk->Start();
	i32V = 100000000;
	while (i32V-- > 0)
	{
		Text::StrInt32(u16buff, i32V);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("StrInt32 (UTF16 0 - 99999999): "));
	Text::SBAppendF64(&sb, t);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	clk->Start();
	i32V = 100000000;
	while (i32V-- > 0)
	{
		Text::StrInt32(u32buff, i32V);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("StrInt32 (UTF32 0 - 99999999): "));
	Text::SBAppendF64(&sb, t);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	clk->Start();
	i64V = 100100000000LL;
	while (i64V-- > 100000000000LL)
	{
		Text::StrInt64(u8buff, i64V);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("StrInt64 (UTF8 100000000000 - 100099999999): "));
	Text::SBAppendF64(&sb, t);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	clk->Start();
	i64V = 100100000000LL;
	while (i64V-- > 100000000000LL)
	{
		Text::StrInt64(u16buff, i64V);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("StrInt64 (UTF16 100000000000 - 100099999999): "));
	Text::SBAppendF64(&sb, t);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	clk->Start();
	i64V = 100100000000LL;
	while (i64V-- > 100000000000LL)
	{
		Text::StrInt64(u32buff, i64V);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("StrInt64 (UTF32 100000000000 - 100099999999): "));
	Text::SBAppendF64(&sb, t);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	clk->Start();
	i32V = 10000;
	while (i32V-- > 0)
	{
		Text::StrDouble(u8buff, Math::PI);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("StrDouble (UTF8 PI): "));
	Text::SBAppendF64(&sb, t / 10000.0);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	clk->Start();
	i32V = 10000;
	while (i32V-- > 0)
	{
		Text::StrDouble(u16buff, Math::PI);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("StrDouble (UTF16 PI): "));
	Text::SBAppendF64(&sb, t / 10000.0);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	clk->Start();
	i32V = 10000;
	while (i32V-- > 0)
	{
		Text::StrDouble(u32buff, Math::PI);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("StrDouble (UTF32 PI): "));
	Text::SBAppendF64(&sb, t / 10000.0);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	{
		UInt64 maxVal = memSize >> 5;
		if (maxVal > 100000000)
		{
			maxVal = 100000000;
		}
		Data::RandomMT19937 rand(0);
		Data::ArrayListInt32 *arrList;
		NEW_CLASS(arrList, Data::ArrayListInt32());
		clk->Start();
		i32V = (Int32)maxVal;
		while (i32V-- > 0)
		{
			arrList->Add(i32V);
		}
		t = clk->GetTimeDiff();
		sb.ClearStr();
		sb.AppendC(UTF8STRC("ArrayList<Int32>.Add (0 - "));
		sb.AppendU64(maxVal - 1);
		sb.AppendC(UTF8STRC("): "));
		Text::SBAppendF64(&sb, t);
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());

		clk->Start();
		while ((i = arrList->GetCount()) > 0)
		{
			arrList->RemoveAt(i - 1);
		}
		t = clk->GetTimeDiff();
		sb.ClearStr();
		sb.AppendC(UTF8STRC("ArrayList<Int32>.RemoveAt(Last) * "));
		sb.AppendU64(maxVal);
		sb.AppendC(UTF8STRC(": "));
		Text::SBAppendF64(&sb, t);
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());

		clk->Start();
		i = 100000;
		while (i-- > 0)
		{
			arrList->SortedInsert(rand.NextInt32());
		}
		t = clk->GetTimeDiff();
		sb.ClearStr();
		sb.AppendC(UTF8STRC("ArrayListInt32.SortedInsert(rand) * 100000: "));
		Text::SBAppendF64(&sb, t);
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());

		DEL_CLASS(arrList);
	}
#endif

#if defined(TEST_HASH)
	Crypto::Hash::IHash *hash;
	i = Crypto::Hash::HT_FIRST;
	while (i <= Crypto::Hash::HT_LAST)
	{
		hash = Crypto::Hash::HashCreator::CreateHash((Crypto::Hash::HashType)i);
		if (hash)
		{
			t = HashTestSpeed(hash);
			sptr = hash->GetName(sbuff);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Hash "));
			sb.AppendP(sbuff, sptr);
			sb.AppendC(UTF8STRC(": "));
			sptr = Text::StrDoubleFmt(sbuff, t, "#,###.###");
			sb.AppendP(sbuff, sptr);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			DEL_CLASS(hash);
		}
		i++;
	}
#endif

#if defined(TEST_SORT)
	UOSInt recordCnt = 100000000;
	UOSInt noOfRec;
	Bool valid;
	UInt32 seed = 0;
	UInt32 *iarray1;
	UInt32 *iarray2;

	if (recordCnt > memSize >> 5)
	{
		recordCnt = memSize >> 5;
	}

	iarray1 = 0;
	iarray2 = 0;
	while (true)
	{
		if (iarray1)
		{
			MemFreeA(iarray1);
		}
		iarray1 = MemAllocA(UInt32, recordCnt);
		iarray2 = MemAllocA(UInt32, recordCnt);
		if (iarray1 && iarray2)
		{
			break;
		}
		recordCnt = recordCnt >> 1;
	}

	Data::RandomMT19937 *rand;
	NEW_CLASS(rand, Data::RandomMT19937(seed));
	clk->Start();
	i = 0;
	while (i < recordCnt)
	{
		iarray1[i] = (UInt32)rand->NextInt32();
		i++;
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Gen Random (MT19937) N = "));
	sb.AppendUOSInt(recordCnt);
	sb.AppendC(UTF8STRC(", seed = "));
	sb.AppendU32(seed);
	sb.AppendC(UTF8STRC(", time = "));
	Text::SBAppendF64(&sb, t);
	sb.AppendC(UTF8STRC("s"));
	console->WriteLineC(sb.ToString(), sb.GetLength());
	writer->WriteLineC(sb.ToString(), sb.GetLength());
	DEL_CLASS(rand);

	noOfRec = 128;
	while (noOfRec <= recordCnt)
	{
		MemCopyANC(iarray2, iarray1, noOfRec * sizeof(Int32));
		clk->Start();
		ArtificialQuickSort_SortUInt32(iarray2, 0, (OSInt)noOfRec - 1);
		t = clk->GetTimeDiff();
		sb.ClearStr();
		sb.AppendC(UTF8STRC("AQuickSort 1 Thread N = "));
		sb.AppendUOSInt(noOfRec);
		sb.AppendC(UTF8STRC(", Sort time = "));
		Text::SBAppendF64(&sb, t);
		sb.AppendC(UTF8STRC("s"));

		valid = true;
		i = 0;
		while (i < noOfRec - 1)
		{
			if (iarray2[i] > iarray2[i + 1])
			{
				valid = false;
				break;
			}
			i++;
		}
		if (valid)
		{
			sb.AppendC(UTF8STRC(", Result sorted"));
		}
		else
		{
			sb.AppendC(UTF8STRC(", Result wrong"));
		}
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());

		if (t >= 1.0)
		{
			break;
		}
		noOfRec = noOfRec << 1;
	}

#if defined(TEST_THREAD)
	if (threadCnt > 1)
	{
		Data::Sort::ArtificialQuickSort *aqsort;
		NEW_CLASS(aqsort, Data::Sort::ArtificialQuickSort());
		noOfRec = 128;
		while (noOfRec <= recordCnt)
		{
			MemCopyANC(iarray2, iarray1, noOfRec * sizeof(Int32));
			clk->Start();
			aqsort->SortUInt32(iarray2, 0, (OSInt)noOfRec - 1);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("AQuickSort "));
			sb.AppendUOSInt(Sync::Thread::GetThreadCnt());
			sb.AppendC(UTF8STRC(" Thread N = "));
			sb.AppendUOSInt(noOfRec);
			sb.AppendC(UTF8STRC(", Sort time = "));
			Text::SBAppendF64(&sb, t);
			sb.AppendC(UTF8STRC("s"));
			valid = true;
			i = 0;
			while (i < noOfRec - 1)
			{
				if (iarray2[i] > iarray2[i + 1])
				{
					valid = false;
					break;
				}
				i++;
			}
			if (valid)
			{
				sb.AppendC(UTF8STRC(", Result sorted"));
			}
			else
			{
				sb.AppendC(UTF8STRC(", Result wrong"));
			}
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			if (t >= 1.0)
			{
				break;
			}
			noOfRec = noOfRec << 1;
		}
		DEL_CLASS(aqsort);
	}
#endif

	noOfRec = 128;
	while (noOfRec <= recordCnt)
	{
		MemCopyANC(iarray2, iarray1, noOfRec * sizeof(Int32));
		clk->Start();
		BubbleSort_SortUInt32(iarray2, 0, (OSInt)noOfRec - 1);
		t = clk->GetTimeDiff();
		sb.ClearStr();
		sb.AppendC(UTF8STRC("BubbleSortv1 1 Thread N = "));
		sb.AppendUOSInt(noOfRec);
		sb.AppendC(UTF8STRC(", Sort time = "));
		Text::SBAppendF64(&sb, t);
		sb.AppendC(UTF8STRC("s"));
		valid = true;
		i = 0;
		while (i < noOfRec - 1)
		{
			if (iarray2[i] > iarray2[i + 1])
			{
				valid = false;
				break;
			}
			i++;
		}
		if (valid)
		{
			sb.AppendC(UTF8STRC(", Result sorted"));
		}
		else
		{
			sb.AppendC(UTF8STRC(", Result wrong"));
		}
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());

		if (t >= 1.0)
		{
			break;
		}
		noOfRec = noOfRec << 1;
	}

	noOfRec = 128;
	while (noOfRec <= recordCnt)
	{
		MemCopyANC(iarray2, iarray1, noOfRec * sizeof(Int32));
		clk->Start();
		InsertionSort_SortUInt32(iarray2, 0, (OSInt)noOfRec - 1);
		t = clk->GetTimeDiff();
		sb.ClearStr();
		sb.AppendC(UTF8STRC("InsertionSort 1 Thread N = "));
		sb.AppendUOSInt(noOfRec);
		sb.AppendC(UTF8STRC(", Sort time = "));
		Text::SBAppendF64(&sb, t);
		sb.AppendC(UTF8STRC("s"));
		valid = true;
		i = 0;
		while (i < noOfRec - 1)
		{
			if (iarray2[i] > iarray2[i + 1])
			{
				valid = false;
				break;
			}
			i++;
		}
		if (valid)
		{
			sb.AppendC(UTF8STRC(", Result sorted"));
		}
		else
		{
			sb.AppendC(UTF8STRC(", Result wrong"));
		}
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());

		if (t >= 1.0)
		{
			break;
		}
		noOfRec = noOfRec << 1;
	}

	noOfRec = 128;
	while (noOfRec <= recordCnt)
	{
		MemCopyANC(iarray2, iarray1, noOfRec * sizeof(Int32));
		clk->Start();
		BitonicSort_SortUInt32(iarray2, 0, (OSInt)noOfRec - 1);
		t = clk->GetTimeDiff();
		sb.ClearStr();
		sb.AppendC(UTF8STRC("BitonicSort 1 Thread N = "));
		sb.AppendUOSInt(noOfRec);
		sb.AppendC(UTF8STRC(", Sort time = "));
		Text::SBAppendF64(&sb, t);
		sb.AppendC(UTF8STRC("s"));
		valid = true;
		i = 0;
		while (i < noOfRec - 1)
		{
			if (iarray2[i] > iarray2[i + 1])
			{
				valid = false;
				break;
			}
			i++;
		}
		if (valid)
		{
			sb.AppendC(UTF8STRC(", Result sorted"));
		}
		else
		{
			sb.AppendC(UTF8STRC(", Result wrong"));
		}
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());

		if (t >= 1.0)
		{
			break;
		}
		noOfRec = noOfRec << 1;
	}

#if 0 && defined(TEST_THREAD)
	if (threadCnt > 1)
	{
		Data::Sort::BitonicSort *bitsort;
		NEW_CLASS(bitsort, Data::Sort::BitonicSort());
		noOfRec = 128;
		while (noOfRec <= recordCnt)
		{
			MemCopyANC(iarray2, iarray1, noOfRec * sizeof(Int32));
			clk->Start();
			bitsort->SortUInt32(iarray2, 0, noOfRec - 1);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("BitonicSort ");
			sb.AppendOSInt(Sync::Thread::GetThreadCnt());
			sb.AppendC(UTF8STRC(" Thread N = ");
			sb.AppendOSInt(noOfRec);
			sb.AppendC(UTF8STRC(", Sort time = ");
			Text::SBAppendF64(&sb, t);
			sb.AppendC(UTF8STRC("s");
			valid = true;
			i = 0;
			while (i < noOfRec - 1)
			{
				if (iarray2[i] > iarray2[i + 1])
				{
					valid = false;
					break;
				}
				i++;
			}
			if (valid)
			{
				sb.AppendC(UTF8STRC(", Result sorted");
			}
			else
			{
				sb.AppendC(UTF8STRC(", Result wrong");
			}
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			if (t >= 1.0)
			{
				break;
			}
			noOfRec = noOfRec << 1;
		}
		DEL_CLASS(bitsort);
	}
#endif

	noOfRec = 128;
	while (noOfRec <= recordCnt)
	{
		MemCopyANC(iarray2, iarray1, noOfRec * sizeof(Int32));
		clk->Start();
		if (!ByteCountingSort_SortUInt32(iarray2, 0, (OSInt)noOfRec - 1))
			break;
		t = clk->GetTimeDiff();
		sb.ClearStr();
		sb.AppendC(UTF8STRC("BCountSort 1 Thread N = "));
		sb.AppendUOSInt(noOfRec);
		sb.AppendC(UTF8STRC(", Sort time = "));
		Text::SBAppendF64(&sb, t);
		sb.AppendC(UTF8STRC("s"));
		valid = true;
		i = 0;
		while (i < noOfRec - 1)
		{
			if (iarray2[i] > iarray2[i + 1])
			{
				valid = false;
				break;
			}
			i++;
		}
		if (valid)
		{
			sb.AppendC(UTF8STRC(", Result sorted"));
		}
		else
		{
			sb.AppendC(UTF8STRC(", Result wrong"));
		}
		console->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(sb.ToString(), sb.GetLength());

		if (t >= 1.0)
		{
			break;
		}
		noOfRec = noOfRec << 1;
	}

	MemFreeA(iarray1);
	MemFreeA(iarray2);
#endif

//-------------------------------------------------------------------------------
#if defined(TEST_MEMORY)
	console->WriteLine();
	writer->WriteLine();
	console->WriteLineC(UTF8STRC("Memory Performance:"));
	writer->WriteLineC(UTF8STRC("Memory Performance:"));
	console->WriteLineC(UTF8STRC("Type\tSize\tData Rate\tLoop Count\tUsed Time"));
	writer->WriteLineC(UTF8STRC("Type\tSize\tData Rate\tLoop Count\tUsed Time"));

	Double oriT;
	UInt8 *buff1;
	UInt8 *buff2;
	UOSInt loopCnt;
	UOSInt currSize;
	UOSInt startSize = 128;
	UOSInt buffSize = 64 << 20;
	while (buffSize > (memSize >> 2))
	{
		buffSize = buffSize >> 1;
	}

	while (true)
	{
		buff1 = MemAllocA64(UInt8, buffSize);
		buff2 = MemAllocA64(UInt8, buffSize);
		if (buff1 && buff2)
		{
			break;
		}

		if (buff1)
		{
			MemFreeA(buff1);
		}
		if (buff2)
		{
			MemFreeA(buff2);
		}
		buffSize = buffSize >> 1;
	}
	clk->Start();
	oriT = clk->GetTimeDiff();

	currSize = startSize;
	while (currSize <= buffSize)
	{
		loopCnt = 1;
		while (true)
		{
			clk->Start();
			Benchmark_MemCopyTest(buff1, buff2, currSize, loopCnt);
			t = clk->GetTimeDiff() - oriT;
			if (t < 0.05)
			{
				loopCnt = loopCnt << 4;
			}
			else if (t >= 1)
			{
				Double rate = (Double)(currSize * loopCnt) / t * 2.0;

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
				writer->WriteLineC(sb.ToString(), sb.GetLength());
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
			clk->Start();
			Benchmark_MemWriteTest(buff1, buff2, currSize, loopCnt);
			t = clk->GetTimeDiff() - oriT;
			if (t < 0.05)
			{
				loopCnt = loopCnt << 4;
			}
			else if (t >= 1)
			{
				Double rate = (Double)(currSize * loopCnt) / t;

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
				writer->WriteLineC(sb.ToString(), sb.GetLength());
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
			clk->Start();
			Benchmark_MemReadTest(buff1, buff2, currSize, loopCnt);
			t = clk->GetTimeDiff() - oriT;
			if (t < 0.05)
			{
				loopCnt = loopCnt << 4;
			}
			else if (t >= 1)
			{
				Double rate = (Double)(currSize * loopCnt) / t;

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
				writer->WriteLineC(sb.ToString(), sb.GetLength());
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
			clk->Start();
			i = loopCnt;
			while (i-- > 0)
			{
				MemCopyNO(buff1, buff2, currSize);
			}
			t = clk->GetTimeDiff() - oriT;
			if (t < 0.05)
			{
				loopCnt = loopCnt << 4;
			}
			else if (t >= 1)
			{
				Double rate = (Double)(currSize * loopCnt) / t * 2.0;

				sb.ClearStr();
				sb.AppendC(UTF8STRC("MemCopyNO\t"));
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
				writer->WriteLineC(sb.ToString(), sb.GetLength());
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
			clk->Start();
			i = loopCnt;
			while (i-- > 0)
			{
				MemCopyO(buff1, buff2, currSize);
			}
			t = clk->GetTimeDiff() - oriT;
			if (t < 0.05)
			{
				loopCnt = loopCnt << 4;
			}
			else if (t >= 1)
			{
				Double rate = (Double)(currSize * loopCnt) / t * 2.0;

				sb.ClearStr();
				sb.AppendC(UTF8STRC("MemCopyO\t"));
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
				writer->WriteLineC(sb.ToString(), sb.GetLength());
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
			clk->Start();
			i = loopCnt;
			while (i-- > 0)
			{
				MemCopyAC(buff1, buff2, currSize);
			}
			t = clk->GetTimeDiff() - oriT;
			if (t < 0.05)
			{
				loopCnt = loopCnt << 4;
			}
			else if (t >= 1)
			{
				Double rate = (Double)(currSize * loopCnt) / t * 2.0;

				sb.ClearStr();
				sb.AppendC(UTF8STRC("MemCopyAC\t"));
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
				writer->WriteLineC(sb.ToString(), sb.GetLength());
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
			clk->Start();
			i = loopCnt;
			while (i-- > 0)
			{
				MemCopyANC(buff1, buff2, currSize);
			}
			t = clk->GetTimeDiff() - oriT;
			if (t < 0.05)
			{
				loopCnt = loopCnt << 4;
			}
			else if (t >= 1)
			{
				Double rate = (Double)(currSize * loopCnt) / t * 2.0;

				sb.ClearStr();
				sb.AppendC(UTF8STRC("MemCopyANC\t"));
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
				writer->WriteLineC(sb.ToString(), sb.GetLength());
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
#endif

#if defined(TEST_IMAGE) && defined(TEST_THREAD)
	{
		Exporter::TIFFExporter exporter;
		Media::ImageList *imgList;
		Media::ImageGen::RingsImageGen *imgGen;
		Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
		UOSInt imgWidth;
		UOSInt imgHeight;
		Media::StaticImage *srcImg;
		UInt8 *tmpBuff;
		Media::CS::CSConverter *csconv;

		console->WriteLine();
		writer->WriteLine();
		console->WriteLineC(UTF8STRC("Image Performance:"));
		writer->WriteLineC(UTF8STRC("Image Performance:"));
		imgWidth = 3840;
		imgHeight = 2160;
		while (imgWidth * imgHeight * 8 * 4 >= memSize)
		{
			imgWidth = imgWidth >> 1;
			imgHeight = imgHeight >> 1;
		}

		NEW_CLASS(imgGen, Media::ImageGen::RingsImageGen());

		clk->Start();
		srcImg = (Media::StaticImage*)imgGen->GenerateImage(&color, imgWidth, imgHeight);
		t = clk->GetTimeDiff();
		DEL_CLASS(imgGen);
		if (srcImg == 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Generate Rings Image: Error, size = "));
			sb.AppendUOSInt(imgWidth);
			sb.AppendC(UTF8STRC(" x "));
			sb.AppendUOSInt(imgHeight);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());
		}
		else
		{
			Media::StaticImage *newImg;
			Media::IImgResizer *resizer;
			UOSInt cnt;

			sb.ClearStr();
			sb.AppendC(UTF8STRC("Generate Rings Image: t = "));
			Text::SBAppendF64(&sb, t);
			sb.AppendC(UTF8STRC(", size = "));
			sb.AppendUOSInt(imgWidth);
			sb.AppendC(UTF8STRC(" x "));
			sb.AppendUOSInt(imgHeight);
			sb.AppendC(UTF8STRC(" "));
			sb.AppendU32(srcImg->info->storeBPP);
			sb.AppendC(UTF8STRC(" bpp"));
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			NEW_CLASS(imgList, Media::ImageList((const UTF8Char*)"Temp"));
#if defined(EXPORT_IMAGE)
			IO::Path::GetProcessFileName(sbuff);
			IO::Path::AppendPath(sbuff, csptr = (const UTF8Char*)"RingsImage64.tif");
			NEW_CLASS(efs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			imgList->AddImage(srcImg, 0);
			exporter.ExportFile(efs, csptr, imgList, 0);
			imgList->RemoveImage(0, false);
			DEL_CLASS(efs);
#endif

			tmpBuff = MemAllocA(UInt8, imgWidth * imgHeight * 8);

			clk->Start();
			NEW_CLASS(newImg, Media::StaticImage(imgWidth >> 1, imgHeight >> 1, 0, 64, Media::PF_LE_B16G16R16A16, 0, &color, Media::ColorProfile::YUVT_BT709, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Create Image: t = "));
			Text::SBAppendF64(&sb, t);
			sb.AppendC(UTF8STRC(", size = "));
			sb.AppendUOSInt(imgWidth >> 1);
			sb.AppendC(UTF8STRC(" x "));
			sb.AppendUOSInt(imgHeight >> 1);
			sb.AppendC(UTF8STRC(" 64 bpp"));
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			imgList->AddImage(newImg, 0);

			//Test Thread
/*			i = 100;
			while (i-- > 0)
			{
				NEW_CLASS(resizer, Media::Resizer::LanczosResizer16_C8(4, 3, &color, &color, 0, Media::AT_NO_ALPHA));
				DEL_CLASS(resizer);
			}*/

			console->WriteLineC(UTF8STRC("Resizer: NearestNeighbourResizer (64->64)"));
			writer->WriteLineC(UTF8STRC("Resizer: NearestNeighbourResizer (64->64)"));
			NEW_CLASS(resizer, Media::Resizer::NearestNeighbourResizer64_64());
			console->WriteLineC(UTF8STRC("Initialized"));

			clk->Start();
			resizer->Resize(srcImg->data, (OSInt)srcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (1st): t = "));
			Text::SBAppendF64(&sb, t);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			if (t * 30 > 10)
				cnt = 1 + (UInt32)Double2Int32(10 / t);
			else
				cnt = 30;
			clk->Start();
			i = cnt;
			while (i-- > 0)
			{
				resizer->Resize(srcImg->data, (OSInt)srcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			}
			t = clk->GetTimeDiff() / UOSInt2Double(cnt);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (2nd): t = "));
			Text::SBAppendF64(&sb, t);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());
			DEL_CLASS(resizer);

#if defined(EXPORT_IMAGE)
			IO::Path::GetProcessFileName(sbuff);
			IO::Path::AppendPath(sbuff, csptr = (const UTF8Char*)"NNResize64_64.tif");
			NEW_CLASS(efs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			exporter.ExportFile(efs, csptr, imgList, 0);
			DEL_CLASS(efs);
#endif

			clk->Start();
			DEL_CLASS(imgList);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Delete Image List: t = "));
			Text::SBAppendF64(&sb, t);
			sb.AppendC(UTF8STRC(", size = "));
			sb.AppendUOSInt(imgWidth >> 1);
			sb.AppendC(UTF8STRC(" x "));
			sb.AppendUOSInt(imgHeight >> 1);
			sb.AppendC(UTF8STRC(" 64 bpp"));
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			clk->Start();
			NEW_CLASS(newImg, Media::StaticImage(imgWidth >> 1, imgHeight >> 1, 0, 32, Media::PF_B8G8R8A8, 0, &color, Media::ColorProfile::YUVT_BT709, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Create Image: t = "));
			Text::SBAppendF64(&sb, t);
			sb.AppendC(UTF8STRC(", size = "));
			sb.AppendUOSInt(imgWidth >> 1);
			sb.AppendC(UTF8STRC(" x "));
			sb.AppendUOSInt(imgHeight >> 1);
			sb.AppendC(UTF8STRC(" 32 bpp"));
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			NEW_CLASS(imgList, Media::ImageList((const UTF8Char*)"Temp"));
			imgList->AddImage(newImg, 0);

			console->WriteLineC(UTF8STRC("Resizer: LanczosResizer (16->8) H4V3 taps, no alpha"));
			writer->WriteLineC(UTF8STRC("Resizer: LanczosResizer (16->8) H4V3 taps, no alpha"));
			NEW_CLASS(resizer, Media::Resizer::LanczosResizer16_C8(4, 3, &color, &color, 0, Media::AT_NO_ALPHA));
			console->WriteLineC(UTF8STRC("Initialized"));

			clk->Start();
			resizer->Resize(srcImg->data, (OSInt)srcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (1st): t = "));
			Text::SBAppendF64(&sb, t);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			if (t * 30 > 10)
				cnt = 1 + (UInt32)Double2Int32(10 / t);
			else
				cnt = 30;
			clk->Start();
			i = cnt;
			while (i-- > 0)
			{
				resizer->Resize(srcImg->data, (OSInt)srcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			}
			t = clk->GetTimeDiff() / UOSInt2Double(cnt);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (2nd): t = "));
			Text::SBAppendF64(&sb, t);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());
			DEL_CLASS(resizer);

#if defined(EXPORT_IMAGE)
			IO::Path::GetProcessFileName(sbuff);
			IO::Path::AppendPath(sbuff, csptr = (const UTF8Char*)"LResize16_C8NA.tif");
			NEW_CLASS(efs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			exporter.ExportFile(efs, csptr, imgList, 0);
			DEL_CLASS(efs);
#endif

			console->WriteLineC(UTF8STRC("Resizer: LanczosResizer (16->8) H4V3 taps, alpha"));
			writer->WriteLineC(UTF8STRC("Resizer: LanczosResizer (16->8) H4V3 taps, alpha"));
			NEW_CLASS(resizer, Media::Resizer::LanczosResizer16_C8(4, 3, &color, &color, 0, Media::AT_ALPHA));
			console->WriteLineC(UTF8STRC("Initialized"));

			clk->Start();
			resizer->Resize(srcImg->data, (OSInt)srcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (1st): t = "));
			Text::SBAppendF64(&sb, t);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			if (t * 30 > 10)
				cnt = 1 + (UInt32)Double2Int32(10 / t);
			else
				cnt = 30;
			clk->Start();
			i = cnt;
			while (i-- > 0)
			{
				resizer->Resize(srcImg->data, (OSInt)srcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			}
			t = clk->GetTimeDiff() / UOSInt2Double(cnt);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (2nd): t = "));
			Text::SBAppendF64(&sb, t);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());
			DEL_CLASS(resizer);

#if defined(EXPORT_IMAGE)
			IO::Path::GetProcessFileName(sbuff);
			IO::Path::AppendPath(sbuff, csptr = (const UTF8Char*)"LResize16_C8A.tif");
			NEW_CLASS(efs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			exporter.ExportFile(efs, csptr, imgList, 0);
			DEL_CLASS(efs);
#endif

/*			NEW_CLASS(resizer, Media::Resizer::LanczosResizerH16_8(4, 3));
			console->WriteLineC(UTF8STRC("Resizer: LanczosResizer (16->8 LQ) H4V3 taps");
			writer->WriteLineC(UTF8STRC("Resizer: LanczosResizer (16->8 LQ) H4V3 taps");

			clk->Start();
			resizer->Resize(srcImg->data, srcImg->GetDataBpl(), OSInt2Double(imgWidth), OSInt2Double(imgHeight), 0, 0, newImg->data, newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (1st): t = ");
			Text::SBAppendF64(&sb, t);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			if (t * 30 > 10)
				cnt = 1 + (Int32)(10 / t);
			else
				cnt = 30;
			clk->Start();
			i = cnt;
			while (i-- > 0)
			{
				resizer->Resize(srcImg->data, srcImg->GetDataBpl(), OSInt2Double(imgWidth), OSInt2Double(imgHeight), 0, 0, newImg->data, newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			}
			t = clk->GetTimeDiff() / OSInt2Double(cnt);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (2nd): t = ");
			Text::SBAppendF64(&sb, t);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());
			DEL_CLASS(resizer);

			IO::Path::GetProcessFileName(sbuff);
			IO::Path::AppendPath(sbuff, csptr = L"LResizeH16_8.tif");
			NEW_CLASS(efs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			exporter.ExportFile(efs, csptr, imgList, 0);
			DEL_CLASS(efs);*/

			console->WriteLineC(UTF8STRC("CSConv: CSRGB16_LRGBC"));
			writer->WriteLineC(UTF8STRC("CSConv: CSRGB16_LRGBC"));
			NEW_CLASS(csconv, Media::CS::CSRGB16_LRGBC(64, Media::PF_LE_B16G16R16A16, false, &color, &color, 0));
			console->WriteLineC(UTF8STRC("Initialized"));

			clk->Start();
			csconv->ConvertV2(&srcImg->data, tmpBuff, srcImg->info->dispWidth, srcImg->info->dispHeight, srcImg->info->storeWidth, srcImg->info->storeHeight, (OSInt)srcImg->info->dispWidth << 3, Media::FT_NON_INTERLACE, Media::YCOFST_C_CENTER_LEFT);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("CSConv (1st): t = "));
			Text::SBAppendF64(&sb, t);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			if (t * 30 > 10)
				cnt = 1 + (UInt32)Double2Int32(10 / t);
			else
				cnt = 30;
			clk->Start();
			i = cnt;
			while (i-- > 0)
			{
				csconv->ConvertV2(&srcImg->data, tmpBuff, srcImg->info->dispWidth, srcImg->info->dispHeight, srcImg->info->storeWidth, srcImg->info->storeHeight, (OSInt)srcImg->info->dispWidth << 3, Media::FT_NON_INTERLACE, Media::YCOFST_C_CENTER_LEFT);
			}
			t = clk->GetTimeDiff() / UOSInt2Double(cnt);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("CSConv (2nd): t = "));
			Text::SBAppendF64(&sb, t);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());
			DEL_CLASS(csconv);

			sb.ClearStr();
			sb.AppendC(UTF8STRC("LRGB First Pixel: "));
			sb.AppendHexBuff(tmpBuff, 8, ' ', Text::LineBreakType::None);
			sb.AppendC(UTF8STRC(" ("));
			sb.AppendI32(ReadInt16(tmpBuff));
			sb.AppendC(UTF8STRC(")"));
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			sb.ClearStr();
			sb.AppendC(UTF8STRC("LRGB Second Pixel: "));
			sb.AppendHexBuff(&tmpBuff[8], 8, ' ', Text::LineBreakType::None);
			sb.AppendC(UTF8STRC(" ("));
			sb.AppendI32(ReadInt16(tmpBuff));
			sb.AppendC(UTF8STRC(")"));
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			console->WriteLineC(UTF8STRC("Resizer: LanczosResizer (LR->32) H4V3 taps, alpha"));
			writer->WriteLineC(UTF8STRC("Resizer: LanczosResizer (LR->32) H4V3 taps, alpha"));
			NEW_CLASS(resizer, Media::Resizer::LanczosResizerLR_C32(4, 3, &color, 0, Media::AT_ALPHA, 300, Media::PF_B8G8R8A8));
			console->WriteLineC(UTF8STRC("Initialized"));

			clk->Start();
			resizer->Resize(tmpBuff, (OSInt)imgWidth << 3, UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (1st): t = "));
			Text::SBAppendF64(&sb, t);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			if (t * 30 > 10)
				cnt = 1 + (UInt32)Double2Int32(10 / t);
			else
				cnt = 30;
			clk->Start();
			i = cnt;
			while (i-- > 0)
			{
				resizer->Resize(tmpBuff, (OSInt)imgWidth << 3, UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			}
			t = clk->GetTimeDiff() / UOSInt2Double(cnt);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (2nd): t = "));
			Text::SBAppendF64(&sb, t);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());
			DEL_CLASS(resizer);

#if defined(EXPORT_IMAGE)
			IO::Path::GetProcessFileName(sbuff);
			IO::Path::AppendPath(sbuff, csptr = (const UTF8Char*)"LResizeLR_C32A.tif");
			newImg->info->atype = Media::AT_ALPHA;
			NEW_CLASS(efs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			exporter.ExportFile(efs, csptr, imgList, 0);
			DEL_CLASS(efs);
#endif

			console->WriteLineC(UTF8STRC("Resizer: LanczosResizer (LR->32) H4V3 taps, no alpha"));
			writer->WriteLineC(UTF8STRC("Resizer: LanczosResizer (LR->32) H4V3 taps, no alpha"));
			NEW_CLASS(resizer, Media::Resizer::LanczosResizerLR_C32(4, 3, &color, 0, Media::AT_NO_ALPHA, 300, Media::PF_B8G8R8A8));
			console->WriteLineC(UTF8STRC("Initialized"));

			clk->Start();
			resizer->Resize(tmpBuff, (OSInt)imgWidth << 3, UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (1st): t = "));
			Text::SBAppendF64(&sb, t);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			if (t * 30 > 10)
				cnt = 1 + (UInt32)Double2Int32(10 / t);
			else
				cnt = 30;
			clk->Start();
			i = cnt;
			while (i-- > 0)
			{
				resizer->Resize(tmpBuff, (OSInt)imgWidth << 3, UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			}
			t = clk->GetTimeDiff() / UOSInt2Double(cnt);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (2nd): t = "));
			Text::SBAppendF64(&sb, t);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());
			DEL_CLASS(resizer);

#if defined(EXPORT_IMAGE)
			IO::Path::GetProcessFileName(sbuff);
			IO::Path::AppendPath(sbuff, csptr = (const UTF8Char*)"LResizeLR_C32NA.tif");
			newImg->info->atype = Media::AT_NO_ALPHA;
			NEW_CLASS(efs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			exporter.ExportFile(efs, csptr, imgList, 0);
			DEL_CLASS(efs);
#endif

			clk->Start();
			srcImg->To32bpp();
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("To 32bpp: t = "));
			Text::SBAppendF64(&sb, t);
			sb.AppendC(UTF8STRC(", size = "));
			sb.AppendUOSInt(imgWidth);
			sb.AppendC(UTF8STRC(" x "));
			sb.AppendUOSInt(imgHeight);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

#if defined(EXPORT_IMAGE)
			IO::Path::GetProcessFileName(sbuff);
			IO::Path::AppendPath(sbuff, csptr = (const UTF8Char*)"OriImage_32.tif");
			NEW_CLASS(efs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			imgList->RemoveImage(0, false);
			imgList->AddImage(srcImg, 0);
			exporter.ExportFile(efs, csptr, imgList, 0);
			imgList->RemoveImage(0, false);
			imgList->AddImage(newImg, 0);
			DEL_CLASS(efs);
#endif

			console->WriteLineC(UTF8STRC("Resizer: LanczosResizer (8->8) H4V3 taps, no alpha"));
			writer->WriteLineC(UTF8STRC("Resizer: LanczosResizer (8->8) H4V3 taps, no alpha"));
			NEW_CLASS(resizer, Media::Resizer::LanczosResizer8_C8(4, 3, &color, &color, 0, Media::AT_NO_ALPHA));
			console->WriteLineC(UTF8STRC("Initialized"));

			clk->Start();
			resizer->Resize(srcImg->data, (OSInt)srcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (1st): t = "));
			Text::SBAppendF64(&sb, t);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			if (t * 30 > 10)
				cnt = 1 + (UInt32)Double2Int32(10 / t);
			else
				cnt = 30;
			clk->Start();
			i = cnt;
			while (i-- > 0)
			{
				resizer->Resize(srcImg->data, (OSInt)srcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			}
			t = clk->GetTimeDiff() / UOSInt2Double(cnt);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (2nd): t = "));
			Text::SBAppendF64(&sb, t);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());
			DEL_CLASS(resizer);

#if defined(EXPORT_IMAGE)
			IO::Path::GetProcessFileName(sbuff);
			IO::Path::AppendPath(sbuff, csptr = (const UTF8Char*)"LResize8_C8NA.tif");
			newImg->info->atype = Media::AT_NO_ALPHA;
			NEW_CLASS(efs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			exporter.ExportFile(efs, csptr, imgList, 0);
			DEL_CLASS(efs);
#endif

			console->WriteLineC(UTF8STRC("Resizer: LanczosResizer (8->8) H4V3 taps, alpha"));
			writer->WriteLineC(UTF8STRC("Resizer: LanczosResizer (8->8) H4V3 taps, alpha"));
			NEW_CLASS(resizer, Media::Resizer::LanczosResizer8_C8(4, 3, &color, &color, 0, Media::AT_ALPHA));
			console->WriteLineC(UTF8STRC("Initialized"));

			clk->Start();
			resizer->Resize(srcImg->data, (OSInt)srcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (1st): t = "));
			Text::SBAppendF64(&sb, t);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			if (t * 30 > 10)
				cnt = 1 + (UInt32)Double2Int32(10 / t);
			else
				cnt = 30;
			clk->Start();
			i = cnt;
			while (i-- > 0)
			{
				resizer->Resize(srcImg->data, (OSInt)srcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			}
			t = clk->GetTimeDiff() / UOSInt2Double(cnt);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (2nd): t = "));
			Text::SBAppendF64(&sb, t);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());
			DEL_CLASS(resizer);

#if defined(EXPORT_IMAGE)
			IO::Path::GetProcessFileName(sbuff);
			IO::Path::AppendPath(sbuff, csptr = (const UTF8Char*)"LResize8_C8A.tif");
			newImg->info->atype = Media::AT_ALPHA;
			NEW_CLASS(efs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			exporter.ExportFile(efs, csptr, imgList, 0);
			DEL_CLASS(efs);
#endif

			console->WriteLineC(UTF8STRC("Resizer: LanczosResizer (8->8 LQ) H4V3 taps"));
			writer->WriteLineC(UTF8STRC("Resizer: LanczosResizer (8->8 LQ) H4V3 taps"));
			NEW_CLASS(resizer, Media::Resizer::LanczosResizerH8_8(4, 3, Media::AT_NO_ALPHA));
			console->WriteLineC(UTF8STRC("Initialized"));

			clk->Start();
			resizer->Resize(srcImg->data, (OSInt)srcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (1st): t = "));
			Text::SBAppendF64(&sb, t);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			if (t * 30 > 10)
				cnt = 1 + (UInt32)Double2Int32(10 / t);
			else
				cnt = 30;
			clk->Start();
			i = cnt;
			while (i-- > 0)
			{
				resizer->Resize(srcImg->data, (OSInt)srcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			}
			t = clk->GetTimeDiff() / UOSInt2Double(cnt);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (2nd): t = "));
			Text::SBAppendF64(&sb, t);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());
			DEL_CLASS(resizer);

#if defined(EXPORT_IMAGE)
			IO::Path::GetProcessFileName(sbuff);
			IO::Path::AppendPath(sbuff, csptr = (const UTF8Char*)"LResizeH8_8.tif");
			newImg->info->atype = Media::AT_NO_ALPHA;
			NEW_CLASS(efs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			exporter.ExportFile(efs, csptr, imgList, 0);
			DEL_CLASS(efs);
#endif

			console->WriteLineC(UTF8STRC("Resizer: NearestNeighbourResizer (32->32)"));
			writer->WriteLineC(UTF8STRC("Resizer: NearestNeighbourResizer (32->32)"));
			NEW_CLASS(resizer, Media::Resizer::NearestNeighbourResizer32_32());
			console->WriteLineC(UTF8STRC("Initialized"));

			clk->Start();
			resizer->Resize(srcImg->data, (OSInt)srcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (1st): t = "));
			Text::SBAppendF64(&sb, t);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			if (t * 30 > 10)
				cnt = 1 + (UInt32)Double2Int32(10 / t);
			else
				cnt = 30;
			clk->Start();
			i = cnt;
			while (i-- > 0)
			{
				resizer->Resize(srcImg->data, (OSInt)srcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			}
			t = clk->GetTimeDiff() / UOSInt2Double(cnt);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (2nd): t = "));
			Text::SBAppendF64(&sb, t);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());
			DEL_CLASS(resizer);

#if defined(EXPORT_IMAGE)
			IO::Path::GetProcessFileName(sbuff);
			IO::Path::AppendPath(sbuff, csptr = (const UTF8Char*)"NNResize32_32.tif");
			newImg->info->atype = Media::AT_NO_ALPHA;
			NEW_CLASS(efs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			exporter.ExportFile(efs, csptr, imgList, 0);
			DEL_CLASS(efs);
#endif

			clk->Start();
			DEL_CLASS(imgList);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Delete Image List: t = "));
			Text::SBAppendF64(&sb, t);
			sb.AppendC(UTF8STRC(", size = "));
			sb.AppendUOSInt(imgWidth >> 1);
			sb.AppendC(UTF8STRC(" x "));
			sb.AppendUOSInt(imgHeight >> 1);
			sb.AppendC(UTF8STRC(" 32 bpp"));
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			clk->Start();
			DEL_CLASS(srcImg);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Delete Image: t = "));
			Text::SBAppendF64(&sb, t);
			sb.AppendC(UTF8STRC(", size = "));
			sb.AppendUOSInt(imgWidth);
			sb.AppendC(UTF8STRC(" x "));
			sb.AppendUOSInt(imgHeight);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			MemFreeA(tmpBuff);
		}
	}
#endif
	DEL_CLASS(writer);

	console->WriteLine();
	console->WriteLineC(UTF8STRC("Do you want to upload result to server? (Y/N)"));
	while (true)
	{
		Int32 key = IO::Console::GetKey();
		if (key == 'n' || key == 'N')
		{
			break;
		}
		else if (key == 'y' || key == 'Y')
		{
			Bool allowRetry = false;
			UInt64 fileSize;
			console->WriteLineC(UTF8STRC("Uploading to server"));
			fileSize = fs->GetLength();
			if (fileSize <= 0)
			{
				console->WriteLineC(UTF8STRC("Error in getting file size"));
			}
			else
			{
				Net::SocketFactory *sockf;
				Net::HTTPClient *cli;
				UInt64 readSize;
				UInt8 *txtBuff = MemAlloc(UInt8, (UOSInt)fileSize);
				fs->SeekFromBeginning(0);
				if (fileSize != (readSize = fs->Read(txtBuff, fileSize)))
				{
					console->WriteLineC(UTF8STRC("Error in reading result file"));
				}
				else
				{
					NEW_CLASS(sockf, Net::OSSocketFactory(false));
					cli = Net::HTTPClient::CreateConnect(sockf, 0, (const UTF8Char*)"http://sswroom.no-ip.org:5080/benchmark/upload", Net::WebUtil::RequestMethod::HTTP_POST, false);
					cli->AddContentType(UTF8STRC("text/plain"));
					cli->AddContentLength(fileSize);
					cli->Write(txtBuff, fileSize);
					if (cli->GetRespStatus() == 200)
					{
						console->WriteLineC(UTF8STRC("Upload successfully"));
					}
					else
					{
						console->WriteLineC(UTF8STRC("Error in uploading file"));
						allowRetry = true;
					}
					DEL_CLASS(cli);
					DEL_CLASS(sockf);
				}
				MemFree(txtBuff);
			}
			if (!allowRetry)
			{
				break;
			}
			else
			{
				console->WriteLineC(UTF8STRC("Do you want to retry? (y/n)"));
			}
		}
	}
	DEL_CLASS(fs);

	DEL_CLASS(mainEvt);
	DEL_CLASS(threadEvt);
	DEL_CLASS(mut);
	DEL_CLASS(clk);
	DEL_CLASS(console);
	DEL_CLASS(exHdlr);
	return 0;
}
