#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "IO/FileStream.h"
#include "SSWR/AVIRead/AVIRMD5CompareForm.h"
#include "Text/UTF8Reader.h"
#include "UI/GUIFileDialog.h"

class SSWR::AVIRead::AVIRMD5CompareForm::MD5Comparator : public Data::Comparator<NN<MD5Entry>>
{
public:
	virtual ~MD5Comparator() {}

	virtual IntOS Compare(NN<MD5Entry> a, NN<MD5Entry> b) const
	{
		return a->fileName->CompareTo(b->fileName);
	}
};

void __stdcall SSWR::AVIRead::AVIRMD5CompareForm::FreeMD5Entry(NN<MD5Entry> entry)
{
	entry->fileName->Release();
	entry->md5->Release();
	MemFreeNN(entry);
}

void SSWR::AVIRead::AVIRMD5CompareForm::LoadMD5(Text::CStringNN fileName, NN<Data::ArrayListNN<MD5Entry>> md5List)
{
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		printf("Error in opening file: %s\r\n", fileName.v.Ptr());
		return;
	}
	Text::UTF8Reader reader(fs);
	Text::StringBuilderUTF8 sb;
	while (reader.ReadLine(sb, 8192))
	{
		if (sb.leng > 32 && sb.v[32] == ' ')
		{
			NN<MD5Entry> entry = MemAllocNN(MD5Entry);
			entry->md5 = Text::String::New(sb.v, 32);
			entry->fileName = Text::String::New(sb.v + 33, sb.leng - 33);
			md5List->Add(entry);
		}
		sb.ClearStr();
	}
}

Optional<SSWR::AVIRead::AVIRMD5CompareForm::MD5Entry> SSWR::AVIRead::AVIRMD5CompareForm::MD5FindEntry(NN<Data::ArrayListNN<MD5Entry>> md5List, Text::CStringNN fileName)
{
	IntOS i = 0;
	IntOS j = (IntOS)md5List->GetCount() - 1;
	IntOS k;
	NN<MD5Entry> entry;
	IntOS l;
	while (i <= j)
	{
		k = (i + j) >> 1;
		entry = md5List->GetItemNoCheck((UIntOS)k);
		l = entry->fileName->CompareTo(fileName);
		if (l == 0)
		{
			return entry;
		}
		else if (l < 0)
		{
			i = k + 1;
		}
		else
		{
			j = k - 1;
		}
	}
	return nullptr;
}

void __stdcall SSWR::AVIRead::AVIRMD5CompareForm::OnFile1Clicked(AnyType userObj)
{
	NN<AVIRMD5CompareForm> me = userObj.GetNN<AVIRMD5CompareForm>();
	Text::StringBuilderUTF8 sb;
	me->txtFile1->GetText(sb);
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"MD5CompareFile1", false);
	dlg->SetAllowMultiSel(false);
	dlg->AddFilter(CSTR("*.md5"), CSTR("MD5 File"));
	if (sb.leng > 0)
	{
		dlg->SetFileName(sb.ToCString());
	}
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtFile1->SetText(dlg->GetFileName()->ToCString());
		me->UpdateStatus();
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRMD5CompareForm::OnFile2Clicked(AnyType userObj)
{
	NN<AVIRMD5CompareForm> me = userObj.GetNN<AVIRMD5CompareForm>();
	Text::StringBuilderUTF8 sb;
	me->txtFile2->GetText(sb);
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"MD5CompareFile2", false);
	dlg->SetAllowMultiSel(false);
	dlg->AddFilter(CSTR("*.md5"), CSTR("MD5 File"));
	if (sb.leng > 0)
	{
		dlg->SetFileName(sb.ToCString());
	}
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtFile2->SetText(dlg->GetFileName()->ToCString());
		me->UpdateStatus();
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRMD5CompareForm::OnMoreClicked(AnyType userObj)
{
	NN<AVIRMD5CompareForm> me = userObj.GetNN<AVIRMD5CompareForm>();
	Text::StringBuilderUTF8 sbFile1;
	Text::StringBuilderUTF8 sbFile2;
	me->txtFile1->GetText(sbFile1);
	me->txtFile2->GetText(sbFile2);
	if (sbFile1.leng == 0 || sbFile2.leng == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please select both files"), CSTR("Error"), me);
		return;
	}
	Data::ArrayListNN<MD5Entry> md5List1;
	Data::ArrayListNN<MD5Entry> md5List2;
	LoadMD5(sbFile1.ToCString(), md5List1);
	LoadMD5(sbFile2.ToCString(), md5List2);
	if (md5List1.GetCount() > 0 && md5List2.GetCount() > 0)
	{
		NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"MD5CompareMore", true);
		dlg->SetAllowMultiSel(false);
		dlg->AddFilter(CSTR("*.md5"), CSTR("MD5 File"));
		if (dlg->ShowDialog(me->GetHandle()))
		{
			IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (fs.IsError())
			{
				me->ui->ShowMsgOK(CSTR("Error in creating file"), CSTR("Error"), me);
			}
			else
			{
				Text::StringBuilderUTF8 sb;
				MD5Comparator comparator;
				Data::Sort::ArtificialQuickSort::Sort<NN<MD5Entry>>(md5List1, comparator);
				Data::Sort::ArtificialQuickSort::Sort<NN<MD5Entry>>(md5List2, comparator);
				NN<MD5Entry> entry2;
				UIntOS i = 0;
				UIntOS j = md5List2.GetCount();
				while (i < j)
				{
					entry2 = md5List2.GetItemNoCheck(i);
					if (MD5FindEntry(md5List1, entry2->fileName->ToCString()).IsNull())
					{
						sb.Append(entry2->md5);
						sb.AppendUTF8Char(' ');
						sb.Append(entry2->fileName);
						sb.AppendC(UTF8STRC("\r\n"));
						if (sb.GetLength() > 65536)
						{
							fs.Write(sb.ToByteArray());
							sb.ClearStr();
						}
					}
					i++;
				}
				if (sb.GetLength() > 0)
				{
					fs.Write(sb.ToByteArray());
				}
			}
		}
		dlg.Delete();
	}
	md5List1.FreeAll(FreeMD5Entry);
	md5List2.FreeAll(FreeMD5Entry);
}

void __stdcall SSWR::AVIRead::AVIRMD5CompareForm::OnLessClicked(AnyType userObj)
{
	NN<AVIRMD5CompareForm> me = userObj.GetNN<AVIRMD5CompareForm>();
	Text::StringBuilderUTF8 sbFile1;
	Text::StringBuilderUTF8 sbFile2;
	me->txtFile1->GetText(sbFile1);
	me->txtFile2->GetText(sbFile2);
	if (sbFile1.leng == 0 || sbFile2.leng == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please select both files"), CSTR("Error"), me);
		return;
	}
	Data::ArrayListNN<MD5Entry> md5List1;
	Data::ArrayListNN<MD5Entry> md5List2;
	LoadMD5(sbFile1.ToCString(), md5List1);
	LoadMD5(sbFile2.ToCString(), md5List2);
	if (md5List1.GetCount() > 0 && md5List2.GetCount() > 0)
	{
		NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"MD5CompareLess", true);
		dlg->SetAllowMultiSel(false);
		dlg->AddFilter(CSTR("*.md5"), CSTR("MD5 File"));
		if (dlg->ShowDialog(me->GetHandle()))
		{
			IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (fs.IsError())
			{
				me->ui->ShowMsgOK(CSTR("Error in creating file"), CSTR("Error"), me);
			}
			else
			{
				Text::StringBuilderUTF8 sb;
				MD5Comparator comparator;
				Data::Sort::ArtificialQuickSort::Sort<NN<MD5Entry>>(md5List1, comparator);
				Data::Sort::ArtificialQuickSort::Sort<NN<MD5Entry>>(md5List2, comparator);
				NN<MD5Entry> entry1;
				UIntOS i = 0;
				UIntOS j = md5List1.GetCount();
				while (i < j)
				{
					entry1 = md5List1.GetItemNoCheck(i);
					if (MD5FindEntry(md5List2, entry1->fileName->ToCString()).IsNull())
					{
						sb.Append(entry1->md5);
						sb.AppendUTF8Char(' ');
						sb.Append(entry1->fileName);
						sb.AppendC(UTF8STRC("\r\n"));
						if (sb.GetLength() > 65536)
						{
							fs.Write(sb.ToByteArray());
							sb.ClearStr();	
						}
					}
					i++;
				}
				if (sb.GetLength() > 0)
				{
					fs.Write(sb.ToByteArray());
				}
			}
		}
		dlg.Delete();
	}
	md5List1.FreeAll(FreeMD5Entry);
	md5List2.FreeAll(FreeMD5Entry);
}

void __stdcall SSWR::AVIRead::AVIRMD5CompareForm::OnMismatchClicked(AnyType userObj)
{
	NN<AVIRMD5CompareForm> me = userObj.GetNN<AVIRMD5CompareForm>();
	Text::StringBuilderUTF8 sbFile1;
	Text::StringBuilderUTF8 sbFile2;
	me->txtFile1->GetText(sbFile1);
	me->txtFile2->GetText(sbFile2);
	if (sbFile1.leng == 0 || sbFile2.leng == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please select both files"), CSTR("Error"), me);
		return;
	}
	Data::ArrayListNN<MD5Entry> md5List1;
	Data::ArrayListNN<MD5Entry> md5List2;
	LoadMD5(sbFile1.ToCString(), md5List1);
	LoadMD5(sbFile2.ToCString(), md5List2);
	if (md5List1.GetCount() > 0 && md5List2.GetCount() > 0)
	{
		NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"MD5CompareMismatch", true);
		dlg->SetAllowMultiSel(false);
		dlg->AddFilter(CSTR("*.md5"), CSTR("MD5 File"));
		if (dlg->ShowDialog(me->GetHandle()))
		{
			IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (fs.IsError())
			{
				me->ui->ShowMsgOK(CSTR("Error in creating file"), CSTR("Error"), me);
			}
			else
			{
				Text::StringBuilderUTF8 sb;
				MD5Comparator comparator;
				Data::Sort::ArtificialQuickSort::Sort<NN<MD5Entry>>(md5List1, comparator);
				Data::Sort::ArtificialQuickSort::Sort<NN<MD5Entry>>(md5List2, comparator);
				NN<MD5Entry> entry1;
				NN<MD5Entry> entry2;
				UIntOS i = 0;
				UIntOS j = md5List2.GetCount();
				while (i < j)
				{
					entry2 = md5List2.GetItemNoCheck(i);
					if (MD5FindEntry(md5List1, entry2->fileName->ToCString()).SetTo(entry1))
					{
						if (!entry1->md5->Equals(entry2->md5))
						{
							sb.Append(entry1->md5);
							sb.AppendUTF8Char(' ');
							sb.Append(entry1->fileName);
							sb.AppendC(UTF8STRC("\r\n"));
							if (sb.GetLength() > 65536)
							{
								fs.Write(sb.ToByteArray());
								sb.ClearStr();
							}
						}
					}
					i++;
				}
				if (sb.GetLength() > 0)
				{
					fs.Write(sb.ToByteArray());
				}
			}
		}
		dlg.Delete();
	}
	md5List1.FreeAll(FreeMD5Entry);
	md5List2.FreeAll(FreeMD5Entry);
}

void __stdcall SSWR::AVIRead::AVIRMD5CompareForm::OnFiles(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<AVIRMD5CompareForm> me = userObj.GetNN<AVIRMD5CompareForm>();
	UIntOS i = 0;
	UIntOS j = files.GetCount();
	while (i < j)
	{
		if (me->nextIs2)
		{
			me->txtFile2->SetText(files.GetItem(i)->ToCString());
			me->nextIs2 = false;
		}
		else
		{
			me->txtFile1->SetText(files.GetItem(i)->ToCString());
			me->nextIs2 = true;
		}
		i++;
	}
	if (j > 0)
	{
		me->UpdateStatus();
	}
}

void SSWR::AVIRead::AVIRMD5CompareForm::UpdateStatus()
{
	Text::StringBuilderUTF8 sbFile1;
	Text::StringBuilderUTF8 sbFile2;
	this->txtFile1->GetText(sbFile1);
	this->txtFile2->GetText(sbFile2);
	if (sbFile1.leng == 0 || sbFile2.leng == 0)
	{
		this->txtLessCount->SetText(CSTR("-"));
		this->txtMoreCount->SetText(CSTR("-"));
		this->txtMismatchCount->SetText(CSTR("-"));
		return;
	}
	Data::ArrayListNN<MD5Entry> md5List1;
	Data::ArrayListNN<MD5Entry> md5List2;
	LoadMD5(sbFile1.ToCString(), md5List1);
	LoadMD5(sbFile2.ToCString(), md5List2);
	if (md5List1.GetCount() > 0 && md5List2.GetCount() > 0)
	{
		MD5Comparator comparator;
		UIntOS mismatchCnt = 0;
		UIntOS moreCnt = 0;
		UIntOS lessCnt = 0;
		Data::Sort::ArtificialQuickSort::Sort<NN<MD5Entry>>(md5List1, comparator);
		Data::Sort::ArtificialQuickSort::Sort<NN<MD5Entry>>(md5List2, comparator);
		NN<MD5Entry> entry1;
		NN<MD5Entry> entry2;
		UIntOS i = 0;
		UIntOS j = md5List2.GetCount();
		while (i < j)
		{
			entry2 = md5List2.GetItemNoCheck(i);
			if (MD5FindEntry(md5List1, entry2->fileName->ToCString()).SetTo(entry1))
			{
				if (!entry1->md5->Equals(entry2->md5))
				{
					mismatchCnt++;
				}
			}
			else
			{
				moreCnt++;
			}
			i++;
		}
		lessCnt = md5List1.GetCount() - (md5List2.GetCount() - moreCnt);
		sbFile1.ClearStr();
		sbFile1.AppendUIntOS(moreCnt);
		this->txtMoreCount->SetText(sbFile1.ToCString());
		sbFile1.ClearStr();
		sbFile1.AppendUIntOS(lessCnt);
		this->txtLessCount->SetText(sbFile1.ToCString());
		sbFile1.ClearStr();
		sbFile1.AppendUIntOS(mismatchCnt);
		this->txtMismatchCount->SetText(sbFile1.ToCString());
	}
	else
	{
		this->txtLessCount->SetText(CSTR("-"));
		this->txtMoreCount->SetText(CSTR("-"));
		this->txtMismatchCount->SetText(CSTR("-"));
	}
	md5List1.FreeAll(FreeMD5Entry);
	md5List2.FreeAll(FreeMD5Entry);

}

SSWR::AVIRead::AVIRMD5CompareForm::AVIRMD5CompareForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 712, 200, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("MD5 Compare"));
	this->SetNoResize(true);

	this->core = core;
	this->nextIs2 = false;

	this->btnFile1 = this->ui->NewButton(*this, CSTR("File 1"));
	this->btnFile1->SetRect(4, 4, 100, 23, false);
	this->btnFile1->HandleButtonClick(OnFile1Clicked, this);
	this->txtFile1 = this->ui->NewTextBox(*this, CSTR(""));
	this->txtFile1->SetRect(104, 4, 600, 23, false);
	this->txtFile1->SetReadOnly(true);
	this->btnFile2 = this->ui->NewButton(*this, CSTR("File 2"));
	this->btnFile2->SetRect(4, 28, 100, 23, false);
	this->btnFile2->HandleButtonClick(OnFile2Clicked, this);
	this->txtFile2 = this->ui->NewTextBox(*this, CSTR(""));
	this->txtFile2->SetRect(104, 28, 600, 23, false);
	this->txtFile2->SetReadOnly(true);
	this->lblMoreCount = this->ui->NewLabel(*this, CSTR("More Count"));
	this->lblMoreCount->SetRect(4, 76, 100, 23, false);
	this->txtMoreCount = this->ui->NewTextBox(*this, CSTR(""));
	this->txtMoreCount->SetRect(104, 76, 100, 23, false);
	this->txtMoreCount->SetReadOnly(true);
	this->btnMore = this->ui->NewButton(*this, CSTR("Save"));
	this->btnMore->SetRect(204, 76, 75, 23, false);
	this->btnMore->HandleButtonClick(OnMoreClicked, this);
	this->lblLessCount = this->ui->NewLabel(*this, CSTR("Less Count"));
	this->lblLessCount->SetRect(4, 100, 100, 23, false);
	this->txtLessCount = this->ui->NewTextBox(*this, CSTR(""));
	this->txtLessCount->SetRect(104, 100, 100, 23, false);
	this->txtLessCount->SetReadOnly(true);
	this->btnLess = this->ui->NewButton(*this, CSTR("Save"));
	this->btnLess->SetRect(204, 100, 75, 23, false);
	this->btnLess->HandleButtonClick(OnLessClicked, this);
	this->lblMismatchCount = this->ui->NewLabel(*this, CSTR("Mismatch Count"));
	this->lblMismatchCount->SetRect(4, 124, 100, 23, false);
	this->txtMismatchCount = this->ui->NewTextBox(*this, CSTR(""));
	this->txtMismatchCount->SetRect(104, 124, 100, 23, false);
	this->txtMismatchCount->SetReadOnly(true);
	this->btnMismatch = this->ui->NewButton(*this, CSTR("Save"));
	this->btnMismatch->SetRect(204, 124, 75, 23, false);
	this->btnMismatch->HandleButtonClick(OnMismatchClicked, this);

	this->HandleDropFiles(OnFiles, this);
}

SSWR::AVIRead::AVIRMD5CompareForm::~AVIRMD5CompareForm()
{
}

void SSWR::AVIRead::AVIRMD5CompareForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
