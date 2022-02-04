#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRLogMergeForm.h"
#include "Text/MyString.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRLogMergeForm::OnFile1Clicked(void *userObj)
{
	SSWR::AVIRead::AVIRLogMergeForm *me = (SSWR::AVIRead::AVIRLogMergeForm *)userObj;
	UI::FileDialog *dlg;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"LogMergeFile1", false));
	dlg->AddFilter((const UTF8Char*)"*.log", (const UTF8Char*)"Log File");
	dlg->SetAllowMultiSel(false);
	me->txtFile1->GetText(&sb);
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToString());
	}
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtFile1->SetText(dlg->GetFileName()->v);
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRLogMergeForm::OnFile2Clicked(void *userObj)
{
	SSWR::AVIRead::AVIRLogMergeForm *me = (SSWR::AVIRead::AVIRLogMergeForm *)userObj;
	UI::FileDialog *dlg;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"LogMergeFile2", false));
	dlg->AddFilter((const UTF8Char*)"*.log", (const UTF8Char*)"Log File");
	dlg->SetAllowMultiSel(false);
	me->txtFile2->GetText(&sb);
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToString());
	}
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtFile2->SetText(dlg->GetFileName()->v);
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRLogMergeForm::OnOFileClicked(void *userObj)
{
	SSWR::AVIRead::AVIRLogMergeForm *me = (SSWR::AVIRead::AVIRLogMergeForm *)userObj;
	UI::FileDialog *dlg;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"LogMergeOFile", true));
	dlg->AddFilter((const UTF8Char*)"*.log", (const UTF8Char*)"Log File");
	me->txtOFile->GetText(&sb);
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToString());
	}
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtOFile->SetText(dlg->GetFileName()->v);
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRLogMergeForm::OnConvertClicked(void *userObj)
{
	SSWR::AVIRead::AVIRLogMergeForm *me = (SSWR::AVIRead::AVIRLogMergeForm *)userObj;
	UTF8Char sbuff[16];
	Text::StringBuilderUTF8 sb1;
	Text::StringBuilderUTF8 sb2;
	Text::StringBuilderUTF8 sb3;
	me->txtFile1->GetText(&sb1);
	me->txtFile2->GetText(&sb2);
	me->txtOFile->GetText(&sb3);
	if (IO::Path::GetPathType(sb1.ToString(), sb1.GetLength()) != IO::Path::PathType::File)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"File1 not found", (const UTF8Char*)"Error", me);
	}
	else if (IO::Path::GetPathType(sb2.ToString(), sb2.GetLength()) != IO::Path::PathType::File)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"File2 not found", (const UTF8Char*)"Error", me);
	}
	else if (sb3.GetLength() <= 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter Output", (const UTF8Char*)"Error", me);
	}
	else
	{
		Bool succ1;
		Bool succ2;
		UOSInt i;
		UOSInt j;
		Data::DateTime dt;
		Int64 t1;
		Int64 t2;
		IO::FileStream *fs1;
		IO::FileStream *fs2;
		IO::FileStream *fs3;
		Text::UTF8Reader *reader1;
		Text::UTF8Reader *reader2;
		Text::UTF8Writer *writer;
		NEW_CLASS(fs1, IO::FileStream(sb1.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASS(reader1, Text::UTF8Reader(fs1));
		NEW_CLASS(fs2, IO::FileStream(sb2.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASS(reader2, Text::UTF8Reader(fs2));
		NEW_CLASS(fs3, IO::FileStream(sb3.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		if (fs3->IsError())
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in creating Output file", (const UTF8Char*)"Error", me);
		}
		else
		{
			dt.ToLocalTime();
			NEW_CLASS(writer, Text::UTF8Writer(fs3));
			writer->WriteSignature();
			sb1.ClearStr();
			succ1 = reader1->ReadLine(&sb1, 1024);
			sb2.ClearStr();
			succ2 = reader2->ReadLine(&sb2, 1024);
			while (succ1 || succ2)
			{
				if (succ1 && succ2)
				{
					Bool is2 = false;
					i = sb1.IndexOf('\t');
					j = sb2.IndexOf('\t');
					if (i == INVALID_INDEX)
					{
						is2 = false;
					}
					else if (j == INVALID_INDEX)
					{
						is2 = true;
					}
					else
					{
						sb1.ToString()[i] = 0;
						dt.SetValue(sb1.ToString(), sb1.GetLength());
						t1 = dt.ToTicks();
						sb1.ToString()[i] = '\t';
						sb2.ToString()[j] = 0;
						dt.SetValue(sb2.ToString(), sb2.GetLength());
						t2 = dt.ToTicks();
						sb2.ToString()[j] = '\t';
						if (t1 == 0)
						{
							is2 = false;
						}
						else if (t2 == 0)
						{
							is2 = true;
						}
						else
						{
							is2 = (t2 < t1);
						}
					}
					if (!is2)
					{
						reader1->GetLastLineBreak(sbuff);
						while (succ1 && sbuff[0] == 0)
						{
							succ1 = reader1->ReadLine(&sb1, 1024);
							reader1->GetLastLineBreak(sbuff);
						}
						writer->WriteLineC(sb1.ToString(), sb1.GetLength());
						sb1.ClearStr();
						succ1 = reader1->ReadLine(&sb1, 1024);
					}
					else
					{
						reader2->GetLastLineBreak(sbuff);
						while (succ2 && sbuff[0] == 0)
						{
							succ2 = reader2->ReadLine(&sb2, 1024);
							reader2->GetLastLineBreak(sbuff);
						}
						writer->WriteLineC(sb2.ToString(), sb2.GetLength());
						sb2.ClearStr();
						succ2 = reader2->ReadLine(&sb2, 1024);
					}
				}
				else if (succ1)
				{
					reader1->GetLastLineBreak(sbuff);
					while (succ1 && sbuff[0] == 0)
					{
						succ1 = reader1->ReadLine(&sb1, 1024);
						reader1->GetLastLineBreak(sbuff);
					}
					writer->WriteLineC(sb1.ToString(), sb1.GetLength());
					sb1.ClearStr();
					succ1 = reader1->ReadLine(&sb1, 1024);
				}
				else if (succ2)
				{
					reader2->GetLastLineBreak(sbuff);
					while (succ2 && sbuff[0] == 0)
					{
						succ2 = reader2->ReadLine(&sb2, 1024);
						reader2->GetLastLineBreak(sbuff);
					}
					writer->WriteLineC(sb2.ToString(), sb2.GetLength());
					sb2.ClearStr();
					succ2 = reader2->ReadLine(&sb2, 1024);
				}
			}
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Complete log merge", (const UTF8Char*)"Success", me);
			DEL_CLASS(writer);
		}
		DEL_CLASS(fs3);
		DEL_CLASS(reader2);
		DEL_CLASS(fs2);
		DEL_CLASS(reader1);
		DEL_CLASS(fs1);
	}
}

SSWR::AVIRead::AVIRLogMergeForm::AVIRLogMergeForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 800, 136, ui)
{
	this->SetText((const UTF8Char*)"Log Merge");
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblFile1, UI::GUILabel(ui, this, (const UTF8Char*)"File1"));
	this->lblFile1->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtFile1, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtFile1->SetRect(104, 4, 560, 23, false);
	NEW_CLASS(this->btnFile1, UI::GUIButton(ui, this, (const UTF8Char*)"Browse"));
	this->btnFile1->SetRect(660, 4, 75, 23, false);
	this->btnFile1->HandleButtonClick(OnFile1Clicked, this);
	NEW_CLASS(this->lblFile2, UI::GUILabel(ui, this, (const UTF8Char*)"File2"));
	this->lblFile2->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtFile2, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtFile2->SetRect(104, 28, 560, 23, false);
	NEW_CLASS(this->btnFile2, UI::GUIButton(ui, this, (const UTF8Char*)"Browse"));
	this->btnFile2->SetRect(660, 28, 75, 23, false);
	this->btnFile2->HandleButtonClick(OnFile2Clicked, this);
	NEW_CLASS(this->lblOFile, UI::GUILabel(ui, this, (const UTF8Char*)"Output"));
	this->lblOFile->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtOFile, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtOFile->SetRect(104, 52, 560, 23, false);
	NEW_CLASS(this->btnOFile, UI::GUIButton(ui, this, (const UTF8Char*)"Browse"));
	this->btnOFile->SetRect(660, 52, 75, 23, false);
	this->btnOFile->HandleButtonClick(OnOFileClicked, this);
	NEW_CLASS(this->btnConvert, UI::GUIButton(ui, this, (const UTF8Char*)"Convert"));
	this->btnConvert->SetRect(200, 76, 75, 23, false);
	this->btnConvert->HandleButtonClick(OnConvertClicked, this);
}

SSWR::AVIRead::AVIRLogMergeForm::~AVIRLogMergeForm()
{
}

void SSWR::AVIRead::AVIRLogMergeForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
