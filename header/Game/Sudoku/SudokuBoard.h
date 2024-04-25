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

			void SetBoardNum(UOSInt xOfst, UOSInt yOfst, UInt8 number, Bool isDefault);
			UInt8 GetBoardNum(UOSInt xOfst, UOSInt yOfst, OptOut<Bool> isDefault);
			void CopyFrom(NN<const SudokuBoard> board);
			Bool IsFinish();

			void Clear();
			Bool Solve();
			Bool SolveOne();
		};
	};
};
#endif
