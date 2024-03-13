#ifndef _SM_SSWR_AVIREAD_AVIRSUDOKUFORM
#define _SM_SSWR_AVIREAD_AVIRSUDOKUFORM
#include "Game/Sudoku/SudokuBoard.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUISudokuViewer.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSudokuForm : public UI::GUIForm
		{
		private:
			UI::GUISudokuViewer *svMain;
			UI::GUIMainMenu *mnuMain;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Game::Sudoku::SudokuBoard *board;

			static void __stdcall EventNumInput(void *userObj, UOSInt selX, UOSInt selY, UInt8 num);
		public:
			AVIRSudokuForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSudokuForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
