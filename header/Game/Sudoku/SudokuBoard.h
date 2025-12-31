#ifndef _SM_GAME_SUDOKU_SUDOKUBOARD
#define _SM_GAME_SUDOKU_SUDOKUBOARD

namespace Game
{
	namespace Sudoku
	{
		class SudokuBoard
		{
		private:
			UnsafeArray<UInt16> board;

			void HintInit();
			void HintHCheck();
			void HintVCheck();
			void HintBoxCheck();
			void HintUpdate(UOSInt xOfst, UOSInt yOfst, UInt16 mask);
			Bool SolveLev1();

			Bool SolveLev2H();
			Bool SolveLev2V();
			Bool SolveLev2Box();
			UOSInt SolveLev3Inner(UOSInt initXOfst, UOSInt initYOfst, NN<SudokuBoard> succBoard);

			Bool HasHError(UOSInt xOfst, UOSInt yOfst) const;
			Bool HasVError(UOSInt xOfst, UOSInt yOfst) const;
			Bool HasBoxError(UOSInt xOfst, UOSInt yOfst) const;
		public:
			SudokuBoard();
			SudokuBoard(const SudokuBoard &board);
			~SudokuBoard();

			void SetBoardNum(UOSInt xOfst, UOSInt yOfst, UInt8 number, Bool isDefault);
			void ToggleHints(UOSInt xOfst, UOSInt yOfst, UInt8 number);
			UInt8 GetBoardNum(UOSInt xOfst, UOSInt yOfst, OptOut<Bool> isDefault) const;
			UInt16 GetBoardValue(UOSInt xOfst, UOSInt yOfst) const;
			void CopyFrom(NN<const SudokuBoard> board);
			Bool IsFinish() const;
			Bool IsEmpty() const;
			Bool HasError(UOSInt xOfst, UOSInt yOfst) const;
			UOSInt ExportData(UnsafeArray<UInt8> boardOut) const;
			Bool ImportData(UnsafeArray<const UInt8> boardIn);

			void Clear();
			void ClearAll();
			Bool FixAll();
			Bool SolveLev1One();
			Bool SolveLev2();
			UOSInt SolveLev3();
			void HintLev1();
			void HintLev2();
			void ClearHints();
			void PrintBoard() const;
		};
	}
}
#endif
