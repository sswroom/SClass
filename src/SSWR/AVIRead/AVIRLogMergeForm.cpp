#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRLogMergeForm.h"
#include "Text/MyString.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRLogMergeForm::OnFile1Clicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLogMergeForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLogMergeForm>();
	Text::StringBuilderUTF8 sb;
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"LogMergeFile1", false);
	dlg->AddFilter(CSTR("*.log"), CSTR("Log File"));
	dlg->SetAllowMultiSel(false);
	me->txtFile1->GetText(sb);
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToCString());
	}
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtFile1->SetText(dlg->GetFileName()->ToCString());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRLogMergeForm::OnFile2Clicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLogMergeForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLogMergeForm>();
	Text::StringBuilderUTF8 sb;
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"LogMergeFile2", false);
	dlg->AddFilter(CSTR("*.log"), CSTR("Log File"));
	dlg->SetAllowMultiSel(false);
	me->txtFile2->GetText(sb);
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToCString());
	}
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtFile2->SetText(dlg->GetFileName()->ToCString());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRLogMergeForm::OnOFileClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLogMergeForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLogMergeForm>();
	Text::StringBuilderUTF8 sb;
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"LogMergeOFile", true);
	dlg->AddFilter(CSTR("*.log"), CSTR("Log File"));
	me->txtOFile->GetText(sb);
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToCString());
	}
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtOFile->SetText(dlg->GetFileName()->ToCString());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRLogMergeForm::OnConvertClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLogMergeForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLogMergeForm>();
	UTF8Char sbuff[16];
	Text::StringBuilderUTF8 sb1;
	Text::StringBuilderUTF8 sb2;
	Text::StringBuilderUTF8 sb3;
	me->txtFile1->GetText(sb1);
	me->txtFile2->GetText(sb2);
	me->txtOFile->GetText(sb3);
	if (IO::Path::GetPathType(sb1.ToCString()) != IO::Path::PathType::File)
	{
		me->ui->ShowMsgOK(CSTR("File1 not found"), CSTR("Error"), me);
	}
	else if (IO::Path::GetPathType(sb2.ToCString()) != IO::Path::PathType::File)
	{
		me->ui->ShowMsgOK(CSTR("File2 not found"), CSTR("Error"), me);
	}
	else if (sb3.GetLength() <= 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter Output"), CSTR("Error"), me);
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
		NN<IO::FileStream> fs1;
		NN<IO::FileStream> fs2;
		NN<IO::FileStream> fs3;
		NN<Text::UTF8Reader> reader1;
		NN<Text::UTF8Reader> reader2;
		NN<Text::UTF8Writer> writer;
		NEW_CLASSNN(fs1, IO::FileStream(sb1.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASSNN(reader1, Text::UTF8Reader(fs1));
		NEW_CLASSNN(fs2, IO::FileStream(sb2.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASSNN(reader2, Text::UTF8Reader(fs2));
		NEW_CLASSNN(fs3, IO::FileStream(sb3.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		if (fs3->IsError())
		{
			me->ui->ShowMsgOK(CSTR("Error in creating Output file"), CSTR("Error"), me);
		}
		else
		{
			dt.ToLocalTime();
			NEW_CLASSNN(writer, Text::UTF8Writer(fs3));
			writer->WriteSignature();
			sb1.ClearStr();
			succ1 = reader1->ReadLine(sb1, 1024);
			sb2.ClearStr();
			succ2 = reader2->ReadLine(sb2, 1024);
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
						sb1.v[i] = 0;
						dt.SetValue(sb1.ToCString());
						t1 = dt.ToTicks();
						sb1.v[i] = '\t';
						sb2.v[j] = 0;
						dt.SetValue(sb2.ToCString());
						t2 = dt.ToTicks();
						sb2.v[j] = '\t';
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
							succ1 = reader1->ReadLine(sb1, 1024);
							reader1->GetLastLineBreak(sbuff);
						}
						writer->WriteLineC(sb1.ToString(), sb1.GetLength());
						sb1.ClearStr();
						succ1 = reader1->ReadLine(sb1, 1024);
					}
					else
					{
						reader2->GetLastLineBreak(sbuff);
						while (succ2 && sbuff[0] == 0)
						{
							succ2 = reader2->ReadLine(sb2, 1024);
							reader2->GetLastLineBreak(sbuff);
						}
						writer->WriteLineC(sb2.ToString(), sb2.GetLength());
						sb2.ClearStr();
						succ2 = reader2->ReadLine(sb2, 1024);
					}
				}
				else if (succ1)
				{
					reader1->GetLastLineBreak(sbuff);
					while (succ1 && sbuff[0] == 0)
					{
						succ1 = reader1->ReadLine(sb1, 1024);
						reader1->GetLastLineBreak(sbuff);
					}
					writer->WriteLineC(sb1.ToString(), sb1.GetLength());
					sb1.ClearStr();
					succ1 = reader1->ReadLine(sb1, 1024);
				}
				else if (succ2)
				{
					reader2->GetLastLineBreak(sbuff);
					while (succ2 && sbuff[0] == 0)
					{
						succ2 = reader2->ReadLine(sb2, 1024);
						reader2->GetLastLineBreak(sbuff);
					}
					writer->WriteLineC(sb2.ToString(), sb2.GetLength());
					sb2.ClearStr();
					succ2 = reader2->ReadLine(sb2, 1024);
				}
			}
			me->ui->ShowMsgOK(CSTR("Complete log merge"), CSTR("Success"), me);
			writer.Delete();
		}
		fs3.Delete();
		reader2.Delete();
		fs2.Delete();
		reader1.Delete();
		fs1.Delete();
	}
}

SSWR::AVIRead::AVIRLogMergeForm::AVIRLogMergeForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 136, ui)
{
	this->SetText(CSTR("Log Merge"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblFile1 = ui->NewLabel(*this, CSTR("File1"));
	this->lblFile1->SetRect(4, 4, 100, 23, false);
	this->txtFile1 = ui->NewTextBox(*this, CSTR(""));
	this->txtFile1->SetRect(104, 4, 560, 23, false);
	this->btnFile1 = ui->NewButton(*this, CSTR("Browse"));
	this->btnFile1->SetRect(660, 4, 75, 23, false);
	this->btnFile1->HandleButtonClick(OnFile1Clicked, this);
	this->lblFile2 = ui->NewLabel(*this, CSTR("File2"));
	this->lblFile2->SetRect(4, 28, 100, 23, false);
	this->txtFile2 = ui->NewTextBox(*this, CSTR(""));
	this->txtFile2->SetRect(104, 28, 560, 23, false);
	this->btnFile2 = ui->NewButton(*this, CSTR("Browse"));
	this->btnFile2->SetRect(660, 28, 75, 23, false);
	this->btnFile2->HandleButtonClick(OnFile2Clicked, this);
	this->lblOFile = ui->NewLabel(*this, CSTR("Output"));
	this->lblOFile->SetRect(4, 52, 100, 23, false);
	this->txtOFile = ui->NewTextBox(*this, CSTR(""));
	this->txtOFile->SetRect(104, 52, 560, 23, false);
	this->btnOFile = ui->NewButton(*this, CSTR("Browse"));
	this->btnOFile->SetRect(660, 52, 75, 23, false);
	this->btnOFile->HandleButtonClick(OnOFileClicked, this);
	this->btnConvert = ui->NewButton(*this, CSTR("Convert"));
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
