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
			NN<UI::GUISudokuViewer> svMain;
			NN<UI::GUIMainMenu> mnuMain;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Game::Sudoku::SudokuBoard> board;

			static void __stdcall EventNumInput(AnyType userObj, UOSInt selX, UOSInt selY, UInt8 num);
		public:
			AVIRSudokuForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSudokuForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
