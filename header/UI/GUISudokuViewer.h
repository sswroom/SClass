#ifndef _SM_UI_GUISUDOKUVIEWER
#define _SM_UI_GUISUDOKUVIEWER
#include "Game/Sudoku/SudokuBoard.h"
#include "UI/GUICustomDraw.h"

namespace UI
{
	class GUISudokuViewer : public UI::GUICustomDraw
	{
	public:
		typedef void (CALLBACKFUNC NumberInputEvent)(AnyType userObj, UIntOS x, UIntOS y, UInt8 num);

	private:
		NN<Game::Sudoku::SudokuBoard> board;
		UIntOS selX;
		UIntOS selY;
		Data::ArrayListObj<NumberInputEvent> numInputHdlr;
		Data::ArrayListObj<AnyType> numInputObj;

	public:
		GUISudokuViewer(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Optional<Media::ColorSess> colorSess, NN<Game::Sudoku::SudokuBoard> board);
		virtual ~GUISudokuViewer();

		virtual UI::EventState OnMouseDown(Math::Coord2D<IntOS> scnPos, MouseButton btn);
		virtual UI::EventState OnKeyDown(UI::GUIControl::GUIKey key);
		virtual void OnDraw(NN<Media::DrawImage> img);

		void HandleNumberInput(NumberInputEvent hdlr, AnyType userObj);

	};
}
#endif
