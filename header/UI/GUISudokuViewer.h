#ifndef _SM_UI_GUISUDOKUVIEWER
#define _SM_UI_GUISUDOKUVIEWER
#include "Game/Sudoku/SudokuBoard.h"
#include "UI/GUICustomDraw.h"

namespace UI
{
	class GUISudokuViewer : public UI::GUICustomDraw
	{
	public:
		typedef void (CALLBACKFUNC NumberInputEvent)(AnyType userObj, UOSInt x, UOSInt y, UInt8 num);

	private:
		Game::Sudoku::SudokuBoard *board;
		UOSInt selX;
		UOSInt selY;
		Data::ArrayList<NumberInputEvent> numInputHdlr;
		Data::ArrayList<AnyType> numInputObj;

	public:
		GUISudokuViewer(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Game::Sudoku::SudokuBoard *board);
		virtual ~GUISudokuViewer();

		virtual Bool OnMouseDown(Math::Coord2D<OSInt> scnPos, MouseButton btn);
		virtual Bool OnKeyDown(UI::GUIControl::GUIKey key);
		virtual void OnDraw(NN<Media::DrawImage> img);

		void HandleNumberInput(NumberInputEvent hdlr, AnyType userObj);

	};
}
#endif
