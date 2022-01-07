#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/ZIPBuilder.h"
#include "SSWR/AVIRead/AVIRTimedFileCopyForm.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRTimedFileCopyForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRTimedFileCopyForm *me = (SSWR::AVIRead::AVIRTimedFileCopyForm*)userObj;
	Data::DateTime dt1;
	Data::DateTime dt2;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[1024];
	UTF8Char *sptr;
	me->dtpStartTime->GetSelectedTime(&dt1);
	me->dtpEndTime->GetSelectedTime(&dt2);
	me->txtFileDir->GetText(&sb);
	if (IO::Path::GetPathType(sb.ToString()) != IO::Path::PathType::Directory)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"The file dir is not a directory", me->GetFormName(), me);
		return;
	}
	dt1.ClearTime();
	dt2.ClearTime();
	if (dt1.CompareTo(&dt2) > 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"The start time is after end time", me->GetFormName(), me);
		return;
	}
	Double days = Data::DateTime::MS2Days(dt2.DiffMS(&dt1));
	if (days > 90)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"The time range is longer than 90 days", me->GetFormName(), me);
		return;
	}

	UI::FileDialog *dlg;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"TimedFileCopy", true));
	dlg->AddFilter((const UTF8Char*)"*.zip", (const UTF8Char*)"Zip file");
	if (dt1.GetYear() == dt2.GetYear() && dt1.GetMonth() == dt2.GetMonth() && dt1.GetDay() == dt2.GetDay())
	{
		sptr = dt1.ToString(sbuff, "yyyyMMdd");
		Text::StrConcatC(sptr, UTF8STRC(".zip"));
	}
	else if (dt1.GetYear() == dt2.GetYear() && dt1.GetMonth() == dt2.GetMonth())
	{
		sptr = dt1.ToString(sbuff, "yyyyMMdd");
		*sptr++ = '-';
		sptr = dt2.ToString(sptr, "dd");
		Text::StrConcatC(sptr, UTF8STRC(".zip"));
	}
	else if (dt1.GetYear() == dt2.GetYear())
	{
		sptr = dt1.ToString(sbuff, "yyyyMMdd");
		*sptr++ = '-';
		sptr = dt2.ToString(sptr, "MMdd");
		Text::StrConcatC(sptr, UTF8STRC(".zip"));
	}
	else
	{
		sptr = dt1.ToString(sbuff, "yyyyMMdd");
		*sptr++ = '-';
		sptr = dt2.ToString(sptr, "yyyyMMdd");
		Text::StrConcatC(sptr, UTF8STRC(".zip"));
	}
	dlg->SetFileName(sbuff);
	if (dlg->ShowDialog(me->GetHandle()))
	{
		IO::ZIPBuilder *zip;
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		if (fs->IsError())
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in creating zip file", me->GetFormName(), me);
		}
		NEW_CLASS(zip, IO::ZIPBuilder(fs));
		sptr = Text::StrConcatC(sbuff, sb.ToString(), sb.GetLength());
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		me->CopyToZip(zip, sbuff, sptr, sptr, &dt1, &dt2, true);

		DEL_CLASS(zip);
		DEL_CLASS(fs);
	}
	DEL_CLASS(dlg);
}

Bool SSWR::AVIRead::AVIRTimedFileCopyForm::CopyToZip(IO::ZIPBuilder *zip, const UTF8Char *buffStart, const UTF8Char *pathBase, UTF8Char *pathEnd, Data::DateTime *startTime, Data::DateTime *endTime, Bool monthDir)
{
	UTF8Char *sptr;
	IO::Path::FindFileSession *sess;
	IO::Path::PathType pt;
	Int32 iVal;
	Bool succ;
	UOSInt i;
	Data::DateTime dt(startTime);
	Data::DateTime modTime;
	Text::StrConcatC(pathEnd, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	sess = IO::Path::FindFile(buffStart);
	if (sess)
	{
		while ((sptr = IO::Path::FindNextFile(pathEnd, sess, &modTime, &pt, 0)) != 0)
		{
			if (pt == IO::Path::PathType::File)
			{
				succ = false;
				i = Text::StrIndexOf(pathEnd, '.');
				if (i != INVALID_INDEX && i >= 8)
				{
					pathEnd[i] = 0;
					succ = Text::StrToInt32(&pathEnd[i - 8], &iVal);
					pathEnd[i] = '.';
				}
				else if (i == INVALID_INDEX && (sptr - pathEnd) >= 8)
				{
					succ = Text::StrToInt32(sptr - 8, &iVal);
				}
				if (succ)
				{
					dt.SetYear((UInt16)(iVal / 10000));
					iVal = iVal % 10000;
					dt.SetMonth(iVal / 100);
					dt.SetDay(iVal % 100);
					if (startTime->CompareTo(&dt) <= 0 && endTime->CompareTo(&dt) >= 0)
					{
						IO::FileStream *fs;
						NEW_CLASS(fs, IO::FileStream(buffStart, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
						if (fs->IsError())
						{
							succ = false;
						}
						else
						{
							UInt8 *fileBuff;
							UInt64 fileLeng = fs->GetLength();
							UOSInt totalRead = 0;
							UOSInt readSize;
							if (fileLeng > 0)
							{
								fileBuff = MemAlloc(UInt8, (UOSInt)fileLeng);
								while (totalRead < fileLeng)
								{
									readSize = fs->Read(&fileBuff[totalRead], (UOSInt)(fileLeng - totalRead));
									if (readSize <= 0)
									{
										break;
									}
									totalRead += readSize;
								}
								if (totalRead == fileLeng)
								{
									succ = zip->AddFile(pathBase, fileBuff, totalRead, modTime.ToTicks(), false);
								}
								else
								{
									succ = false;
								}
								MemFree(fileBuff);
							}
						}
						DEL_CLASS(fs);
						if (!succ)
						{
							Text::StringBuilderUTF8 sb;
							sb.AppendC(UTF8STRC("Error in copying "));
							sb.Append(buffStart);
							UI::MessageDialog::ShowDialog(sb.ToString(), this->GetFormName(), this);
							return false;
						}
					}
				}
			}
			else if (pt == IO::Path::PathType::Directory)
			{
				if (pathEnd[0] != '.')
				{
					if (monthDir)
					{
						if ((sptr - pathEnd) >= 6 && Text::StrToInt32(sptr - 6, &iVal))
						{
							dt.SetDate((UInt16)(iVal / 100), iVal % 100, 1);
							if ((dt.GetYear() == startTime->GetYear() && dt.GetMonth() == startTime->GetMonth()) || (startTime->CompareTo(&dt) <= 0 && endTime->CompareTo(&dt) >= 0))
							{
								*sptr++ = IO::Path::PATH_SEPERATOR;
								if (!this->CopyToZip(zip, buffStart, pathBase, sptr, startTime, endTime, false))
								{
									return false;
								}
							}
						}
					}
					else
					{
						*sptr++ = IO::Path::PATH_SEPERATOR;
						if (!this->CopyToZip(zip, buffStart, pathBase, sptr, startTime, endTime, false))
						{
							return false;
						}
					}
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}
	return true;
}

SSWR::AVIRead::AVIRTimedFileCopyForm::AVIRTimedFileCopyForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 652, 180, ui)
{
	this->SetText(this->GetFormName());
	this->SetFont(0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblFileDir, UI::GUILabel(ui, this, (const UTF8Char*)"File Dir"));
	this->lblFileDir->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtFileDir, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtFileDir->SetRect(104, 4, 400, 23, false);
	NEW_CLASS(this->lblStartTime, UI::GUILabel(ui, this, (const UTF8Char*)"Start Time"));
	this->lblStartTime->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->dtpStartTime, UI::GUIDateTimePicker(ui, this, UI::GUIDateTimePicker::ST_UPDOWN));
	this->dtpStartTime->SetFormat("yyyy-MM-dd");
	this->dtpStartTime->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->lblEndTime, UI::GUILabel(ui, this, (const UTF8Char*)"End Time"));
	this->lblEndTime->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->dtpEndTime, UI::GUIDateTimePicker(ui, this, UI::GUIDateTimePicker::ST_UPDOWN));
	this->dtpEndTime->SetFormat("yyyy-MM-dd");
	this->dtpEndTime->SetRect(104, 52, 100, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this, (const UTF8Char*)"Start"));
	this->btnStart->SetRect(104, 76, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
}

SSWR::AVIRead::AVIRTimedFileCopyForm::~AVIRTimedFileCopyForm()
{

}

const UTF8Char *SSWR::AVIRead::AVIRTimedFileCopyForm::GetFormName()
{
	return (const UTF8Char*)"Timed File Copy";
}

void SSWR::AVIRead::AVIRTimedFileCopyForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
