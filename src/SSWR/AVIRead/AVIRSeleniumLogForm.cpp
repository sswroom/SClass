#include "Stdafx.h"
#include "IO/DirectoryPackage.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRSeleniumLogForm.h"
#include "Text/JSON.h"
#include "UI/GUIFileDialog.h"
#include "UI/GUIFolderDialog.h"

void __stdcall SSWR::AVIRead::AVIRSeleniumLogForm::OnBrowseClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSeleniumLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSeleniumLogForm>();
	NN<UI::GUIFolderDialog> dlg = me->ui->NewFolderDialog();
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->OpenDir(dlg->GetFolder()->ToCString());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRSeleniumLogForm::OnCSVClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSeleniumLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSeleniumLogForm>();
	if (me->logItems.GetCount() > 0)
	{
		Text::StringBuilderUTF8 sb;
		NN<Text::String> s;
		NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"SeleniumLogCSV", true);
		dlg->AddFilter(CSTR("*.csv"), CSTR("CSV File"));
		if (me->srcFile.SetTo(s))
		{
			sb.Append(s);
			if (sb.EndsWith(IO::Path::PATH_SEPERATOR))
			{
				sb.RemoveChars(1);
			}
			else if (sb.EndsWith(CSTR(".zip")))
			{
				sb.RemoveChars(4);
			}
			sb.Append(CSTR(".csv"));
			dlg->SetFileName(sb.ToCString());
		}
		if (dlg->ShowDialog(me->GetHandle()))
		{
			NN<LogItem> log;
			IO::FileStream fs(dlg->GetFileName()->ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			fs.Write(CSTR("StartTime,EndTime,Status,Title\r\n").ToByteArray());
			UOSInt i = 0;
			UOSInt j = me->logItems.GetCount();
			while (i < j)
			{
				sb.ClearStr();
				log = me->logItems.GetItemNoCheck(i);
				sb.AppendTSNoZone(Data::Timestamp(log->startTime, Data::DateTimeUtil::GetLocalTzQhr()));
				sb.AppendUTF8Char(',');
				sb.AppendTSNoZone(Data::Timestamp(log->endTime, Data::DateTimeUtil::GetLocalTzQhr()));
				sb.AppendUTF8Char(',');
				s = Text::String::NewCSVRec(log->status->v);
				sb.Append(s);
				s->Release();
				sb.AppendUTF8Char(',');
				s = Text::String::NewCSVRec(log->title->v);
				sb.Append(s);
				s->Release();
				sb.Append(CSTR("\r\n"));
				fs.Write(sb.ToByteArray());
				i++;
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRSeleniumLogForm::OnDirectoryDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRSeleniumLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSeleniumLogForm>();
	UOSInt i = 0;
	UOSInt j = files.GetCount();
	while (i < j)
	{
		if (IO::Path::GetPathType(files[i]->ToCString()) == IO::Path::PathType::Directory)
		{
			me->OpenDir(files[i]->ToCString());
		}
		i++;
	}
}

void SSWR::AVIRead::AVIRSeleniumLogForm::OpenDir(Text::CStringNN dir)
{
	this->ClearItems();
	this->lvContent->ClearItems();
	OPTSTR_DEL(this->srcFile);
	this->srcFile = Text::String::New(dir);
	IO::DirectoryPackage dpkg(dir);
	this->OpenPackage(dpkg);
}

void SSWR::AVIRead::AVIRSeleniumLogForm::OpenPackage(NN<IO::PackageFile> pkg)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	IO::PackageFile::PackObjectType pot;
	Bool needRelease;
	NN<IO::PackageFile> innerPkg;
	NN<IO::StreamData> stmData;
	UnsafeArray<UTF8Char> buff = MemAllocArr(UTF8Char, 1048577);
	NN<Text::JSONBase> json;
	NN<LogItem> item;
	UInt64 fileLen;
	UOSInt k;
	UOSInt i = 0;
	UOSInt j = pkg->GetCount();
	while (i < j)
	{
		pot = pkg->GetItemType(i);
		if (pot == IO::PackageFile::PackObjectType::PackageFileType)
		{
			if (pkg->GetItemPack(i, needRelease).SetTo(innerPkg))
			{
				this->OpenPackage(innerPkg);
				if (needRelease)
				{
					innerPkg.Delete();
				}
			}
		}
		else if (pot == IO::PackageFile::PackObjectType::StreamData)
		{
			sbuff[0] = 0;
			sptr = pkg->GetItemName(sbuff, i).Or(sbuff);
			if (Text::StrEndsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC(".json")) && pkg->GetItemStmDataNew(i).SetTo(stmData))
			{
				fileLen = stmData->GetDataSize();
				if (fileLen >= 2 && fileLen <= 1048576)
				{
					if (stmData->GetRealData(0, (UOSInt)fileLen, Data::ByteArray(buff, 1048576)) == fileLen)
					{
						buff[(UOSInt)fileLen] = 0;
						if (Text::JSONBase::ParseJSONStr(Text::CStringNN(buff, (UOSInt)fileLen)).SetTo(json))
						{
							Int64 startTime = json->GetValueAsInt64(CSTR("testResults[0].startTime"));
							Int64 endTime = json->GetValueAsInt64(CSTR("testResults[0].endTime"));
							Optional<Text::String> optStatus = json->GetValueString(CSTR("testResults[0].status"));
							Optional<Text::String> optTitle = json->GetValueString(CSTR("testResults[0].assertionResults[0].ancestorTitles[0]"));
							NN<Text::String> status;
							NN<Text::String> title;
							if (startTime != 0 && endTime != 0 && optStatus.SetTo(status) && optTitle.SetTo(title))
							{
								NEW_CLASSNN(item, LogItem());
								item->startTime = startTime;
								item->endTime = endTime;
								item->status = status->Clone();
								item->title = title->Clone();
								this->logItems.Add(item);
								sptr = Data::Timestamp(startTime, Data::DateTimeUtil::GetLocalTzQhr()).ToStringNoZone(sbuff);
								k = this->lvContent->AddItem(CSTRP(sbuff, sptr), item);
								sptr = Data::Timestamp(endTime, Data::DateTimeUtil::GetLocalTzQhr()).ToStringNoZone(sbuff);
								this->lvContent->SetSubItem(k, 1, CSTRP(sbuff, sptr));
								this->lvContent->SetSubItem(k, 2, status->ToCString());
								this->lvContent->SetSubItem(k, 3, title->ToCString());
							}
							json->EndUse();
						}
					}
				}
				stmData.Delete();
			}
		}
		i++;
	}
	MemFreeArr(buff);
}

void SSWR::AVIRead::AVIRSeleniumLogForm::ClearItems()
{
	this->logItems.FreeAll(FreeItem);
}

void SSWR::AVIRead::AVIRSeleniumLogForm::FreeItem(NN<LogItem> item)
{
	item->status->Release();
	item->title->Release();
	item.Delete();
}

SSWR::AVIRead::AVIRSeleniumLogForm::AVIRSeleniumLogForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 480, ui)
{
	this->SetText(CSTR("Selenium Log"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->srcFile = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnBrowse = ui->NewButton(this->pnlControl, CSTR("Browse"));
	this->btnBrowse->SetRect(4, 4, 75, 23, false);
	this->btnBrowse->HandleButtonClick(OnBrowseClick, this);
	this->btnCSV = ui->NewButton(this->pnlControl, CSTR("Export CSV"));
	this->btnCSV->SetRect(84, 4, 75, 23, false);
	this->btnCSV->HandleButtonClick(OnCSVClick, this);
	this->lvContent = ui->NewListView(*this, UI::ListViewStyle::Table, 4);
	this->lvContent->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvContent->AddColumn(CSTR("StartTime"), 150);
	this->lvContent->AddColumn(CSTR("EndTime"), 150);
	this->lvContent->AddColumn(CSTR("Status"), 100);
	this->lvContent->AddColumn(CSTR("Title"), 300);
	this->HandleDropFiles(OnDirectoryDrop, this);
}

SSWR::AVIRead::AVIRSeleniumLogForm::~AVIRSeleniumLogForm()
{
	this->ClearItems();
	OPTSTR_DEL(this->srcFile);
}

void SSWR::AVIRead::AVIRSeleniumLogForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
