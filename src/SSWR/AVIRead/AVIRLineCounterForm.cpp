#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StreamWriter.h"
#include "SSWR/AVIRead/AVIRLineCounterForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRLineCounterForm::OnExtensionsAddClicked(void *userObj)
{
	SSWR::AVIRead::AVIRLineCounterForm *me = (SSWR::AVIRead::AVIRLineCounterForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtExtensions->GetText(sb);
	if (sb.GetLength() == 0)
		return;
	Data::ArrayIterator<NotNullPtr<Text::String>> it = me->extList.Iterator();
	while (it.HasNext())
	{
		if (it.Next()->EqualsICase(sb.ToString(), sb.GetLength()))
		{
			return;
		}
	}
	me->extList.Add(Text::String::New(sb.ToString(), sb.GetLength()));
	me->lbExtensions->AddItem(sb.ToCString(), 0);
}

void __stdcall SSWR::AVIRead::AVIRLineCounterForm::OnExtensionsRemoveClicked(void *userObj)
{
	SSWR::AVIRead::AVIRLineCounterForm *me = (SSWR::AVIRead::AVIRLineCounterForm*)userObj;
	UOSInt i = me->lbExtensions->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		OPTSTR_DEL(me->extList.RemoveAt(i));
		me->lbExtensions->RemoveItem(i);
	}
}

void __stdcall SSWR::AVIRead::AVIRLineCounterForm::OnExtensionsClearClicked(void *userObj)
{
	SSWR::AVIRead::AVIRLineCounterForm *me = (SSWR::AVIRead::AVIRLineCounterForm*)userObj;
	me->ClearExts(true);
}

void __stdcall SSWR::AVIRead::AVIRLineCounterForm::OnCalcClicked(void *userObj)
{
	SSWR::AVIRead::AVIRLineCounterForm *me = (SSWR::AVIRead::AVIRLineCounterForm*)userObj;
	UTF8Char sbuff[512];
	Text::StringBuilderUTF8 sb;
	if (me->extList.GetCount() <= 0)
		return;
	me->txtPath->GetText(sb);
	if (sb.GetLength() == 0)
	{
		return;
	}
	if (sb.EndsWith(IO::Path::PATH_SEPERATOR))
	{
		sb.RemoveChars(1);
	}
	if (IO::Path::GetPathType(sb.ToCString()) != IO::Path::PathType::Directory)
	{
		return;
	}
	me->ClearResult(true);
	UTF8Char *sptr = Text::StrConcatC(sbuff, sb.ToString(), sb.GetLength());
	me->CalcDir(sbuff, sptr);
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt totalLine = 0;
	UOSInt nonEmpty = 0;
	UOSInt codeCnt = 0;
	FileInfo *fi;
	i = 0;
	j = me->resList.GetCount();
	while (i < j)
	{
		fi = me->resList.GetItem(i);
		sptr = Text::StrUOSInt(sbuff, fi->totalLineCnt);
		k = me->lvResult->AddItem(CSTRP(sbuff, sptr), fi);
		sptr = Text::StrUOSInt(sbuff, fi->nonEmpyCnt);
		me->lvResult->SetSubItem(k, 1, CSTRP(sbuff, sptr));
		sptr = Text::StrUOSInt(sbuff, fi->codeCnt);
		me->lvResult->SetSubItem(k, 2, CSTRP(sbuff, sptr));
		me->lvResult->SetSubItem(k, 3, fi->fileName);
		totalLine += fi->totalLineCnt;
		nonEmpty += fi->nonEmpyCnt;
		codeCnt += fi->codeCnt;
		i++;
	}
	sptr = Text::StrUOSInt(sbuff, totalLine);
	me->txtTotalLine->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrUOSInt(sbuff, nonEmpty);
	me->txtNonEmpty->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrUOSInt(sbuff, codeCnt);
	me->txtCode->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRLineCounterForm::OnResultSaveClicked(void *userObj)
{
	SSWR::AVIRead::AVIRLineCounterForm *me = (SSWR::AVIRead::AVIRLineCounterForm*)userObj;
	if (me->resList.GetCount() == 0)
		return;

	NotNullPtr<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"LineCounterSave", true);
	dlg->AddFilter(CSTR("*.txt"), CSTR("Result file"));
	if (dlg->ShowDialog(me->GetHandle()))
	{
		Text::StringBuilderUTF8 sb;
		FileInfo *fi;
		UOSInt i;
		UOSInt j;
		IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		IO::StreamWriter writer(fs, (UInt32)0);
		i = 0;
		j = me->resList.GetCount();
		while (i < j)
		{
			fi = me->resList.GetItem(i);
			sb.ClearStr();
			sb.AppendUOSInt(fi->totalLineCnt);
			sb.AppendUTF8Char('\t');
			sb.AppendUOSInt(fi->nonEmpyCnt);
			sb.AppendUTF8Char('\t');
			sb.AppendUOSInt(fi->codeCnt);
			sb.AppendUTF8Char('\t');
			sb.Append(fi->fileName);
			writer.WriteLineC(sb.ToString(), sb.GetLength());
			i++;
		}
	}
	dlg.Delete();
}

void SSWR::AVIRead::AVIRLineCounterForm::CalcDir(UTF8Char *pathBuff, UTF8Char *pathBuffEnd)
{
	IO::Path::PathType pt;
	UTF8Char *sptr;
	IO::Path::FindFileSession *sess;
	Text::StringBuilderUTF8 sb;
	UOSInt lineCnt;
	UOSInt nonEmptyCnt;
	UOSInt codeCnt;
	UOSInt i;
	Bool found;
	FileInfo *fi;

	*pathBuffEnd++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(pathBuffEnd, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	sess = IO::Path::FindFile(CSTRP(pathBuff, sptr));
	if (sess)
	{
		while ((sptr = IO::Path::FindNextFile(pathBuffEnd, sess, 0, &pt, 0)) != 0)
		{
			if (pt == IO::Path::PathType::Directory)
			{
				if (pathBuffEnd[0] != '.')
				{
					CalcDir(pathBuff, sptr);
				}
			}
			else if (pt == IO::Path::PathType::File)
			{
				found = false;
				i = Text::StrLastIndexOfCharC(pathBuffEnd, (UOSInt)(sptr - pathBuffEnd), '.') + 1;
				Data::ArrayIterator<NotNullPtr<Text::String>> it = this->extList.Iterator();
				while (it.HasNext())
				{
					if (it.Next()->EqualsICase(&pathBuffEnd[i], (UOSInt)(sptr - &pathBuffEnd[i])))
					{
						found = true;
						break;
					}
				}
				if (found)
				{
					nonEmptyCnt = 0;
					codeCnt = 0;
					lineCnt = 0;
					{
						IO::FileStream fs({pathBuff, (UOSInt)(sptr - pathBuff)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
						Text::UTF8Reader reader(fs);
						while (true)
						{
							sb.ClearStr();
							if (reader.ReadLine(sb, 4096))
							{
								lineCnt++;
								sb.Trim();
								if (sb.leng > 0)
								{
									nonEmptyCnt++;
									if (sb.HasAlphaNumeric())
									{
										codeCnt++;
									}
								}
							}
							else
							{
								break;
							}
						}
					}

					fi = MemAlloc(FileInfo, 1);
					fi->totalLineCnt = lineCnt;
					fi->nonEmpyCnt = nonEmptyCnt;
					fi->codeCnt = codeCnt;
					fi->fileName = Text::String::New(pathBuff, (UOSInt)(sptr - pathBuff));
					this->resList.Add(fi);
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}
}

void SSWR::AVIRead::AVIRLineCounterForm::ClearExts(Bool inclDisp)
{
	if (inclDisp) this->lbExtensions->ClearItems();
	this->extList.FreeAll();
}

void SSWR::AVIRead::AVIRLineCounterForm::ClearResult(Bool inclDisp)
{
	UOSInt i;
	FileInfo *fi;
	if (inclDisp) this->lvResult->ClearItems();
	i = this->resList.GetCount();
	while (i-- > 0)
	{
		fi = this->resList.GetItem(i);
		fi->fileName->Release();
		MemFree(fi);
	}
	this->resList.Clear();
}

SSWR::AVIRead::AVIRLineCounterForm::AVIRLineCounterForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetText(CSTR("Line Counter"));
	this->SetFont(0, 0, 8.25, false);

	this->pnlConfig = ui->NewPanel(*this);
	this->pnlConfig->SetRect(0, 0, 100, 268, false);
	this->pnlConfig->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblPath = ui->NewLabel(this->pnlConfig, CSTR("Path"));
	this->lblPath->SetRect(4, 4, 100, 23, false);
	this->txtPath = ui->NewTextBox(this->pnlConfig, CSTR(""));
	this->txtPath->SetRect(104, 4, 700, 23, false);
	this->lblExtensions = ui->NewLabel(this->pnlConfig, CSTR("Extensions"));
	this->lblExtensions->SetRect(4, 28, 100, 23, false);
	this->txtExtensions = ui->NewTextBox(this->pnlConfig, CSTR(""));
	this->txtExtensions->SetRect(104, 28, 100, 23, false);
	this->btnExtensionsAdd = ui->NewButton(this->pnlConfig, CSTR("Add"));
	this->btnExtensionsAdd->SetRect(204, 28, 75, 23, false);
	this->btnExtensionsAdd->HandleButtonClick(OnExtensionsAddClicked, this);
	this->lbExtensions = ui->NewListBox(this->pnlConfig, false);
	this->lbExtensions->SetRect(104, 52, 100, 71, false);
	this->btnExtensionsRemove = ui->NewButton(this->pnlConfig, CSTR("Remove"));
	this->btnExtensionsRemove->SetRect(104, 124, 75, 23, false);
	this->btnExtensionsRemove->HandleButtonClick(OnExtensionsRemoveClicked, this);
	this->btnExtensionsClear = ui->NewButton(this->pnlConfig, CSTR("Clear"));
	this->btnExtensionsClear->SetRect(184, 124, 75, 23, false);
	this->btnExtensionsClear->HandleButtonClick(OnExtensionsClearClicked, this);
	this->btnCalc = ui->NewButton(this->pnlConfig, CSTR("Calc"));
	this->btnCalc->SetRect(104, 172, 75, 23, false);
	this->btnCalc->HandleButtonClick(OnCalcClicked, this);
	this->lblResult = ui->NewLabel(this->pnlConfig, CSTR("Result:"));
	this->lblResult->SetRect(4, 196, 100, 23, false);
	this->lblTotalLine = ui->NewLabel(this->pnlConfig, CSTR("Total Lines"));
	this->lblTotalLine->SetRect(104, 196, 100, 23, false);
	this->txtTotalLine = ui->NewTextBox(this->pnlConfig, CSTR(""));
	this->txtTotalLine->SetReadOnly(true);
	this->txtTotalLine->SetRect(204, 196, 100, 23, false);
	this->lblNonEmpty = ui->NewLabel(this->pnlConfig, CSTR("Non-empty"));
	this->lblNonEmpty->SetRect(104, 220, 100, 23, false);
	this->txtNonEmpty = ui->NewTextBox(this->pnlConfig, CSTR(""));
	this->txtNonEmpty->SetReadOnly(true);
	this->txtNonEmpty->SetRect(204, 220, 100, 23, false);
	this->lblCode = ui->NewLabel(this->pnlConfig, CSTR("Code"));
	this->lblCode->SetRect(104, 244, 100, 23, false);
	this->txtCode = ui->NewTextBox(this->pnlConfig, CSTR(""));
	this->txtCode->SetReadOnly(true);
	this->txtCode->SetRect(204, 244, 100, 23, false);
	this->btnResultSave = ui->NewButton(this->pnlConfig, CSTR("Save Result"));
	this->btnResultSave->SetRect(304, 244, 100, 23, false);
	this->btnResultSave->HandleButtonClick(OnResultSaveClicked, this);

	this->lvResult = ui->NewListView(*this, UI::ListViewStyle::Table, 4);
	this->lvResult->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvResult->SetShowGrid(true);
	this->lvResult->SetFullRowSelect(true);
	this->lvResult->AddColumn(CSTR("Total"), 60);
	this->lvResult->AddColumn(CSTR("NonEmpty"), 60);
	this->lvResult->AddColumn(CSTR("Code"), 60);
	this->lvResult->AddColumn(CSTR("File Path"), 692);
}

SSWR::AVIRead::AVIRLineCounterForm::~AVIRLineCounterForm()
{
	this->ClearExts(false);
	this->ClearResult(false);
}

void SSWR::AVIRead::AVIRLineCounterForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
