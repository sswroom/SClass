#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/SystemInfo.h"
#include "Manage/Benchmark.h"
#include "Manage/CPUInfo.h"
#include "Manage/HiResClock.h"
#include "SSWR/AVIRead/AVIRBenchmarkForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"
#include "UI/GUIFileDialog.h"

UnsafeArray<UTF8Char> SSWR::AVIRead::AVIRBenchmarkForm::ByteDisp(UnsafeArray<UTF8Char> sbuff, UOSInt byteSize)
{
	if (byteSize >= 1048576)
	{
		return Text::StrConcatC(Text::StrUOSInt(sbuff, byteSize >> 20), UTF8STRC("MB"));
	}
	else if (byteSize >= 1024)
	{
		return Text::StrConcatC(Text::StrUOSInt(sbuff, byteSize >> 10), UTF8STRC("KB"));
	}
	else
	{
		return Text::StrConcatC(Text::StrUOSInt(sbuff, byteSize), UTF8STRC("B"));
	}
}

void SSWR::AVIRead::AVIRBenchmarkForm::StartTest(UOSInt startSize, UOSInt buffSize)
{
	UOSInt i;
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	Double oriT;
	Double t;
	Manage::HiResClock clk;
	UInt8 *buff1;
	UInt8 *buff2;
	UOSInt loopCnt;
	UOSInt currSize;
	NN<SSWR::AVIRead::AVIRBenchmarkForm::TestResult> result;

	UOSInt testCnt = 0;
	currSize = startSize;
	while (currSize <= buffSize)
	{
		testCnt += 3;
		currSize = currSize << 1;
	}
	this->processing = true;
	this->pbMain->ProgressStart(CSTR("Testing"), testCnt);
	testCnt = 0;

	this->ClearResult();
	this->lvCopy->ClearItems();
	this->lvRead->ClearItems();
	this->lvWrite->ClearItems();
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

				sptr = ByteDisp(sbuff, currSize);
				i = this->lvCopy->AddItem(CSTRP(sbuff, sptr), 0);
				sptr = Text::StrDoubleFmt(sbuff, rate, "0.0");
				this->lvCopy->SetSubItem(i, 1, CSTRP(sbuff, sptr));

				result = MemAllocNN(SSWR::AVIRead::AVIRBenchmarkForm::TestResult);
				result->tt = TT_COPY;
				result->testSize = currSize;
				result->resultRate = rate;
				this->resultList.Add(result);
				break;
			}
			else
			{
				loopCnt = loopCnt << 1;
			}
		}
		testCnt++;
		this->pbMain->ProgressUpdate(testCnt, testCnt);
		this->ui->ProcessMessages();
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

				sptr = ByteDisp(sbuff, currSize);
				i = this->lvWrite->AddItem(CSTRP(sbuff, sptr), 0);
				sptr = Text::StrDoubleFmt(sbuff, rate, "0.0");
				this->lvWrite->SetSubItem(i, 1, CSTRP(sbuff, sptr));

				result = MemAllocNN(SSWR::AVIRead::AVIRBenchmarkForm::TestResult);
				result->tt = TT_WRITE;
				result->testSize = currSize;
				result->resultRate = rate;
				this->resultList.Add(result);
				break;
			}
			else
			{
				loopCnt = loopCnt << 1;
			}
		}
		testCnt++;
		this->pbMain->ProgressUpdate(testCnt, testCnt);
		this->ui->ProcessMessages();
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

				sptr = ByteDisp(sbuff, currSize);
				i = this->lvRead->AddItem(CSTRP(sbuff, sptr), 0);
				sptr = Text::StrDoubleFmt(sbuff, rate, "0.0");
				this->lvRead->SetSubItem(i, 1, CSTRP(sbuff, sptr));

				result = MemAllocNN(SSWR::AVIRead::AVIRBenchmarkForm::TestResult);
				result->tt = TT_READ;
				result->testSize = currSize;
				result->resultRate = rate;
				this->resultList.Add(result);
				break;
			}
			else
			{
				loopCnt = loopCnt << 1;
			}
		}
		testCnt++;
		this->pbMain->ProgressUpdate(testCnt, testCnt);
		this->ui->ProcessMessages();
		currSize = currSize << 1;
	}

	this->pbMain->ProgressEnd();
	this->processing = false;

	MemFreeA64(buff1);
	MemFreeA64(buff2);
}

void SSWR::AVIRead::AVIRBenchmarkForm::ClearResult()
{
	this->resultList.MemFreeAll();
}

void __stdcall SSWR::AVIRead::AVIRBenchmarkForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRBenchmarkForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBenchmarkForm>();
	if (me->processing)
		return;
	me->StartTest(128, 64 << 20); //64 MB
}

void __stdcall SSWR::AVIRead::AVIRBenchmarkForm::OnQuickClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRBenchmarkForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBenchmarkForm>();
	if (me->processing)
		return;
	me->StartTest(64 << 20, 64 << 20); //64 MB
}

void __stdcall SSWR::AVIRead::AVIRBenchmarkForm::OnSaveClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRBenchmarkForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBenchmarkForm>();
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	NN<SSWR::AVIRead::AVIRBenchmarkForm::TestResult> result;

	if (me->processing)
		return;

	if (me->resultList.GetCount() <= 0)
	{
		me->ui->ShowMsgOK(CSTR("No result"), CSTR("Error"), me);
		return;
	}
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"BenchmarkSave", true);
	dlg->AddFilter(CSTR("*.txt"), CSTR("Result File"));
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Benchmark_"));
		me->txtPlatform->GetText(sb);
		sb.AppendC(UTF8STRC(".txt"));
		dlg->SetFileName(sb.ToCString());
	}
	if (dlg->ShowDialog(me->GetHandle()))
	{
		Text::StringBuilderUTF8 sb;
		IO::SystemInfo sysInfo;
		IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		Text::UTF8Writer writer(fs);
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
		writer.WriteLine(sb.ToCString());

		Manage::CPUInfo cpu;
		sb.ClearStr();
		sb.AppendC(UTF8STRC("CPU: "));
		if (cpu.GetCPUName(sbuff).SetTo(sptr))
		{
			sb.AppendP(sbuff, sptr);
		}
		else
		{
			sb.AppendC(UTF8STRC("-"));
		}
		writer.WriteLine(sb.ToCString());

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
			writer.WriteLine(sb.ToCString());
			i++;
		}
		sysInfo.FreeRAMInfo(ramList);

		writer.WriteLine(CSTR("Result:"));
		i = 0;
		j = me->resultList.GetCount();
		while (i < j)
		{
			result = me->resultList.GetItemNoCheck(i);
			sb.ClearStr();
			switch (result->tt)
			{
			case TT_COPY:
				sb.AppendC(UTF8STRC("Copy"));
				break;
			case TT_WRITE:
				sb.AppendC(UTF8STRC("Write"));
				break;
			case TT_READ:
				sb.AppendC(UTF8STRC("Read"));
				break;
			default:
				sb.AppendC(UTF8STRC("Unknown"));
				break;
			}
			sb.AppendC(UTF8STRC("\t"));
			sb.AppendUOSInt(result->testSize);
			sb.AppendC(UTF8STRC("\t"));
			sb.AppendDouble(result->resultRate);
			writer.WriteLine(sb.ToCString());
			i++;
		}
	}
	dlg.Delete();
}

SSWR::AVIRead::AVIRBenchmarkForm::AVIRBenchmarkForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 480, ui)
{
	this->SetText(CSTR("Benchmark"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->processing = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlCtrl = ui->NewPanel(*this);
	this->pnlCtrl->SetRect(0, 0, 100, 32, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnStart = ui->NewButton(this->pnlCtrl, CSTR("Start"));
	this->btnStart->SetRect(4, 4, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->btnQuick = ui->NewButton(this->pnlCtrl, CSTR("Quick"));
	this->btnQuick->SetRect(84, 4, 75, 23, false);
	this->btnQuick->HandleButtonClick(OnQuickClicked, this);
	this->btnSave = ui->NewButton(this->pnlCtrl, CSTR("Save"));
	this->btnSave->SetRect(164, 4, 75, 23, false);
	this->btnSave->HandleButtonClick(OnSaveClicked, this);
	this->pbMain = ui->NewProgressBar(*this, 100);
	this->pbMain->SetRect(0, 0, 100, 24, false);
	this->pbMain->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpCopy = this->tcMain->AddTabPage(CSTR("Copy"));
	this->lvCopy = ui->NewListView(this->tpCopy, UI::ListViewStyle::Table, 2);
	this->lvCopy->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvCopy->SetShowGrid(true);
	this->lvCopy->SetFullRowSelect(true);
	this->lvCopy->AddColumn(CSTR("Buffer Size"), 200);
	this->lvCopy->AddColumn(CSTR("Data Rate"), 150);

	this->tpRead = this->tcMain->AddTabPage(CSTR("Read"));
	this->lvRead = ui->NewListView(this->tpRead, UI::ListViewStyle::Table, 2);
	this->lvRead->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvRead->SetShowGrid(true);
	this->lvRead->SetFullRowSelect(true);
	this->lvRead->AddColumn(CSTR("Buffer Size"), 200);
	this->lvRead->AddColumn(CSTR("Data Rate"), 150);

	this->tpWrite = this->tcMain->AddTabPage(CSTR("Write"));
	this->lvWrite = ui->NewListView(this->tpWrite, UI::ListViewStyle::Table, 2);
	this->lvWrite->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvWrite->SetShowGrid(true);
	this->lvWrite->SetFullRowSelect(true);
	this->lvWrite->AddColumn(CSTR("Buffer Size"), 200);
	this->lvWrite->AddColumn(CSTR("Data Rate"), 150);

	this->tpInfo = this->tcMain->AddTabPage(CSTR("Info"));
	this->pnlInfo = ui->NewPanel(this->tpInfo);
	this->pnlInfo->SetRect(0, 0, 100, 52, false);
	this->pnlInfo->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblPlatform = ui->NewLabel(this->pnlInfo, CSTR("Platform"));
	this->lblPlatform->SetRect(4, 4, 100, 23, false);
	this->txtPlatform = ui->NewTextBox(this->pnlInfo, CSTR(""));
	this->txtPlatform->SetRect(104, 4, 300, 23, false);
	this->txtPlatform->SetReadOnly(true);
	this->lblCPU = ui->NewLabel(this->pnlInfo, CSTR("CPU"));
	this->lblCPU->SetRect(4, 28, 100, 23, false);
	this->txtCPU = ui->NewTextBox(this->pnlInfo, CSTR(""));
	this->txtCPU->SetRect(104, 28, 300, 23, false);
	this->txtCPU->SetReadOnly(true);
	this->lvRAM = ui->NewListView(this->tpInfo, UI::ListViewStyle::Table, 9);
	this->lvRAM->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvRAM->SetFullRowSelect(true);
	this->lvRAM->SetShowGrid(true);
	this->lvRAM->AddColumn(CSTR("Locator"), 100);
	this->lvRAM->AddColumn(CSTR("Manufacturer"), 60);
	this->lvRAM->AddColumn(CSTR("PartNo"), 140);
	this->lvRAM->AddColumn(CSTR("SN"), 80);
	this->lvRAM->AddColumn(CSTR("Def Speed"), 40);
	this->lvRAM->AddColumn(CSTR("Conf Speed"), 40);
	this->lvRAM->AddColumn(CSTR("Data Width"), 40);
	this->lvRAM->AddColumn(CSTR("Total Width"), 40);
	this->lvRAM->AddColumn(CSTR("Memory Size"), 80);

	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	IO::SystemInfo sysInfo;
	if (sysInfo.GetPlatformName(sbuff).SetTo(sptr))
	{
		this->txtPlatform->SetText(CSTRP(sbuff, sptr));
	}
	else
	{
		this->txtPlatform->SetText(CSTR("-"));
	}
	Manage::CPUInfo cpu;
	if (cpu.GetCPUName(sbuff).SetTo(sptr))
	{
		this->txtCPU->SetText(CSTRP(sbuff, sptr));
	}
	else
	{
		this->txtCPU->SetText(CSTR("-"));
	}
	Data::ArrayListNN<IO::SystemInfo::RAMInfo> ramList;
	NN<IO::SystemInfo::RAMInfo> ram;
	sysInfo.GetRAMInfo(ramList);
	UOSInt i;
	UOSInt j;
	UOSInt k;
	i = 0;
	j = ramList.GetCount();
	while (i < j)
	{
		ram = ramList.GetItemNoCheck(i);
		k = this->lvRAM->AddItem(Text::String::OrEmpty(ram->deviceLocator), 0);
		this->lvRAM->SetSubItem(k, 1, Text::String::OrEmpty(ram->manufacturer));
		this->lvRAM->SetSubItem(k, 2, Text::String::OrEmpty(ram->partNo));
		this->lvRAM->SetSubItem(k, 3, Text::String::OrEmpty(ram->sn));
		sptr = Text::StrUOSInt(sbuff, ram->defSpdMHz);
		this->lvRAM->SetSubItem(k, 4, CSTRP(sbuff, sptr));
		sptr = Text::StrUOSInt(sbuff, ram->confSpdMHz);
		this->lvRAM->SetSubItem(k, 5, CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, ram->dataWidth);
		this->lvRAM->SetSubItem(k, 6, CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, ram->totalWidth);
		this->lvRAM->SetSubItem(k, 7, CSTRP(sbuff, sptr));
		if (ram->memorySize == 0)
		{
			sptr = Text::StrUInt64(sbuff, ram->memorySize);
		}
		else if (ram->memorySize >= 1073741824)
		{
			sptr = Text::StrConcatC(Text::StrUInt64(sbuff, ram->memorySize / 1073741824), UTF8STRC("GB"));
		}
		else if (ram->memorySize >= 1048576)
		{
			sptr = Text::StrConcatC(Text::StrUInt64(sbuff, ram->memorySize / 1073741824), UTF8STRC("MB"));
		}
		else if (ram->memorySize >= 1024)
		{
			sptr = Text::StrConcatC(Text::StrUInt64(sbuff, ram->memorySize / 1073741824), UTF8STRC("KB"));
		}
		this->lvRAM->SetSubItem(k, 8, CSTRP(sbuff, sptr));
		i++;
	}
	sysInfo.FreeRAMInfo(ramList);
}

SSWR::AVIRead::AVIRBenchmarkForm::~AVIRBenchmarkForm()
{
	this->ClearResult();
}

void SSWR::AVIRead::AVIRBenchmarkForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
