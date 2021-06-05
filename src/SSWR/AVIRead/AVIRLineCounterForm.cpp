#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StreamReader.h"
#include "IO/StreamWriter.h"
#include "SSWR/AVIRead/AVIRLineCounterForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/FileDialog.h"

void __stdcall SSWR::AVIRead::AVIRLineCounterForm::OnExtensionsAddClicked(void *userObj)
{
	SSWR::AVIRead::AVIRLineCounterForm *me = (SSWR::AVIRead::AVIRLineCounterForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtExtensions->GetText(&sb);
	if (sb.GetLength() == 0)
		return;
	UOSInt i = me->extList->GetCount();
	while (i-- > 0)
	{
		if (Text::StrEqualsICase(me->extList->GetItem(i), sb.ToString()))
		{
			return;
		}
	}
	me->extList->Add(Text::StrCopyNew(sb.ToString()));
	me->lbExtensions->AddItem(sb.ToString(), 0);
}

void __stdcall SSWR::AVIRead::AVIRLineCounterForm::OnExtensionsRemoveClicked(void *userObj)
{
	SSWR::AVIRead::AVIRLineCounterForm *me = (SSWR::AVIRead::AVIRLineCounterForm*)userObj;
	OSInt i = me->lbExtensions->GetSelectedIndex();
	if (i >= 0)
	{
		Text::StrDelNew(me->extList->RemoveAt((UOSInt)i));
		me->lbExtensions->RemoveItem((UOSInt)i);
	}
}

void __stdcall SSWR::AVIRead::AVIRLineCounterForm::OnExtensionsClearClicked(void *userObj)
{
	SSWR::AVIRead::AVIRLineCounterForm *me = (SSWR::AVIRead::AVIRLineCounterForm*)userObj;
	me->ClearExts();
}

void __stdcall SSWR::AVIRead::AVIRLineCounterForm::OnCalcClicked(void *userObj)
{
	SSWR::AVIRead::AVIRLineCounterForm *me = (SSWR::AVIRead::AVIRLineCounterForm*)userObj;
	UTF8Char sbuff[512];
	Text::StringBuilderUTF8 sb;
	if (me->extList->GetCount() <= 0)
		return;
	me->txtPath->GetText(&sb);
	if (sb.GetLength() == 0)
	{
		return;
	}
	if (sb.EndsWith((Char)IO::Path::PATH_SEPERATOR))
	{
		sb.RemoveChars(1);
	}
	if (IO::Path::GetPathType(sb.ToString()) != IO::Path::PT_DIRECTORY)
	{
		return;
	}
	me->ClearResult();
	UTF8Char *sptr = Text::StrConcat(sbuff, sb.ToString());
	me->CalcDir(sbuff, sptr);
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt totalLine = 0;
	FileInfo *fi;
	i = 0;
	j = me->resList->GetCount();
	while (i < j)
	{
		fi = me->resList->GetItem(i);
		Text::StrUOSInt(sbuff, fi->lineCnt);
		k = me->lvResult->AddItem(sbuff, fi);
		me->lvResult->SetSubItem(k, 1, fi->fileName);
		totalLine += fi->lineCnt;
		i++;
	}
	Text::StrUOSInt(sbuff, totalLine);
	me->txtTotalLine->SetText(sbuff);
}

void __stdcall SSWR::AVIRead::AVIRLineCounterForm::OnResultSaveClicked(void *userObj)
{
	SSWR::AVIRead::AVIRLineCounterForm *me = (SSWR::AVIRead::AVIRLineCounterForm*)userObj;
	if (me->resList->GetCount() == 0)
		return;

	UI::FileDialog *dlg;
	NEW_CLASS(dlg, UI::FileDialog(L"sswr", L"AVIRead", L"LineCounterSave", true));
	dlg->AddFilter((const UTF8Char*)"*.txt", (const UTF8Char*)"Result file");
	if (dlg->ShowDialog(me->GetHandle()))
	{
		Text::StringBuilderUTF8 sb;
		FileInfo *fi;
		IO::FileStream *fs;
		IO::StreamWriter *writer;
		UOSInt i;
		UOSInt j;
		NEW_CLASS(fs, IO::FileStream(dlg->GetFileName(), IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
		NEW_CLASS(writer, IO::StreamWriter(fs, (UInt32)0));
		i = 0;
		j = me->resList->GetCount();
		while (i < j)
		{
			fi = me->resList->GetItem(i);
			sb.ClearStr();
			sb.AppendUOSInt(fi->lineCnt);
			sb.Append((const UTF8Char*)"\t");
			sb.Append(fi->fileName);
			writer->WriteLine(sb.ToString());
			i++;
		}
		DEL_CLASS(writer);
		DEL_CLASS(fs);
	}
	DEL_CLASS(dlg);
}

void SSWR::AVIRead::AVIRLineCounterForm::CalcDir(UTF8Char *pathBuff, UTF8Char *pathBuffEnd)
{
	IO::Path::PathType pt;
	UTF8Char *sptr;
	IO::Path::FindFileSession *sess;
	Text::StringBuilderUTF8 sb;
	IO::FileStream *fs;
	IO::StreamReader *reader;
	UOSInt lineCnt;
	UOSInt i;
	UOSInt j;
	Bool found;
	FileInfo *fi;

	*pathBuffEnd++ = IO::Path::PATH_SEPERATOR;
	Text::StrConcat(pathBuffEnd, IO::Path::ALL_FILES);
	sess = IO::Path::FindFile(pathBuff);
	if (sess)
	{
		while ((sptr = IO::Path::FindNextFile(pathBuffEnd, sess, 0, &pt, 0)) != 0)
		{
			if (pt == IO::Path::PT_DIRECTORY)
			{
				if (pathBuffEnd[0] != '.')
				{
					CalcDir(pathBuff, sptr);
				}
			}
			else if (pt == IO::Path::PT_FILE)
			{
				found = false;
				i = (UOSInt)Text::StrLastIndexOf(pathBuffEnd, '.') + 1;
				j = this->extList->GetCount();
				while (j-- > 0)
				{
					if (Text::StrEqualsICase(&pathBuffEnd[i], this->extList->GetItem(j)))
					{
						found = true;
						break;
					}
				}
				if (found)
				{
					lineCnt = 0;
					NEW_CLASS(fs, IO::FileStream(pathBuff, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
					NEW_CLASS(reader, IO::StreamReader(fs, 65001));
					while (true)
					{
						sb.ClearStr();
						if (reader->ReadLine(&sb, 4096))
						{
							lineCnt++;
						}
						else
						{
							break;
						}
					}
					DEL_CLASS(reader);
					DEL_CLASS(fs);

					fi = MemAlloc(FileInfo, 1);
					fi->lineCnt = lineCnt;
					fi->fileName = Text::StrCopyNew(pathBuff);
					this->resList->Add(fi);
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}
}

void SSWR::AVIRead::AVIRLineCounterForm::ClearExts()
{
	UOSInt i;
	this->lbExtensions->ClearItems();
	i = this->extList->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->extList->GetItem(i));
	}
	this->extList->Clear();
}

void SSWR::AVIRead::AVIRLineCounterForm::ClearResult()
{
	UOSInt i;
	FileInfo *fi;
	this->lvResult->ClearItems();
	i = this->resList->GetCount();
	while (i-- > 0)
	{
		fi = this->resList->GetItem(i);
		Text::StrDelNew(fi->fileName);
		MemFree(fi);
	}
	this->resList->Clear();
}

SSWR::AVIRead::AVIRLineCounterForm::AVIRLineCounterForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetText((const UTF8Char*)"Line Counter");
	this->SetFont(0, 8.25, false);

	NEW_CLASS(this->extList, Data::ArrayList<const UTF8Char *>());
	NEW_CLASS(this->resList, Data::ArrayList<FileInfo*>());

	NEW_CLASS(this->pnlConfig, UI::GUIPanel(ui, this));
	this->pnlConfig->SetRect(0, 0, 100, 220, false);
	this->pnlConfig->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblPath, UI::GUILabel(ui, this->pnlConfig, (const UTF8Char*)"Path"));
	this->lblPath->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtPath, UI::GUITextBox(ui, this->pnlConfig, (const UTF8Char*)""));
	this->txtPath->SetRect(104, 4, 700, 23, false);
	NEW_CLASS(this->lblExtensions, UI::GUILabel(ui, this->pnlConfig, (const UTF8Char*)"Extensions"));
	this->lblExtensions->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtExtensions, UI::GUITextBox(ui, this->pnlConfig, (const UTF8Char*)""));
	this->txtExtensions->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->btnExtensionsAdd, UI::GUIButton(ui, this->pnlConfig, (const UTF8Char*)"Add"));
	this->btnExtensionsAdd->SetRect(204, 28, 75, 23, false);
	this->btnExtensionsAdd->HandleButtonClick(OnExtensionsAddClicked, this);
	NEW_CLASS(this->lbExtensions, UI::GUIListBox(ui, this->pnlConfig, false));
	this->lbExtensions->SetRect(104, 52, 100, 71, false);
	NEW_CLASS(this->btnExtensionsRemove, UI::GUIButton(ui, this->pnlConfig, (const UTF8Char*)"Remove"));
	this->btnExtensionsRemove->SetRect(104, 124, 75, 23, false);
	this->btnExtensionsRemove->HandleButtonClick(OnExtensionsRemoveClicked, this);
	NEW_CLASS(this->btnExtensionsClear, UI::GUIButton(ui, this->pnlConfig, (const UTF8Char*)"Clear"));
	this->btnExtensionsClear->SetRect(184, 124, 75, 23, false);
	this->btnExtensionsClear->HandleButtonClick(OnExtensionsClearClicked, this);
	NEW_CLASS(this->btnCalc, UI::GUIButton(ui, this->pnlConfig, (const UTF8Char*)"Calc"));
	this->btnCalc->SetRect(104, 172, 75, 23, false);
	this->btnCalc->HandleButtonClick(OnCalcClicked, this);
	NEW_CLASS(this->lblResult, UI::GUILabel(ui, this->pnlConfig, (const UTF8Char*)"Result:"));
	this->lblResult->SetRect(4, 196, 100, 23, false);
	NEW_CLASS(this->lblTotalLine, UI::GUILabel(ui, this->pnlConfig, (const UTF8Char*)"Total Lines"));
	this->lblTotalLine->SetRect(104, 196, 100, 23, false);
	NEW_CLASS(this->txtTotalLine, UI::GUITextBox(ui, this->pnlConfig, (const UTF8Char*)""));
	this->txtTotalLine->SetReadOnly(true);
	this->txtTotalLine->SetRect(204, 196, 100, 23, false);
	NEW_CLASS(this->btnResultSave, UI::GUIButton(ui, this->pnlConfig, (const UTF8Char*)"Save Result"));
	this->btnResultSave->SetRect(304, 196, 100, 23, false);
	this->btnResultSave->HandleButtonClick(OnResultSaveClicked, this);

	NEW_CLASS(this->lvResult, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvResult->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvResult->SetShowGrid(true);
	this->lvResult->SetFullRowSelect(true);
	this->lvResult->AddColumn((const UTF8Char*)"Count", 60);
	this->lvResult->AddColumn((const UTF8Char*)"File Path", 812);
}

SSWR::AVIRead::AVIRLineCounterForm::~AVIRLineCounterForm()
{
	this->ClearExts();
	this->ClearResult();
	DEL_CLASS(this->extList);
	DEL_CLASS(this->resList);
}

void SSWR::AVIRead::AVIRLineCounterForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
