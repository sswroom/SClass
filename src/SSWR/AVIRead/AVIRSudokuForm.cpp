#include "Stdafx.h"
#include "Text/MyString.h"
#include "SSWR/AVIRead/AVIRSudokuForm.h"
#include "SSWR/AVIRead/AVIRSudokuImportForm.h"

typedef enum
{
	MNU_SOLVE = 100,
	MNU_SOLVE_ONE,
	MNU_CLEAR,
	MNU_CLEARALL,
	MNU_IMPORT
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
	me->board->ShowHints();
	me->svMain->Redraw();
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
	NEW_CLASSNN(this->svMain, UI::GUISudokuViewer(ui, *this, this->core->GetDrawEngine(), 0, board));
	this->svMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->svMain->HandleNumberInput(EventNumInput, this);
	NN<UI::GUIMenu> mnu;
	NEW_CLASSNN(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu(CSTR("&Board"));
	mnu->AddItem(CSTR("&Import"), MNU_IMPORT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("&Clear"), MNU_CLEAR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Clear &All"), MNU_CLEARALL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("&Solve"), MNU_SOLVE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("S&olve One"), MNU_SOLVE_ONE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->SetMenu(mnuMain);
	this->svMain->Focus();
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
	case MNU_SOLVE:
		solutionCnt = this->board->Solve();
		this->svMain->Redraw();
		if (solutionCnt > 1)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(CSTR("More than 1 solutions found, total of "))->AppendUOSInt(solutionCnt)->Append(CSTR(" solutions"));
			this->ui->ShowMsgOK(sb.ToCString(), CSTR("Sudoku Solver"), this);
		}
		break;
	case MNU_SOLVE_ONE:
		this->board->SolveOne();
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
	case MNU_IMPORT:
		{
			SSWR::AVIRead::AVIRSudokuImportForm frm(Optional<UI::GUIClientControl>(), this->ui, this->core, this->board);
			if (frm.ShowDialog(*this) == UI::GUIForm::DR_OK)
			{
				this->svMain->Redraw();
			}
		}
		break;
	}
}

void SSWR::AVIRead::AVIRSudokuForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
