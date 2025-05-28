#include "Stdafx.h"
#include "DB/CSVFile.h"
#include "IO/FileStream.h"
#include "SSWR/AVIRead/AVIRSIDERunnerLogForm.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRSIDERunnerLogForm::OnFiles(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRSIDERunnerLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSIDERunnerLogForm>();
	UOSInt i = 0;
	UOSInt j = files.GetCount();
	UOSInt k;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> s;
	NN<StepStat> step;
	NN<Text::String> file;
	while (i < j)
	{
		file = files[i];
		if (file->EndsWith(CSTR(".side")))
		{
			UInt64 fileLen;
			UnsafeArray<UInt8> buff;
			UOSInt buffSize;
			NN<Text::JSONBase> json;
			NN<IO::SeleniumIDE> side;
			NN<IO::SeleniumTest> test;
			NN<IO::SeleniumCommand> command;
			IO::FileStream fs(file->ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (!fs.IsError() && (fileLen = fs.GetLength()) >= 4 && fileLen <= 1048576)
			{
				buffSize = (UOSInt)fileLen;
				buff = MemAllocArr(UInt8, buffSize + 1);
				if (fs.Read(Data::ByteArray(buff, buffSize)) == buffSize)
				{
					buff[buffSize] = 0;
					if (Text::JSONBase::ParseJSONStr(Text::CStringNN(buff, buffSize)).SetTo(json))
					{
						if (json->GetType() == Text::JSONType::Object)
						{
							NEW_CLASSNN(side, IO::SeleniumIDE(file, NN<Text::JSONObject>::ConvertFrom(json)));
							if (side->GetTest(0).SetTo(test))
							{
								me->side.Delete();
								me->side = side;
								me->txtFile->SetText(file->ToCString());
								me->steps.MemFreeAll();
								me->lvSteps->ClearItems();
								k = 0;
								while (test->GetCommand(k).SetTo(command))
								{
									step = MemAllocNN(StepStat);
									step->count = 0;
									step->totalTime = 0;
									step->minTime = 0;
									step->maxTime = 0;
									me->steps.Add(step);
									sptr = Text::StrUOSInt(sbuff, k);
									me->lvSteps->AddItem(CSTRP(sbuff, sptr), step);
									if (command->GetCommand().SetTo(s)) me->lvSteps->SetSubItem(k, 1, s);
									if (command->GetTarget().SetTo(s)) me->lvSteps->SetSubItem(k, 2, s);
									if (command->GetValue().SetTo(s)) me->lvSteps->SetSubItem(k, 3, s);
									me->lvSteps->SetSubItem(k, 4, CSTR("0"));
									me->lvSteps->SetSubItem(k, 5, CSTR("0"));
									me->lvSteps->SetSubItem(k, 6, CSTR("0"));
									me->lvSteps->SetSubItem(k, 7, CSTR("0"));
									me->lvSteps->SetSubItem(k, 8, CSTR("0"));
									k++;
								}
							}
							else
							{
								side.Delete();
							}
						}
						json->EndUse();
					}
				}
				MemFreeArr(buff);
			}
		}
		else if (file->EndsWith(CSTR(".csv")) && (k = me->steps.GetCount()) > 0)
		{
			DB::CSVFile csv(file, 0);
			NN<DB::DBReader> r;
			if (csv.QueryTableData(0, CSTR(""), 0, 0, 0, 0, 0).SetTo(r))
			{
				Bool succ = true;
				if (!r->GetName(0, sbuff).SetTo(sptr) || !CSTRP(sbuff, sptr).Equals(CSTR("Time"))) succ = false;
				if (!r->GetName(1, sbuff).SetTo(sptr) || !CSTRP(sbuff, sptr).Equals(CSTR("TestId"))) succ = false;
				if (!r->GetName(2, sbuff).SetTo(sptr) || !CSTRP(sbuff, sptr).Equals(CSTR("Step"))) succ = false;
				if (!r->GetName(3, sbuff).SetTo(sptr) || !CSTRP(sbuff, sptr).Equals(CSTR("Duration"))) succ = false;
				if (succ)
				{
					Int32 index;
					Double dur;
					while (r->ReadNext())
					{
						index = r->GetInt32(2);
						dur = r->GetDblOr(3, 0);
						if (index >= 0 && (UInt32)index < k)
						{
							step = me->steps.GetItemNoCheck((UInt32)index);
							if (step->count == 0)
							{
								step->count++;
								step->maxTime = dur;
								step->minTime = dur;
								step->totalTime = dur;
							}
							else
							{
								step->count++;
								step->totalTime += dur;
								if (step->maxTime < dur) step->maxTime = dur;
								if (step->minTime > dur) step->minTime = dur;
							}
						}
					}

					k = me->steps.GetCount();
					while (k-- > 0)
					{
						step = me->steps.GetItemNoCheck(k);
						sptr = Text::StrUOSInt(sbuff, step->count);
						me->lvSteps->SetSubItem(k, 4, CSTRP(sbuff, sptr));
						sptr = Text::StrDoubleDP(sbuff, step->totalTime, 0, 6);
						me->lvSteps->SetSubItem(k, 5, CSTRP(sbuff, sptr));
						if (step->count == 0)
						{
							me->lvSteps->SetSubItem(k, 6, CSTR("0"));
						}
						else
						{
							sptr = Text::StrDoubleDP(sbuff, step->totalTime / (Double)step->count, 0, 6);
							me->lvSteps->SetSubItem(k, 6, CSTRP(sbuff, sptr));
						}
						sptr = Text::StrDoubleDP(sbuff, step->minTime, 0, 6);
						me->lvSteps->SetSubItem(k, 7, CSTRP(sbuff, sptr));
						sptr = Text::StrDoubleDP(sbuff, step->maxTime, 0, 6);
						me->lvSteps->SetSubItem(k, 8, CSTRP(sbuff, sptr));
					}
				}
				csv.CloseReader(r);
			}
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRSIDERunnerLogForm::OnExportClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSIDERunnerLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSIDERunnerLogForm>();
	NN<IO::SeleniumIDE> side;
	NN<IO::SeleniumTest> test;
	NN<IO::SeleniumCommand> command;
	NN<StepStat> step;
	NN<Text::String> s;
	if (me->steps.GetCount() > 0 && me->side.SetTo(side) && side->GetTest(0).SetTo(test))
	{
		Text::StringBuilderUTF8 sb;
		NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"SIDERunnerLog", true);
		dlg->AddFilter(CSTR("*.csv"), CSTR("CSV File"));
		me->txtFile->GetText(sb);
		sb.Append(CSTR(".csv"));
		dlg->SetFileName(sb.ToCString());
		if (dlg->ShowDialog(me->GetHandle()))
		{
			IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			UOSInt i;
			
			fs.Write(CSTR("Index,Command,Target,Value,Count,Total Time,Avg Time,Min Time,Max Time\r\n").ToByteArray());
			i = 0;
			while (test->GetCommand(i).SetTo(command) && me->steps.GetItem(i).SetTo(step))
			{
				sb.ClearStr();
				sb.AppendUOSInt(i);
				sb.AppendUTF8Char(',');
#define APPENDOPT(optstr) if (optstr.SetTo(s)) { s = Text::String::NewCSVRec(s->v); sb.Append(s); s->Release(); }
				APPENDOPT(command->GetCommand())
				sb.AppendUTF8Char(',');
				APPENDOPT(command->GetTarget())
				sb.AppendUTF8Char(',');
				APPENDOPT(command->GetValue())
#undef APPENDOPT
				sb.AppendUTF8Char(',');
				sb.AppendUOSInt(step->count);
				sb.AppendUTF8Char(',');
				sb.AppendDouble(step->totalTime);
				sb.AppendUTF8Char(',');
				sb.AppendDouble(step->totalTime / (Double)step->count);
				sb.AppendUTF8Char(',');
				sb.AppendDouble(step->minTime);
				sb.AppendUTF8Char(',');
				sb.AppendDouble(step->maxTime);
				sb.Append(CSTR("\r\n"));
				fs.Write(sb.ToByteArray());
				i++;
			}
		}
		dlg.Delete();
	}
}

SSWR::AVIRead::AVIRSIDERunnerLogForm::AVIRSIDERunnerLogForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetText(CSTR("SIDERunner Log"));

	this->pnlFile = ui->NewPanel(*this);
	this->pnlFile->SetRect(0, 0, 100, 31, false);
	this->pnlFile->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblFile = ui->NewLabel(this->pnlFile, CSTR("File"));
	this->lblFile->SetRect(4, 4, 100, 23, false);
	this->txtFile = ui->NewTextBox(this->pnlFile, CSTR(""));
	this->txtFile->SetReadOnly(true);
	this->txtFile->SetRect(104, 4, 600, 23, false);
	this->pnlCtrl = ui->NewPanel(*this);
	this->pnlCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnSave = ui->NewButton(this->pnlCtrl, CSTR("Save"));
	this->btnSave->SetRect(4, 4, 75, 23, false);
	this->btnSave->HandleButtonClick(OnExportClicked, this);
	this->lvSteps = ui->NewListView(*this, UI::ListViewStyle::Table, 9);
	this->lvSteps->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvSteps->SetFullRowSelect(true);
	this->lvSteps->SetShowGrid(true);
	this->lvSteps->AddColumn(CSTR("Index"), 60);
	this->lvSteps->AddColumn(CSTR("Command"), 100);
	this->lvSteps->AddColumn(CSTR("Target"), 200);
	this->lvSteps->AddColumn(CSTR("Value"), 200);
	this->lvSteps->AddColumn(CSTR("Count"), 60);
	this->lvSteps->AddColumn(CSTR("Total Time"), 60);
	this->lvSteps->AddColumn(CSTR("Avg Time"), 60);
	this->lvSteps->AddColumn(CSTR("Min Time"), 60);
	this->lvSteps->AddColumn(CSTR("Max Time"), 60);

	this->side = 0;
	this->HandleDropFiles(OnFiles, this);
}

SSWR::AVIRead::AVIRSIDERunnerLogForm::~AVIRSIDERunnerLogForm()
{
	this->steps.MemFreeAll();
	this->side.Delete();
}

void SSWR::AVIRead::AVIRSIDERunnerLogForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
