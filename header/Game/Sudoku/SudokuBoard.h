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
			Bool SolveLev3();
		public:
			SudokuBoard();
			~SudokuBoard();

			void SetBoardNum(OSInt xOfst, OSInt yOfst, UInt8 number, Bool isDefault);
			UInt8 GetBoardNum(OSInt xOfst, OSInt yOfst, Bool *isDefault);
			void CopyFrom(SudokuBoard *board);
			Bool IsFinish();

			void Clear();
			Bool Solve();
			Bool SolveOne();
		};
	};
};
#endif
