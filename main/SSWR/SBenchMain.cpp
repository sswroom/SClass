#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/HashCreator.h"
#include "Core/Core.h"
#include "Data/ArrayListInt32.h"
#include "Data/ByteBuffer.h"
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
#include "Media/Resizer/LanczosResizer16_C8.h"
#include "Media/Resizer/LanczosResizerH8_8.h"
//#include "Media/Resizer/LanczosResizerH16_8.h"
#include "Media/Resizer/LanczosResizerLR_C16.h"
#include "Media/Resizer/LanczosResizerLR_C32.h"
#include "Media/Resizer/LanczosResizerRGB_C8.h"
#include "Media/Resizer/NearestNeighbourResizer32_32.h"
#include "Media/Resizer/NearestNeighbourResizer64_64.h"
#include "Net/HTTPClient.h"
#include "Net/OSSocketFactory.h"
#include "Sync/Event.h"
#include "Sync/Interlocked.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
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
NN<Sync::Mutex> mut;
Sync::Event *threadEvt;
Sync::Event *mainEvt;
Double threadT;

UInt32 __stdcall TestThread(AnyType userObj)
{
	threadT = clk->GetTimeDiff();
	clk->Start();
	mainEvt->Set();
	Sync::MutexUsage mutUsage(mut);
	Sync::SimpleThread::Sleep(500);
	clk->Start();
	mutUsage.EndUse();
	return 0;
}

Double HashTestSpeed(NN<Crypto::Hash::HashAlgorithm> hash)
{
	UInt8 hashVal[64];
	UInt8 *testBlock = MemAllocA(UInt8, 1048576);	
	OSInt i;
	MemClear(testBlock, 1048576);
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

UnsafeArray<UTF8Char> ByteDisp(UnsafeArray<UTF8Char> sbuff, UInt64 byteSize)
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

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Manage::ExceptionRecorder *exHdlr;
	IO::ConsoleWriter *console;

	NN<IO::FileStream> fs;
#if defined(EXPORT_IMAGE)
	IO::FileStream *efs;
	Text::CString cstr;
#endif
	Text::UTF8Writer *writer;
	IO::SystemInfo sysInfo;
	UInt64 memSize;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[512];
	UTF8Char u8buff[256];
	UTF16Char u16buff[32];
	UTF32Char u32buff[32];
	UnsafeArray<UTF8Char> sptr;
	Text::CStringNN nns;
	UOSInt i;
	UOSInt j;
	Double t;
	UOSInt threadCnt = Sync::ThreadUtil::GetThreadCnt();

	MemSetLogFile(UTF8STRCPTR("Memory.log"));
	NEW_CLASS(exHdlr, Manage::ExceptionRecorder(CSTR("SBench.log"), Manage::ExceptionRecorder::EA_CLOSE));
	NEW_CLASS(console, IO::ConsoleWriter());
	NEW_CLASS(clk, Manage::HiResClock());
	NEW_CLASSNN(mut, Sync::Mutex());
	NEW_CLASS(threadEvt, Sync::Event(true));
	NEW_CLASS(mainEvt, Sync::Event(true));

	IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::ReplaceExt(sbuff, UTF8STRC("txt"));
	NEW_CLASSNN(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(writer, Text::UTF8Writer(fs));

	console->WriteLine(CSTR("SBench Result:"));
	writer->WriteLine(CSTR("SBench Result:"));
	console->WriteLine(CSTR("Computer Info:"));
	writer->WriteLine(CSTR("Computer Info:"));
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Platform: "));
	if (sysInfo.GetPlatformName(sbuff).SetTo(sptr))
	{
		sb.AppendP(sbuff, sptr);
	}
	else
	{
		sb.AppendC(UTF8STRC("-"));
	}
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());

	{
		Manage::CPUInfoDetail cpuInfo;
		sb.ClearStr();
		sb.AppendC(UTF8STRC("CPU: "));
		if (cpuInfo.GetCPUName(u8buff).SetTo(sptr))
		{
			sb.AppendP(u8buff, sptr);
		}
		else
		{
			sb.AppendC(UTF8STRC("-"));
		}
		console->WriteLine(sb.ToCString());
		writer->WriteLine(sb.ToCString());

		Optional<const Manage::CPUDB::CPUSpec> cpuSpec = 0;
		NN<const Manage::CPUDB::CPUSpec> nncpuSpec;
		sb.ClearStr();
		sb.AppendC(UTF8STRC("CPU Model: "));
		if (cpuInfo.GetCPUModel().SetTo(nns))
		{
			sb.Append(nns);
			cpuSpec = Manage::CPUDB::GetCPUSpec(nns);
		}
		else
		{
			sb.AppendC(UTF8STRC("-"));
		}
		console->WriteLine(sb.ToCString());
		writer->WriteLine(sb.ToCString());

		sb.ClearStr();
		sb.AppendC(UTF8STRC("CPU Brand Name: "));
		if (cpuSpec.SetTo(nncpuSpec))
		{
			sb.Append(Manage::CPUVendor::GetBrandName(nncpuSpec->brand));
			sb.AppendC(UTF8STRC(" "));
			sb.AppendSlow((const UTF8Char*)nncpuSpec->name);
		}
		else
		{
			sb.AppendC(UTF8STRC("-"));
		}
		console->WriteLine(sb.ToCString());
		writer->WriteLine(sb.ToCString());

#if defined(CPU_X86_32) || defined(CPU_X86_64)
		sb.ClearStr();
		sb.AppendC(UTF8STRC("CPU FamilyId = "));
		sb.AppendI32(cpuInfo.GetFamilyId());
		sb.AppendC(UTF8STRC(", ModelId = "));
		sb.AppendI32(cpuInfo.GetModelId());
		sb.AppendC(UTF8STRC(", Stepping = "));
		sb.AppendI32(cpuInfo.GetStepping());
		console->WriteLine(sb.ToCString());
		writer->WriteLine(sb.ToCString());
#endif
	}

	sb.ClearStr();
	sb.AppendC(UTF8STRC("CPU Thread Count: "));
	sb.AppendUOSInt(threadCnt);
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());
	sb.ClearStr();
	sb.AppendC(UTF8STRC("OS: "));
	if (IO::OS::GetDistro(sbuff).SetTo(sptr))
	{
		sb.AppendP(sbuff, sptr);
	}
	else
	{
		sb.AppendC(UTF8STRC("Unknown"));
	}
	if (IO::OS::GetVersion(sbuff).SetTo(sptr))
	{
		sb.AppendC(UTF8STRC(" "));
		sb.AppendP(sbuff, sptr);
	}
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Build Time: "));
		sb.AppendTSNoZone(IO::BuildTime::GetBuildTime().ToUTCTime());
		console->WriteLine(sb.ToCString());
		writer->WriteLine(sb.ToCString());

		sb.ClearStr();
		sb.AppendC(UTF8STRC("Benchmark Time: "));
		sb.AppendTSNoZone(Data::Timestamp::UtcNow());
		console->WriteLine(sb.ToCString());
		writer->WriteLine(sb.ToCString());
	}

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Total Memory Size: "));
	memSize = sysInfo.GetTotalMemSize();
	sptr = ByteDisp(sbuff, memSize);
	sb.AppendP(sbuff, sptr);
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Total Usable Memory Size: "));
	memSize = sysInfo.GetTotalUsableMemSize();
	sptr = ByteDisp(sbuff, memSize);
	sb.AppendP(sbuff, sptr);
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());

	{
		Data::ArrayListNN<IO::SystemInfo::RAMInfo> ramList;
		NN<IO::SystemInfo::RAMInfo> ram;
		sysInfo.GetRAMInfo(ramList);
		i = 0;
		j = ramList.GetCount();
		while (i < j)
		{
			ram = ramList.GetItemNoCheck(i);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("RAM: "));
			sb.AppendOpt(ram->deviceLocator);
			sb.AppendC(UTF8STRC("\t"));
			sb.AppendOpt(ram->manufacturer);
			sb.AppendC(UTF8STRC("\t"));
			sb.AppendOpt(ram->partNo);
			sb.AppendC(UTF8STRC("\t"));
			sb.AppendOpt(ram->sn);
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
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());
			i++;
		}
		sysInfo.FreeRAMInfo(ramList);
	}

	{
		Data::ArrayListNN<Media::DDCReader> readerList;
		NN<Media::DDCReader> reader;
		Media::DDCReader::CreateDDCReaders(readerList);
		UOSInt edidSize;
		UnsafeArray<UInt8> edid;
		i = 0;
		j = readerList.GetCount();
		while (i < j)
		{
			reader = readerList.GetItemNoCheck(i);
			if (reader->GetEDID(edidSize).SetTo(edid))
			{
				Media::EDID::EDIDInfo edidInfo;
				if (Media::EDID::Parse(edid, edidInfo))
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
					console->WriteLine(sb.ToCString());
					writer->WriteLine(sb.ToCString());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Monitor Manufacture: Year "));
					sb.AppendI32(edidInfo.yearOfManu);
					sb.AppendC(UTF8STRC(" Week "));
					sb.AppendU32(edidInfo.weekOfManu);
					console->WriteLine(sb.ToCString());
					writer->WriteLine(sb.ToCString());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Monitor Size: "));
					sb.AppendDouble(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, Math_Sqrt(edidInfo.dispPhysicalW_mm * edidInfo.dispPhysicalW_mm + edidInfo.dispPhysicalH_mm * edidInfo.dispPhysicalH_mm)));
					sb.AppendC(UTF8STRC("\" ("));
					sb.AppendU32(edidInfo.dispPhysicalW_mm);
					sb.AppendC(UTF8STRC(" x "));
					sb.AppendU32(edidInfo.dispPhysicalH_mm);
					sb.AppendC(UTF8STRC("mm)"));
					console->WriteLine(sb.ToCString());
					writer->WriteLine(sb.ToCString());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Monitor Pixels: "));
					sb.AppendU32(edidInfo.pixelW);
					sb.AppendC(UTF8STRC(" x "));
					sb.AppendU32(edidInfo.pixelH);
					console->WriteLine(sb.ToCString());
					writer->WriteLine(sb.ToCString());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Monitor bit per color: "));
					sb.AppendU32(edidInfo.bitPerColor);
					console->WriteLine(sb.ToCString());
					writer->WriteLine(sb.ToCString());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Monitor White: x = "));
					sb.AppendDouble(edidInfo.w.x);
					sb.AppendC(UTF8STRC(", y = "));
					sb.AppendDouble(edidInfo.w.y);
					console->WriteLine(sb.ToCString());
					writer->WriteLine(sb.ToCString());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Monitor Red x = "));
					sb.AppendDouble(edidInfo.r.x);
					sb.AppendC(UTF8STRC(", y = "));
					sb.AppendDouble(edidInfo.r.y);
					console->WriteLine(sb.ToCString());
					writer->WriteLine(sb.ToCString());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Monitor Green: x = "));
					sb.AppendDouble(edidInfo.g.x);
					sb.AppendC(UTF8STRC(", y = "));
					sb.AppendDouble(edidInfo.g.y);
					console->WriteLine(sb.ToCString());
					writer->WriteLine(sb.ToCString());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Monitor Blue: x = "));
					sb.AppendDouble(edidInfo.b.x);
					sb.AppendC(UTF8STRC(", y = "));
					sb.AppendDouble(edidInfo.b.y);
					console->WriteLine(sb.ToCString());
					writer->WriteLine(sb.ToCString());
				}
			}
			reader.Delete();
			i++;
		}
	}

//-------------------------------------------------------------------------------
	console->WriteLine();
	writer->WriteLine();
	console->WriteLine(CSTR("API Performance:"));
	writer->WriteLine(CSTR("API Performance:"));

	clk->Start();
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Check Time: "));
	sb.AppendDouble(t);
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());

	clk->Start();
	i = 1000;
	while (i-- > 0)
	{
		Sync::ThreadUtil::GetThreadId();
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("GetThreadId: "));
	sb.AppendDouble(t / 1000.0);
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());

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
	sb.AppendDouble(t / 1000.0);
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());

	Int32 tmpVal = 0;
	clk->Start();
	i = 1000;
	while (i-- > 0)
	{
		Sync::Interlocked::IncrementI32(tmpVal);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Interlocked Increment: "));
	sb.AppendDouble(t / 1000.0);
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());

	clk->Start();
	i = 1000;
	while (i-- > 0)
	{
		mainEvt->Set();
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Event.Set: "));
	sb.AppendDouble(t / 1000.0);
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());

	mainEvt->Clear();
	clk->Start();
	mainEvt->Wait(100);
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Event.Wait(100): "));
	sb.AppendDouble(t);
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());

#if defined(TEST_THREAD)
	threadT = 0;
	clk->Start();
	Sync::ThreadUtil::Create(TestThread, 0);
	mainEvt->Wait(1000);
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Thread Create: "));
	sb.AppendDouble(threadT);
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Event Wake: "));
	sb.AppendDouble(t);
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());

	Sync::SimpleThread::Sleep(100);
	Sync::MutexUsage mutUsage(mut);
	t = clk->GetTimeDiff();
	mutUsage.EndUse();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Mutex Lock Release: "));
	sb.AppendDouble(t);
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());
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
	sb.AppendDouble(t / 1000.0);
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());

	clk->Start();
	i = 1000;
	while (i-- > 0)
	{
		MemFree(tmpArr[i]);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("MemFree: "));
	sb.AppendDouble(t / 1000.0);
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());

	clk->Start();
	i = 1000;
	while (i-- > 0)
	{
		tmpArr[i] = MemAllocA(UInt8, 16);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("MemAllocA: "));
	sb.AppendDouble(t / 1000.0);
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());

	clk->Start();
	i = 1000;
	while (i-- > 0)
	{
		MemFreeA(tmpArr[i]);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("MemFreeA: "));
	sb.AppendDouble(t / 1000.0);
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());

	MemFree(tmpArr);

//-------------------------------------------------------------------------------
	console->WriteLine();
	writer->WriteLine();
	console->WriteLine(CSTR("Implementation Performance:"));
	writer->WriteLine(CSTR("Implementation Performance:"));

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
	sb.AppendDouble(t);
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());

	clk->Start();
	i32V = 100000000;
	while (i32V-- > 0)
	{
		Text::StrInt32(u16buff, i32V);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("StrInt32 (UTF16 0 - 99999999): "));
	sb.AppendDouble(t);
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());

	clk->Start();
	i32V = 100000000;
	while (i32V-- > 0)
	{
		Text::StrInt32(u32buff, i32V);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("StrInt32 (UTF32 0 - 99999999): "));
	sb.AppendDouble(t);
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());

	clk->Start();
	i64V = 100100000000LL;
	while (i64V-- > 100000000000LL)
	{
		Text::StrInt64(u8buff, i64V);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("StrInt64 (UTF8 100000000000 - 100099999999): "));
	sb.AppendDouble(t);
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());

	clk->Start();
	i64V = 100100000000LL;
	while (i64V-- > 100000000000LL)
	{
		Text::StrInt64(u16buff, i64V);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("StrInt64 (UTF16 100000000000 - 100099999999): "));
	sb.AppendDouble(t);
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());

	clk->Start();
	i64V = 100100000000LL;
	while (i64V-- > 100000000000LL)
	{
		Text::StrInt64(u32buff, i64V);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("StrInt64 (UTF32 100000000000 - 100099999999): "));
	sb.AppendDouble(t);
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());

	clk->Start();
	i32V = 10000;
	while (i32V-- > 0)
	{
		Text::StrDouble(u8buff, Math::PI);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("StrDouble (UTF8 PI): "));
	sb.AppendDouble(t / 10000.0);
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());

	clk->Start();
	i32V = 10000;
	while (i32V-- > 0)
	{
		Text::StrDoubleW(u16buff, Math::PI);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("StrDouble (UTF16 PI): "));
	sb.AppendDouble(t / 10000.0);
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());

	clk->Start();
	i32V = 10000;
	while (i32V-- > 0)
	{
		Text::StrDoubleW(u32buff, Math::PI);
	}
	t = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("StrDouble (UTF32 PI): "));
	sb.AppendDouble(t / 10000.0);
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());

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
		sb.AppendDouble(t);
		console->WriteLine(sb.ToCString());
		writer->WriteLine(sb.ToCString());

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
		sb.AppendDouble(t);
		console->WriteLine(sb.ToCString());
		writer->WriteLine(sb.ToCString());

		clk->Start();
		i = 100000;
		while (i-- > 0)
		{
			arrList->SortedInsert(rand.NextInt32());
		}
		t = clk->GetTimeDiff();
		sb.ClearStr();
		sb.AppendC(UTF8STRC("ArrayListInt32.SortedInsert(rand) * 100000: "));
		sb.AppendDouble(t);
		console->WriteLine(sb.ToCString());
		writer->WriteLine(sb.ToCString());

		DEL_CLASS(arrList);
	}
#endif

#if defined(TEST_HASH)
	NN<Crypto::Hash::HashAlgorithm> hash;
	Crypto::Hash::HashType currHash = Crypto::Hash::HashType::First;
	while (currHash <= Crypto::Hash::HashType::Last)
	{
		if (Crypto::Hash::HashCreator::CreateHash(currHash).SetTo(hash))
		{
			t = HashTestSpeed(hash);
			sptr = hash->GetName(sbuff);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Hash "));
			sb.AppendP(sbuff, sptr);
			sb.AppendC(UTF8STRC(": "));
			sptr = Text::StrDoubleFmt(sbuff, t, "#,###.###");
			sb.AppendP(sbuff, sptr);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());

			hash.Delete();
		}
		currHash = (Crypto::Hash::HashType)((OSInt)currHash + 1);
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
		recordCnt = (UOSInt)(memSize >> 5);
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
	sb.AppendDouble(t);
	sb.AppendC(UTF8STRC("s"));
	console->WriteLine(sb.ToCString());
	writer->WriteLine(sb.ToCString());
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
		sb.AppendDouble(t);
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
		console->WriteLine(sb.ToCString());
		writer->WriteLine(sb.ToCString());

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
			sb.AppendUOSInt(Sync::ThreadUtil::GetThreadCnt());
			sb.AppendC(UTF8STRC(" Thread N = "));
			sb.AppendUOSInt(noOfRec);
			sb.AppendC(UTF8STRC(", Sort time = "));
			sb.AppendDouble(t);
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
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());

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
		sb.AppendDouble(t);
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
		console->WriteLine(sb.ToCString());
		writer->WriteLine(sb.ToCString());

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
		sb.AppendDouble(t);
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
		console->WriteLine(sb.ToCString());
		writer->WriteLine(sb.ToCString());

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
		sb.AppendDouble(t);
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
		console->WriteLine(sb.ToCString());
		writer->WriteLine(sb.ToCString());

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
			sb.AppendOSInt(Sync::ThreadUtil::GetThreadCnt());
			sb.AppendC(UTF8STRC(" Thread N = ");
			sb.AppendOSInt(noOfRec);
			sb.AppendC(UTF8STRC(", Sort time = ");
			sb.AppendDouble(t);
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
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());

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
		sb.AppendDouble(t);
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
		console->WriteLine(sb.ToCString());
		writer->WriteLine(sb.ToCString());

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
	console->WriteLine(CSTR("Memory Performance:"));
	writer->WriteLine(CSTR("Memory Performance:"));
	console->WriteLine(CSTR("Type\tSize\tData Rate\tLoop Count\tUsed Time"));
	writer->WriteLine(CSTR("Type\tSize\tData Rate\tLoop Count\tUsed Time"));

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
				sb.AppendDouble(t);
				console->WriteLine(sb.ToCString());
				writer->WriteLine(sb.ToCString());
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
				sb.AppendDouble(t);
				console->WriteLine(sb.ToCString());
				writer->WriteLine(sb.ToCString());
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
				sb.AppendDouble(t);
				console->WriteLine(sb.ToCString());
				writer->WriteLine(sb.ToCString());
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
				sb.AppendDouble(t);
				console->WriteLine(sb.ToCString());
				writer->WriteLine(sb.ToCString());
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
				sb.AppendDouble(t);
				console->WriteLine(sb.ToCString());
				writer->WriteLine(sb.ToCString());
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
				sb.AppendDouble(t);
				console->WriteLine(sb.ToCString());
				writer->WriteLine(sb.ToCString());
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
				sb.AppendDouble(t);
				console->WriteLine(sb.ToCString());
				writer->WriteLine(sb.ToCString());
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
		Optional<Media::StaticImage> srcImg;
		NN<Media::StaticImage> nnsrcImg;
		NN<Media::StaticImage> newImg;
		UInt8 *tmpBuff;
		Media::CS::CSConverter *csconv;

		console->WriteLine();
		writer->WriteLine();
		console->WriteLine(CSTR("Image Performance:"));
		writer->WriteLine(CSTR("Image Performance:"));
		imgWidth = 3840;
		imgHeight = 2160;
		while (imgWidth * imgHeight * 8 * 4 >= memSize)
		{
			imgWidth = imgWidth >> 1;
			imgHeight = imgHeight >> 1;
		}

		NEW_CLASS(imgGen, Media::ImageGen::RingsImageGen());

		clk->Start();
		srcImg = Optional<Media::StaticImage>::ConvertFrom(imgGen->GenerateImage(color, Math::Size2D<UOSInt>(imgWidth, imgHeight)));
		t = clk->GetTimeDiff();
		DEL_CLASS(imgGen);
		if (!srcImg.SetTo(nnsrcImg))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Generate Rings Image: Error, size = "));
			sb.AppendUOSInt(imgWidth);
			sb.AppendC(UTF8STRC(" x "));
			sb.AppendUOSInt(imgHeight);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());
		}
		else
		{
			Media::ImageResizer *resizer;
			UOSInt cnt;

			sb.ClearStr();
			sb.AppendC(UTF8STRC("Generate Rings Image: t = "));
			sb.AppendDouble(t);
			sb.AppendC(UTF8STRC(", size = "));
			sb.AppendUOSInt(imgWidth);
			sb.AppendC(UTF8STRC(" x "));
			sb.AppendUOSInt(imgHeight);
			sb.AppendC(UTF8STRC(" "));
			sb.AppendU32(nnsrcImg->info.storeBPP);
			sb.AppendC(UTF8STRC(" bpp"));
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());

			NEW_CLASS(imgList, Media::ImageList(CSTR("Temp")));
#if defined(EXPORT_IMAGE)
			sptr = IO::Path::GetProcessFileName(sbuff);
			sptr = IO::Path::AppendPath(sbuff, sptr, cstr = CSTR("RingsImage64.tif"));
			NEW_CLASS(efs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			imgList->AddImage(nnsrcImg, 0);
			exporter.ExportFile(efs, cstr, imgList, 0);
			imgList->RemoveImage(0, false);
			DEL_CLASS(efs);
#endif

			tmpBuff = MemAllocA(UInt8, imgWidth * imgHeight * 8);

			clk->Start();
			NEW_CLASSNN(newImg, Media::StaticImage(Math::Size2D<UOSInt>(imgWidth >> 1, imgHeight >> 1), 0, 64, Media::PF_LE_B16G16R16A16, 0, color, Media::ColorProfile::YUVT_BT709, Media::AT_IGNORE_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Create Image: t = "));
			sb.AppendDouble(t);
			sb.AppendC(UTF8STRC(", size = "));
			sb.AppendUOSInt(imgWidth >> 1);
			sb.AppendC(UTF8STRC(" x "));
			sb.AppendUOSInt(imgHeight >> 1);
			sb.AppendC(UTF8STRC(" 64 bpp"));
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());

			imgList->AddImage(newImg, 0);

			//Test Thread
/*			i = 100;
			while (i-- > 0)
			{
				NEW_CLASS(resizer, Media::Resizer::LanczosResizer16_C8(4, 3, &color, &color, 0, Media::AT_NO_ALPHA));
				DEL_CLASS(resizer);
			}*/

			console->WriteLine(CSTR("Resizer: NearestNeighbourResizer (64->64)"));
			writer->WriteLine(CSTR("Resizer: NearestNeighbourResizer (64->64)"));
			NEW_CLASS(resizer, Media::Resizer::NearestNeighbourResizer64_64());
			console->WriteLine(CSTR("Initialized"));

			clk->Start();
			resizer->Resize(nnsrcImg->data, (OSInt)nnsrcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (1st): t = "));
			sb.AppendDouble(t);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());

			if (t * 30 > 10)
				cnt = 1 + (UInt32)Double2Int32(10 / t);
			else
				cnt = 30;
			clk->Start();
			i = cnt;
			while (i-- > 0)
			{
				resizer->Resize(nnsrcImg->data, (OSInt)nnsrcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			}
			t = clk->GetTimeDiff() / UOSInt2Double(cnt);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (2nd): t = "));
			sb.AppendDouble(t);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());
			DEL_CLASS(resizer);

#if defined(EXPORT_IMAGE)
			sptr = IO::Path::GetProcessFileName(sbuff);
			sptr = IO::Path::AppendPath(sbuff, sptr, cstr = CSTR("NNResize64_64.tif"));
			NEW_CLASS(efs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			exporter.ExportFile(efs, cstr, imgList, 0);
			DEL_CLASS(efs);
#endif

			clk->Start();
			DEL_CLASS(imgList);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Delete Image List: t = "));
			sb.AppendDouble(t);
			sb.AppendC(UTF8STRC(", size = "));
			sb.AppendUOSInt(imgWidth >> 1);
			sb.AppendC(UTF8STRC(" x "));
			sb.AppendUOSInt(imgHeight >> 1);
			sb.AppendC(UTF8STRC(" 64 bpp"));
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());

			clk->Start();
			NEW_CLASSNN(newImg, Media::StaticImage(Math::Size2D<UOSInt>(imgWidth >> 1, imgHeight >> 1), 0, 32, Media::PF_B8G8R8A8, 0, color, Media::ColorProfile::YUVT_BT709, Media::AT_IGNORE_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Create Image: t = "));
			sb.AppendDouble(t);
			sb.AppendC(UTF8STRC(", size = "));
			sb.AppendUOSInt(imgWidth >> 1);
			sb.AppendC(UTF8STRC(" x "));
			sb.AppendUOSInt(imgHeight >> 1);
			sb.AppendC(UTF8STRC(" 32 bpp"));
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());

			NEW_CLASS(imgList, Media::ImageList(CSTR("Temp")));
			imgList->AddImage(newImg, 0);

			console->WriteLine(CSTR("Resizer: LanczosResizer (16->8) H4V3 taps, no alpha"));
			writer->WriteLine(CSTR("Resizer: LanczosResizer (16->8) H4V3 taps, no alpha"));
			NEW_CLASS(resizer, Media::Resizer::LanczosResizer16_C8(4, 3, color, color, 0, Media::AT_IGNORE_ALPHA));
			console->WriteLine(CSTR("Initialized"));

			clk->Start();
			resizer->Resize(nnsrcImg->data, (OSInt)nnsrcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (1st): t = "));
			sb.AppendDouble(t);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());

			if (t * 30 > 10)
				cnt = 1 + (UInt32)Double2Int32(10 / t);
			else
				cnt = 30;
			clk->Start();
			i = cnt;
			while (i-- > 0)
			{
				resizer->Resize(nnsrcImg->data, (OSInt)nnsrcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			}
			t = clk->GetTimeDiff() / UOSInt2Double(cnt);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (2nd): t = "));
			sb.AppendDouble(t);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());
			DEL_CLASS(resizer);

#if defined(EXPORT_IMAGE)
			sptr = IO::Path::GetProcessFileName(sbuff);
			sptr = IO::Path::AppendPath(sbuff, sptr, cstr = CSTR("LResize16_C8NA.tif"));
			NEW_CLASS(efs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			exporter.ExportFile(efs, cstr, imgList, 0);
			DEL_CLASS(efs);
#endif

			console->WriteLine(CSTR("Resizer: LanczosResizer (16->8) H4V3 taps, alpha"));
			writer->WriteLine(CSTR("Resizer: LanczosResizer (16->8) H4V3 taps, alpha"));
			NEW_CLASS(resizer, Media::Resizer::LanczosResizer16_C8(4, 3, color, color, 0, Media::AT_ALPHA));
			console->WriteLine(CSTR("Initialized"));

			clk->Start();
			resizer->Resize(nnsrcImg->data, (OSInt)nnsrcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (1st): t = "));
			sb.AppendDouble(t);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());

			if (t * 30 > 10)
				cnt = 1 + (UInt32)Double2Int32(10 / t);
			else
				cnt = 30;
			clk->Start();
			i = cnt;
			while (i-- > 0)
			{
				resizer->Resize(nnsrcImg->data, (OSInt)nnsrcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			}
			t = clk->GetTimeDiff() / UOSInt2Double(cnt);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (2nd): t = "));
			sb.AppendDouble(t);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());
			DEL_CLASS(resizer);

#if defined(EXPORT_IMAGE)
			sptr = IO::Path::GetProcessFileName(sbuff);
			sptr = IO::Path::AppendPath(sbuff, sptr, cstr = CSTR("LResize16_C8A.tif"));
			NEW_CLASS(efs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			exporter.ExportFile(efs, cstr, imgList, 0);
			DEL_CLASS(efs);
#endif

/*			NEW_CLASS(resizer, Media::Resizer::LanczosResizerH16_8(4, 3));
			console->WriteLine(CSTR("Resizer: LanczosResizer (16->8 LQ) H4V3 taps");
			writer->WriteLine(CSTR("Resizer: LanczosResizer (16->8 LQ) H4V3 taps");

			clk->Start();
			resizer->Resize(srcImg->data, srcImg->GetDataBpl(), OSInt2Double(imgWidth), OSInt2Double(imgHeight), 0, 0, newImg->data, newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (1st): t = ");
			sb.AppendDouble(t);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());

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
			sb.AppendDouble(t);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());
			DEL_CLASS(resizer);

			IO::Path::GetProcessFileName(sbuff);
			IO::Path::AppendPath(sbuff, csptr = L"LResizeH16_8.tif");
			NEW_CLASS(efs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			exporter.ExportFile(efs, csptr, imgList, 0);
			DEL_CLASS(efs);*/

			console->WriteLine(CSTR("CSConv: CSRGB16_LRGBC"));
			writer->WriteLine(CSTR("CSConv: CSRGB16_LRGBC"));
			NEW_CLASS(csconv, Media::CS::CSRGB16_LRGBC(64, Media::PF_LE_B16G16R16A16, false, color, color, 0));
			console->WriteLine(CSTR("Initialized"));

			clk->Start();
			csconv->ConvertV2(&nnsrcImg->data, tmpBuff, nnsrcImg->info.dispSize.x, nnsrcImg->info.dispSize.y, nnsrcImg->info.storeSize.x, nnsrcImg->info.storeSize.y, (OSInt)nnsrcImg->info.dispSize.x << 3, Media::FT_NON_INTERLACE, Media::YCOFST_C_CENTER_LEFT);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("CSConv (1st): t = "));
			sb.AppendDouble(t);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());

			if (t * 30 > 10)
				cnt = 1 + (UInt32)Double2Int32(10 / t);
			else
				cnt = 30;
			clk->Start();
			i = cnt;
			while (i-- > 0)
			{
				csconv->ConvertV2(&nnsrcImg->data, tmpBuff, nnsrcImg->info.dispSize.x, nnsrcImg->info.dispSize.y, nnsrcImg->info.storeSize.x, nnsrcImg->info.storeSize.y, (OSInt)nnsrcImg->info.dispSize.x << 3, Media::FT_NON_INTERLACE, Media::YCOFST_C_CENTER_LEFT);
			}
			t = clk->GetTimeDiff() / UOSInt2Double(cnt);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("CSConv (2nd): t = "));
			sb.AppendDouble(t);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());
			DEL_CLASS(csconv);

			sb.ClearStr();
			sb.AppendC(UTF8STRC("LRGB First Pixel: "));
			sb.AppendHexBuff(tmpBuff, 8, ' ', Text::LineBreakType::None);
			sb.AppendC(UTF8STRC(" ("));
			sb.AppendI32(ReadInt16(tmpBuff));
			sb.AppendC(UTF8STRC(")"));
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());

			sb.ClearStr();
			sb.AppendC(UTF8STRC("LRGB Second Pixel: "));
			sb.AppendHexBuff(&tmpBuff[8], 8, ' ', Text::LineBreakType::None);
			sb.AppendC(UTF8STRC(" ("));
			sb.AppendI32(ReadInt16(tmpBuff));
			sb.AppendC(UTF8STRC(")"));
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());

			console->WriteLine(CSTR("Resizer: LanczosResizer (LR->32) H4V3 taps, alpha"));
			writer->WriteLine(CSTR("Resizer: LanczosResizer (LR->32) H4V3 taps, alpha"));
			NEW_CLASS(resizer, Media::Resizer::LanczosResizerLR_C32(4, 3, color, 0, Media::AT_ALPHA, 300, Media::PF_B8G8R8A8));
			console->WriteLine(CSTR("Initialized"));

			clk->Start();
			resizer->Resize(tmpBuff, (OSInt)imgWidth << 3, UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (1st): t = "));
			sb.AppendDouble(t);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());

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
			sb.AppendDouble(t);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());
			DEL_CLASS(resizer);

#if defined(EXPORT_IMAGE)
			sptr = IO::Path::GetProcessFileName(sbuff);
			sptr = IO::Path::AppendPath(sbuff, sptr, cstr = CSTR("LResizeLR_C32A.tif"));
			newImg->info.atype = Media::AT_ALPHA;
			NEW_CLASS(efs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			exporter.ExportFile(efs, cstr, imgList, 0);
			DEL_CLASS(efs);
#endif

			console->WriteLine(CSTR("Resizer: LanczosResizer (LR->32) H4V3 taps, no alpha"));
			writer->WriteLine(CSTR("Resizer: LanczosResizer (LR->32) H4V3 taps, no alpha"));
			NEW_CLASS(resizer, Media::Resizer::LanczosResizerLR_C32(4, 3, color, 0, Media::AT_IGNORE_ALPHA, 300, Media::PF_B8G8R8A8));
			console->WriteLine(CSTR("Initialized"));

			clk->Start();
			resizer->Resize(tmpBuff, (OSInt)imgWidth << 3, UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (1st): t = "));
			sb.AppendDouble(t);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());

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
			sb.AppendDouble(t);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());
			DEL_CLASS(resizer);

#if defined(EXPORT_IMAGE)
			sptr = IO::Path::GetProcessFileName(sbuff);
			sptr = IO::Path::AppendPath(sbuff, sptr, cstr = CSTR("LResizeLR_C32NA.tif"));
			newImg->info.atype = Media::AT_NO_ALPHA;
			NEW_CLASS(efs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			exporter.ExportFile(efs, cstr, imgList, 0);
			DEL_CLASS(efs);
#endif

			clk->Start();
			nnsrcImg->ToB8G8R8A8();
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("To 32bpp: t = "));
			sb.AppendDouble(t);
			sb.AppendC(UTF8STRC(", size = "));
			sb.AppendUOSInt(imgWidth);
			sb.AppendC(UTF8STRC(" x "));
			sb.AppendUOSInt(imgHeight);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());

#if defined(EXPORT_IMAGE)
			sptr = IO::Path::GetProcessFileName(sbuff);
			sptr = IO::Path::AppendPath(sbuff, sptr, cstr = CSTR("OriImage_32.tif"));
			NEW_CLASS(efs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			imgList->RemoveImage(0, false);
			imgList->AddImage(nnsrcImg, 0);
			exporter.ExportFile(efs, cstr, imgList, 0);
			imgList->RemoveImage(0, false);
			imgList->AddImage(newImg, 0);
			DEL_CLASS(efs);
#endif

			console->WriteLine(CSTR("Resizer: LanczosResizer (8->8) H4V3 taps, no alpha"));
			writer->WriteLine(CSTR("Resizer: LanczosResizer (8->8) H4V3 taps, no alpha"));
			NEW_CLASS(resizer, Media::Resizer::LanczosResizerRGB_C8(4, 3, color, color, 0, Media::AT_IGNORE_ALPHA));
			console->WriteLine(CSTR("Initialized"));

			clk->Start();
			resizer->Resize(nnsrcImg->data, (OSInt)nnsrcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (1st): t = "));
			sb.AppendDouble(t);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());

			if (t * 30 > 10)
				cnt = 1 + (UInt32)Double2Int32(10 / t);
			else
				cnt = 30;
			clk->Start();
			i = cnt;
			while (i-- > 0)
			{
				resizer->Resize(nnsrcImg->data, (OSInt)nnsrcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			}
			t = clk->GetTimeDiff() / UOSInt2Double(cnt);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (2nd): t = "));
			sb.AppendDouble(t);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());
			DEL_CLASS(resizer);

#if defined(EXPORT_IMAGE)
			sptr = IO::Path::GetProcessFileName(sbuff);
			sptr = IO::Path::AppendPath(sbuff, sptr, cstr = CSTR("LResize8_C8NA.tif"));
			newImg->info.atype = Media::AT_NO_ALPHA;
			NEW_CLASS(efs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			exporter.ExportFile(efs, cstr, imgList, 0);
			DEL_CLASS(efs);
#endif

			console->WriteLine(CSTR("Resizer: LanczosResizer (8->8) H4V3 taps, alpha"));
			writer->WriteLine(CSTR("Resizer: LanczosResizer (8->8) H4V3 taps, alpha"));
			NEW_CLASS(resizer, Media::Resizer::LanczosResizerRGB_C8(4, 3, color, color, 0, Media::AT_ALPHA));
			console->WriteLine(CSTR("Initialized"));

			clk->Start();
			resizer->Resize(nnsrcImg->data, (OSInt)nnsrcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (1st): t = "));
			sb.AppendDouble(t);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());

			if (t * 30 > 10)
				cnt = 1 + (UInt32)Double2Int32(10 / t);
			else
				cnt = 30;
			clk->Start();
			i = cnt;
			while (i-- > 0)
			{
				resizer->Resize(nnsrcImg->data, (OSInt)nnsrcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			}
			t = clk->GetTimeDiff() / UOSInt2Double(cnt);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (2nd): t = "));
			sb.AppendDouble(t);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());
			DEL_CLASS(resizer);

#if defined(EXPORT_IMAGE)
			sptr = IO::Path::GetProcessFileName(sbuff);
			sptr = IO::Path::AppendPath(sbuff, sptr, cstr = CSTR("LResize8_C8A.tif"));
			newImg->info.atype = Media::AT_ALPHA;
			NEW_CLASS(efs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			exporter.ExportFile(efs, cstr, imgList, 0);
			DEL_CLASS(efs);
#endif

			console->WriteLine(CSTR("Resizer: LanczosResizer (8->8 LQ) H4V3 taps"));
			writer->WriteLine(CSTR("Resizer: LanczosResizer (8->8 LQ) H4V3 taps"));
			NEW_CLASS(resizer, Media::Resizer::LanczosResizerH8_8(4, 3, Media::AT_IGNORE_ALPHA));
			console->WriteLine(CSTR("Initialized"));

			clk->Start();
			resizer->Resize(nnsrcImg->data, (OSInt)nnsrcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (1st): t = "));
			sb.AppendDouble(t);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());

			if (t * 30 > 10)
				cnt = 1 + (UInt32)Double2Int32(10 / t);
			else
				cnt = 30;
			clk->Start();
			i = cnt;
			while (i-- > 0)
			{
				resizer->Resize(nnsrcImg->data, (OSInt)nnsrcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			}
			t = clk->GetTimeDiff() / UOSInt2Double(cnt);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (2nd): t = "));
			sb.AppendDouble(t);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());
			DEL_CLASS(resizer);

#if defined(EXPORT_IMAGE)
			sptr = IO::Path::GetProcessFileName(sbuff);
			sptr = IO::Path::AppendPath(sbuff, sptr, cstr = CSTR("LResizeH8_8.tif"));
			newImg->info.atype = Media::AT_NO_ALPHA;
			NEW_CLASS(efs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			exporter.ExportFile(efs, cstr, imgList, 0);
			DEL_CLASS(efs);
#endif

			console->WriteLine(CSTR("Resizer: NearestNeighbourResizer (32->32)"));
			writer->WriteLine(CSTR("Resizer: NearestNeighbourResizer (32->32)"));
			NEW_CLASS(resizer, Media::Resizer::NearestNeighbourResizer32_32());
			console->WriteLine(CSTR("Initialized"));

			clk->Start();
			resizer->Resize(nnsrcImg->data, (OSInt)nnsrcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (1st): t = "));
			sb.AppendDouble(t);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());

			if (t * 30 > 10)
				cnt = 1 + (UInt32)Double2Int32(10 / t);
			else
				cnt = 30;
			clk->Start();
			i = cnt;
			while (i-- > 0)
			{
				resizer->Resize(nnsrcImg->data, (OSInt)nnsrcImg->GetDataBpl(), UOSInt2Double(imgWidth), UOSInt2Double(imgHeight), 0, 0, newImg->data, (OSInt)newImg->GetDataBpl(), imgWidth >> 1, imgHeight >> 1);
			}
			t = clk->GetTimeDiff() / UOSInt2Double(cnt);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Resize (2nd): t = "));
			sb.AppendDouble(t);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());
			DEL_CLASS(resizer);

#if defined(EXPORT_IMAGE)
			sptr = IO::Path::GetProcessFileName(sbuff);
			sptr = IO::Path::AppendPath(sbuff, sptr, cstr = CSTR("NNResize32_32.tif"));
			newImg->info.atype = Media::AT_NO_ALPHA;
			NEW_CLASS(efs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			exporter.ExportFile(efs, cstr, imgList, 0);
			DEL_CLASS(efs);
#endif

			clk->Start();
			DEL_CLASS(imgList);
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Delete Image List: t = "));
			sb.AppendDouble(t);
			sb.AppendC(UTF8STRC(", size = "));
			sb.AppendUOSInt(imgWidth >> 1);
			sb.AppendC(UTF8STRC(" x "));
			sb.AppendUOSInt(imgHeight >> 1);
			sb.AppendC(UTF8STRC(" 32 bpp"));
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());

			clk->Start();
			srcImg.Delete();
			t = clk->GetTimeDiff();
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Delete Image: t = "));
			sb.AppendDouble(t);
			sb.AppendC(UTF8STRC(", size = "));
			sb.AppendUOSInt(imgWidth);
			sb.AppendC(UTF8STRC(" x "));
			sb.AppendUOSInt(imgHeight);
			console->WriteLine(sb.ToCString());
			writer->WriteLine(sb.ToCString());

			MemFreeA(tmpBuff);
		}
	}
#endif
	DEL_CLASS(writer);

	console->WriteLine();
	console->WriteLine(CSTR("Do you want to upload result to server? (Y/N)"));
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
			console->WriteLine(CSTR("Uploading to server"));
			fileSize = fs->GetLength();
			if (fileSize <= 0)
			{
				console->WriteLine(CSTR("Error in getting file size"));
			}
			else
			{
				NN<Net::HTTPClient> cli;
				UInt64 readSize;
				Data::ByteBuffer txtBuff((UOSInt)fileSize);
				fs->SeekFromBeginning(0);
				if (fileSize != (readSize = fs->Read(txtBuff)))
				{
					console->WriteLine(CSTR("Error in reading result file"));
				}
				else
				{
					Net::OSSocketFactory sockf(false);
					Net::TCPClientFactory clif(sockf);
					cli = Net::HTTPClient::CreateConnect(clif, 0, CSTR("http://sswroom.no-ip.org:5080/benchmark/upload"), Net::WebUtil::RequestMethod::HTTP_POST, false);
					cli->AddContentType(CSTR("text/plain"));
					cli->AddContentLength(fileSize);
					cli->Write(txtBuff.WithSize((UOSInt)fileSize));
					if (cli->GetRespStatus() == 200)
					{
						console->WriteLine(CSTR("Upload successfully"));
					}
					else
					{
						console->WriteLine(CSTR("Error in uploading file"));
						allowRetry = true;
					}
					cli.Delete();
				}
			}
			if (!allowRetry)
			{
				break;
			}
			else
			{
				console->WriteLine(CSTR("Do you want to retry? (y/n)"));
			}
		}
	}
	fs.Delete();

	DEL_CLASS(mainEvt);
	DEL_CLASS(threadEvt);
	mut.Delete();
	DEL_CLASS(clk);
	DEL_CLASS(console);
	DEL_CLASS(exHdlr);
	return 0;
}
