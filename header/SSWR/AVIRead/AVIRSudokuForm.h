#ifndef _SM_SSWR_AVIREAD_AVIRSUDOKUFORM
#define _SM_SSWR_AVIREAD_AVIRSUDOKUFORM
#include "Game/Sudoku/SudokuBoard.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIMainMenu.h"
#include "UI/GUIPanel.h"
#include "UI/GUISudokuViewer.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSudokuForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIPanel> pnlMain;
			NN<UI::GUILabel> lblMode;
			NN<UI::GUIComboBox> cboMode;
			NN<UI::GUIButton> btnHints;
			NN<UI::GUIButton> btnSolve;
			NN<UI::GUIButton> btnClear;
			NN<UI::GUISudokuViewer> svMain;
			NN<UI::GUIMainMenu> mnuMain;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Game::Sudoku::SudokuBoard> board;

			static void __stdcall EventNumInput(AnyType userObj, UIntOS selX, UIntOS selY, UInt8 num);
			static void __stdcall OnHintsClicked(AnyType userObj);
			static void __stdcall OnSolveClicked(AnyType userObj);
			static void __stdcall OnClearClicked(AnyType userObj);
			static void __stdcall OnFileImport(AnyType userObj, Data::DataArray<NN<Text::String>> fileNames);
		public:
			AVIRSudokuForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSudokuForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
