#ifndef _SM_GAME_SUDOKU_SUDOKUBOARD
#define _SM_GAME_SUDOKU_SUDOKUBOARD

namespace Game
{
	namespace Sudoku
	{
		class SudokuBoard
		{
		private:
			UInt16 *board;

			void HintInit();
			void HintHCheck();
			void HintVCheck();
			void HintBoxCheck();
			void HintCheckLev1();
			Bool SolveLev1();
			Bool SolveLev1One();

			Bool SolveLev2H();
			Bool SolveLev2V();
			Bool SolveLev2Box();
			Bool SolveLev2();
			UOSInt SolveLev3();
		public:
			SudokuBoard();
			~SudokuBoard();

			void SetBoardNum(UOSInt xOfst, UOSInt yOfst, UInt8 number, Bool isDefault);
			void ToggleHints(UOSInt xOfst, UOSInt yOfst, UInt8 number);
			UInt8 GetBoardNum(UOSInt xOfst, UOSInt yOfst, OptOut<Bool> isDefault);
			UInt16 GetBoardValue(UOSInt xOfst, UOSInt yOfst);
			void CopyFrom(NN<const SudokuBoard> board);
			Bool IsFinish();

			void Clear();
			void ClearAll();
			UOSInt Solve();
			Bool SolveOne();
			void ShowHints();
		};
	}
}
#endif
