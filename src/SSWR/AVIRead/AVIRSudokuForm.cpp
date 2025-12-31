#include "Stdafx.h"
#include "IO/FileStream.h"
#include "SSWR/AVIRead/AVIRSudokuForm.h"
#include "SSWR/AVIRead/AVIRSudokuImportForm.h"
#include "Text/MyString.h"
#include "UI/GUIFileDialog.h"

typedef enum
{
	MNU_SOLVE3 = 100,
	MNU_SOLVE2,
	MNU_SOLVE1_ONE,
	MNU_CLEAR,
	MNU_CLEARALL,
	MNU_IMPORT_IMAGE,
	MNU_IMPORT,
	MNU_EXPORT,
	MNU_HINT_LEV1,
	MNU_HINT_LEV2,
	MNU_HINT_CLEAR,
	MNU_FIX_ALL
} MenuItem;

void __stdcall SSWR::AVIRead::AVIRSudokuForm::EventNumInput(AnyType userObj, UOSInt selX, UOSInt selY, UInt8 num)
{
	NN<SSWR::AVIRead::AVIRSudokuForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSudokuForm>();
	UOSInt mode = me->cboMode->GetSelectedItem().GetUOSInt();
	if (mode == 1)
	{
		me->board->SetBoardNum(selX, selY, num, false);
	}
	else if (mode == 2)
	{
		me->board->ToggleHints(selX, selY, num);
	}
	else
	{
		me->board->SetBoardNum(selX, selY, num, true);
	}
	me->svMain->Redraw();
}

void __stdcall SSWR::AVIRead::AVIRSudokuForm::OnHintsClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSudokuForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSudokuForm>();
	me->board->HintLev2();
	me->svMain->Redraw();
}

void __stdcall SSWR::AVIRead::AVIRSudokuForm::OnSolveClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSudokuForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSudokuForm>();
	me->board->SolveLev3();
	me->svMain->Redraw();
}

void __stdcall SSWR::AVIRead::AVIRSudokuForm::OnClearClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSudokuForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSudokuForm>();
	me->board->ClearAll();
	me->svMain->Redraw();
}

void __stdcall SSWR::AVIRead::AVIRSudokuForm::OnFileImport(AnyType userObj, Data::DataArray<NN<Text::String>> fileNames)
{
	NN<SSWR::AVIRead::AVIRSudokuForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSudokuForm>();
	NN<Text::String> fileName;
	UInt8 fileBuff[512];
	UOSInt i = 0;
	UOSInt j = fileNames.GetCount();
	while (i < j)
	{
		fileName = fileNames.GetItem(i);
		if (fileName->EndsWith(CSTR(".txt")))
		{
			IO::FileStream fs(fileNames.GetItem(i), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			UOSInt fileSize = fs.Read(BYTEARR(fileBuff));
			fileBuff[fileSize] = 0;
			if (me->board->ImportData(fileBuff))
			{
				me->svMain->Redraw();
				return;
			}
		}
		i++;
	}
}

SSWR::AVIRead::AVIRSudokuForm::AVIRSudokuForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 768, ui)
{
	this->SetText(CSTR("Sudoku Solver"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASSNN(this->board, Game::Sudoku::SudokuBoard());

	this->pnlMain = ui->NewPanel(*this);
	this->pnlMain->SetRect(0, 0, 100, 31, false);
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblMode = ui->NewLabel(this->pnlMain, CSTR("Mode"));
	this->lblMode->SetRect(4, 4, 100, 23, false);
	this->cboMode = ui->NewComboBox(this->pnlMain, false);
	this->cboMode->SetRect(104, 4, 150, 23, false);
	this->cboMode->AddItem(CSTR("Default"), (void*)0);
	this->cboMode->AddItem(CSTR("Normal"), (void*)1);
	this->cboMode->AddItem(CSTR("Hints"), (void*)2);
	this->cboMode->SetSelectedIndex(0);
	this->btnHints = ui->NewButton(this->pnlMain, CSTR("Hints"));
	this->btnHints->SetRect(260, 4, 75, 23, false);
	this->btnHints->HandleButtonClick(OnHintsClicked, this);
	this->btnSolve = ui->NewButton(this->pnlMain, CSTR("Solve"));
	this->btnSolve->SetRect(340, 4, 75, 23, false);
	this->btnSolve->HandleButtonClick(OnSolveClicked, this);
	this->btnClear = ui->NewButton(this->pnlMain, CSTR("Clear"));
	this->btnClear->SetRect(420, 4, 75, 23, false);
	this->btnClear->HandleButtonClick(OnClearClicked, this);
	NEW_CLASSNN(this->svMain, UI::GUISudokuViewer(ui, *this, this->core->GetDrawEngine(), 0, board));
	this->svMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->svMain->HandleNumberInput(EventNumInput, this);
	NN<UI::GUIMenu> mnu;
	NEW_CLASSNN(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu(CSTR("&Board"));
	mnu->AddItem(CSTR("&Import Image"), MNU_IMPORT_IMAGE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("I&mport"), MNU_IMPORT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("&Export"), MNU_EXPORT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("&Clear"), MNU_CLEAR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Clear &All"), MNU_CLEARALL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("&Fix All"), MNU_FIX_ALL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu = this->mnuMain->AddSubMenu(CSTR("&Solve"));
	mnu->AddItem(CSTR("S&olve Level 1"), MNU_SOLVE1_ONE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("S&olve Level 2"), MNU_SOLVE2, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("&Solve Level 3"), MNU_SOLVE3, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu = this->mnuMain->AddSubMenu(CSTR("&Hints"));
	mnu->AddItem(CSTR("Hints Level &1"), MNU_HINT_LEV1, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Hints Level &2"), MNU_HINT_LEV2, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("&Clear Hints"), MNU_HINT_CLEAR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->SetMenu(mnuMain);
	this->svMain->Focus();
	this->HandleDropFiles(OnFileImport, this);
}

SSWR::AVIRead::AVIRSudokuForm::~AVIRSudokuForm()
{
	this->board.Delete();
}

void SSWR::AVIRead::AVIRSudokuForm::EventMenuClicked(UInt16 cmdId)
{
	UOSInt solutionCnt;
	switch (cmdId)
	{
	case MNU_SOLVE3:
		solutionCnt = this->board->SolveLev3();
		this->svMain->Redraw();
		if (solutionCnt > 1)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(CSTR("More than 1 solutions found, total of "))->AppendUOSInt(solutionCnt)->Append(CSTR(" solutions"));
			this->ui->ShowMsgOK(sb.ToCString(), CSTR("Sudoku Solver"), this);
		}
		break;
	case MNU_SOLVE2:
		this->board->SolveLev2();
		this->svMain->Redraw();
		break;
	case MNU_SOLVE1_ONE:
		this->board->SolveLev1One();
		this->svMain->Redraw();
		break;
	case MNU_CLEAR:
		this->board->Clear();
		this->svMain->Redraw();
		break;
	case MNU_CLEARALL:
		this->board->ClearAll();
		this->svMain->Redraw();
		break;
	case MNU_IMPORT_IMAGE:
		{
			SSWR::AVIRead::AVIRSudokuImportForm frm(Optional<UI::GUIClientControl>(), this->ui, this->core, this->board);
			if (frm.ShowDialog(*this) == UI::GUIForm::DR_OK)
			{
				this->svMain->Redraw();
			}
		}
		break;
	case MNU_IMPORT:
		{
			NN<UI::GUIFileDialog> dlg = ui->NewFileDialog(L"SSWR", L"AVIRead", L"SudokuImport", false);
			dlg->AddFilter(CSTR("*.txt"), CSTR("Sudoku Board File"));
			dlg->SetAllowMultiSel(false);
			if (dlg->ShowDialog(this->GetHandle()))
			{
				NN<Text::String> fileName = dlg->GetFileName();
				UInt8 fileBuff[512];
				IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				UOSInt readSize = fs.Read(BYTEARR(fileBuff));
				fileBuff[readSize] = 0;
				if (this->board->ImportData(fileBuff))
				{
					this->svMain->Redraw();
				}
				else
				{
					this->ui->ShowMsgOK(CSTR("Error loading Sudoku board file"), CSTR("Sudoku Solver"), *this);
				}
			}
			dlg.Delete();
		}
		break;
	case MNU_EXPORT:
		{
			NN<UI::GUIFileDialog> dlg = ui->NewFileDialog(L"SSWR", L"AVIRead", L"SudokuExport", true);
			dlg->AddFilter(CSTR("*.txt"), CSTR("Sudoku Board File"));
			dlg->SetAllowMultiSel(false);
			if (dlg->ShowDialog(this->GetHandle()))
			{
				NN<Text::String> fileName = dlg->GetFileName();
				IO::FileStream fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				UInt8 fileBuff[512];
				UOSInt fileSize = this->board->ExportData(fileBuff);
				fs.Write(Data::ByteArrayR(fileBuff, fileSize));
			}
			dlg.Delete();
		}
		break;
	case MNU_HINT_LEV1:
		this->board->HintLev1();
		this->svMain->Redraw();
		break;
	case MNU_HINT_LEV2:
		this->board->HintLev2();
		this->svMain->Redraw();
		break;
	case MNU_HINT_CLEAR:
		this->board->ClearHints();
		this->svMain->Redraw();
		break;
	case MNU_FIX_ALL:
		if (this->board->FixAll())
		{
			this->svMain->Redraw();
		}
		break;
	}
}

void SSWR::AVIRead::AVIRSudokuForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
