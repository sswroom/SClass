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
			void HintUpdate(UIntOS xOfst, UIntOS yOfst, UInt16 mask);
			Bool SolveLev1();

			Bool SolveLev2H();
			Bool SolveLev2V();
			Bool SolveLev2Box();
			UIntOS SolveLev3Inner(UIntOS initXOfst, UIntOS initYOfst, NN<SudokuBoard> succBoard);

			Bool HasHError(UIntOS xOfst, UIntOS yOfst) const;
			Bool HasVError(UIntOS xOfst, UIntOS yOfst) const;
			Bool HasBoxError(UIntOS xOfst, UIntOS yOfst) const;
		public:
			SudokuBoard();
			SudokuBoard(const SudokuBoard &board);
			~SudokuBoard();

			void SetBoardNum(UIntOS xOfst, UIntOS yOfst, UInt8 number, Bool isDefault);
			void ToggleHints(UIntOS xOfst, UIntOS yOfst, UInt8 number);
			UInt8 GetBoardNum(UIntOS xOfst, UIntOS yOfst, OptOut<Bool> isDefault) const;
			UInt16 GetBoardValue(UIntOS xOfst, UIntOS yOfst) const;
			void CopyFrom(NN<const SudokuBoard> board);
			Bool IsFinish() const;
			Bool IsEmpty() const;
			Bool HasError(UIntOS xOfst, UIntOS yOfst) const;
			UIntOS ExportData(UnsafeArray<UInt8> boardOut) const;
			Bool ImportData(UnsafeArray<const UInt8> boardIn);

			void Clear();
			void ClearAll();
			Bool FixAll();
			Bool SolveLev1One();
			Bool SolveLev2();
			UIntOS SolveLev3();
			void HintLev1();
			void HintLev2();
			void ClearHints();
			void PrintBoard() const;
		};
	}
}
#endif
