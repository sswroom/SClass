#ifndef _SM_UI_GUISUDOKUVIEWER
#define _SM_UI_GUISUDOKUVIEWER
#include "Game/Sudoku/SudokuBoard.h"
#include "UI/GUICustomDraw.h"

namespace UI
{
	class GUISudokuViewer : public UI::GUICustomDraw
	{
	public:
		typedef void (__stdcall *NumberInputEvent)(void *userObj, UOSInt x, UOSInt y, UInt8 num);

	private:
		Game::Sudoku::SudokuBoard *board;
		UOSInt selX;
		UOSInt selY;
		Data::ArrayList<NumberInputEvent> numInputHdlr;
		Data::ArrayList<void *> numInputObj;

	public:
		GUISudokuViewer(NotNullPtr<GUICore> ui, UI::GUIClientControl *parent, NotNullPtr<Media::DrawEngine> eng, Game::Sudoku::SudokuBoard *board);
		virtual ~GUISudokuViewer();

		virtual Bool OnMouseDown(Math::Coord2D<OSInt> scnPos, MouseButton btn);
		virtual Bool OnKeyDown(UI::GUIControl::GUIKey key);
		virtual void OnDraw(NotNullPtr<Media::DrawImage> img);

		void HandleNumberInput(NumberInputEvent hdlr, void *userObj);

	};
}
#endif
