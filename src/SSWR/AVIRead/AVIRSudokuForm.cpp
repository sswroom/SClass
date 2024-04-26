#include "Stdafx.h"
#include "Text/MyString.h"
#include "SSWR/AVIRead/AVIRSudokuForm.h"

typedef enum
{
	MNU_SOLVE = 100,
	MNU_SOLVE_ONE,
	MNU_CLEAR
} MenuItem;

void __stdcall SSWR::AVIRead::AVIRSudokuForm::EventNumInput(AnyType userObj, UOSInt selX, UOSInt selY, UInt8 num)
{
	NN<SSWR::AVIRead::AVIRSudokuForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSudokuForm>();
	me->board->SetBoardNum(selX, selY, num, true);
	me->svMain->Redraw();
}

SSWR::AVIRead::AVIRSudokuForm::AVIRSudokuForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 768, ui)
{
	this->SetText(CSTR("Sudoku Solver"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASS(this->board, Game::Sudoku::SudokuBoard());
	NEW_CLASS(this->svMain, UI::GUISudokuViewer(ui, *this, this->core->GetDrawEngine(), board));
	this->svMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->svMain->HandleNumberInput(EventNumInput, this);
	NN<UI::GUIMenu> mnu;
	NEW_CLASSNN(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu(CSTR("&Board"));
	mnu->AddItem(CSTR("&Clear"), MNU_CLEAR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("&Solve"), MNU_SOLVE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("S&olve One"), MNU_SOLVE_ONE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->SetMenu(mnuMain);
	this->svMain->Focus();
}

SSWR::AVIRead::AVIRSudokuForm::~AVIRSudokuForm()
{
	DEL_CLASS(this->board);
}

void SSWR::AVIRead::AVIRSudokuForm::EventMenuClicked(UInt16 cmdId)
{
	switch (cmdId)
	{
	case MNU_SOLVE:
		this->board->Solve();
		this->svMain->Redraw();
		break;
	case MNU_SOLVE_ONE:
		this->board->Solve();
		this->svMain->Redraw();
		break;
	case MNU_CLEAR:
		this->board->Clear();
		this->svMain->Redraw();
		break;
	}
}

void SSWR::AVIRead::AVIRSudokuForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
