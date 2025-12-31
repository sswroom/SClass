#include "Stdafx.h"
#include "MyMemory.h"
#include "Game/Sudoku/SudokuBoard.h"
#include "Text/MyString.h"

void Game::Sudoku::SudokuBoard::HintInit()
{
	OSInt i = 81;
	while (i-- > 0)
	{
		if (this->board[i] & 15)
		{
			this->board[i] = this->board[i] & 31;
		}
		else
		{
			this->board[i] = this->board[i] | 0x3fe0;
		}
	}
}

void Game::Sudoku::SudokuBoard::HintHCheck()
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt currOfst;
	Bool numExist[10];
	UInt16 v;
	currOfst = 0;
	i = 9;
	while (i-- > 0)
	{
		k = 10;
		while (k-- > 0)
		{
			numExist[k] = false;
		}
		j = 9;
		while (j-- > 0)
		{
			numExist[this->board[currOfst + j] & 0xf] = true;
		}

		k = 10;
		while (k-- > 1)
		{
			if (numExist[k])
			{
				v = (UInt16)~(16 << k);
				j = 9;
				while (j-- > 0)
				{
					this->board[currOfst + j] = this->board[currOfst + j] & v;
				}
			}
		}

		currOfst += 9;
	}
}

void Game::Sudoku::SudokuBoard::HintVCheck()
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt currOfst;
	Bool numExist[10];
	UInt16 v;
	currOfst = 0;
	i = 9;
	while (i-- > 0)
	{
		k = 10;
		while (k-- > 0)
		{
			numExist[k] = false;
		}
		j = 9;
		while (j-- > 0)
		{
			numExist[this->board[currOfst + j * 9] & 0xf] = true;
		}

		k = 10;
		while (k-- > 1)
		{
			if (numExist[k])
			{
				v = (UInt16)~(16 << k);
				j = 9;
				while (j-- > 0)
				{
					this->board[currOfst + j * 9] = (UInt16)(this->board[currOfst + j * 9] & v);
				}
			}
		}

		currOfst += 1;
	}
}

void Game::Sudoku::SudokuBoard::HintBoxCheck()
{
	UOSInt i1;
	UOSInt i2;
	UOSInt j1;
	UOSInt j2;
	UOSInt k;
	UOSInt currOfst;
	UOSInt currOfst2;
	Bool numExist[10];
	UInt16 v;
	currOfst = 0;
	i1 = 3;
	while (i1-- > 0)
	{
		currOfst2 = currOfst;
		j1 = 3;
		while (j1-- > 0)
		{
			k = 10;
			while (k-- > 0)
			{
				numExist[k] = false;
			}
			i2 = 3;
			while (i2-- > 0)
			{
				j2 = 3;
				while (j2-- > 0)
				{
					numExist[this->board[currOfst2 + i2 + j2 * 9] & 0xf] = true;
				}
			}

			k = 10;
			while (k-- > 1)
			{
				if (numExist[k])
				{
					v = (UInt16)~(16 << k);
					i2 = 3;
					while (i2-- > 0)
					{
						j2 = 3;
						while (j2-- > 0)
						{
							this->board[currOfst2 + i2 + j2 * 9] = (UInt16)(this->board[currOfst2 + i2 + j2 * 9] & v);
						}
					}
				}
			}
			currOfst2+= 3;
		}
		currOfst += 27;
	}
}

void Game::Sudoku::SudokuBoard::HintUpdate(UOSInt xOfst, UOSInt yOfst, UInt16 mask)
{
	UOSInt y;
	UOSInt x;
	UOSInt boxXOfst;
	UOSInt boxYOfst;

	x = 0;
	while (x < 9)
	{
		if (this->board[yOfst * 9 + x] & mask)
		{
			this->board[yOfst * 9 + x] &= (UInt16)(~mask);
		}
		x++;
	}

	y = 0;
	while (y < 9)
	{
		if (this->board[y * 9 + xOfst] & mask)
		{
			this->board[y * 9 + xOfst] &= (UInt16)(~mask);
		}
		y++;
	}

	boxXOfst = xOfst / 3 * 3;
	boxYOfst = yOfst / 3 * 3;
	y = 0;
	while (y < 3)
	{
		x = 0;
		while (x < 3)
		{
			if (this->board[(boxYOfst + y) * 9 + boxXOfst + x] & mask)
			{
				this->board[(boxYOfst + y) * 9 + boxXOfst + x] &= (UInt16)(~mask);
			}
			x++;
		}
		y++;
	}
}

Bool Game::Sudoku::SudokuBoard::SolveLev1()
{
	UInt16 v;
	Bool changed;
	Bool modified;
	changed = false;
	HintLev2();
	modified = true;
	while (modified)
	{
		modified = false;
		UOSInt i = 81;
		while (i-- > 0)
		{
			v = this->board[i];
			if (v == 0x20)
			{
				this->board[i] = 1;
				this->HintUpdate(i % 9, i / 9, 0x20);
				modified = true;
			}
			else if (v == 0x40)
			{
				this->board[i] = 2;
				this->HintUpdate(i % 9, i / 9, 0x40);
				modified = true;
			}
			else if (v == 0x80)
			{
				this->board[i] = 3;
				this->HintUpdate(i % 9, i / 9, 0x80);
				modified = true;
			}
			else if (v == 0x100)
			{
				this->board[i] = 4;
				this->HintUpdate(i % 9, i / 9, 0x100);
				modified = true;
			}
			else if (v == 0x200)
			{
				this->board[i] = 5;
				this->HintUpdate(i % 9, i / 9, 0x200);
				modified = true;
			}
			else if (v == 0x400)
			{
				this->board[i] = 6;
				this->HintUpdate(i % 9, i / 9, 0x400);
				modified = true;
			}
			else if (v == 0x800)
			{
				this->board[i] = 7;
				this->HintUpdate(i % 9, i / 9, 0x800);
				modified = true;
			}
			else if (v == 0x1000)
			{
				this->board[i] = 8;
				this->HintUpdate(i % 9, i / 9, 0x1000);
				modified = true;
			}
			else if (v == 0x2000)
			{
				this->board[i] = 9;
				this->HintUpdate(i % 9, i / 9, 0x2000);
				modified = true;
			}
		}
		if (modified)
			changed = true;
	}
	return changed;
}

Bool Game::Sudoku::SudokuBoard::SolveLev2H()
{
	UInt8 cnts[10];
	UInt16 v;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt currOfst;
	Bool modified = false;
	currOfst = 0;
	i = 9;
	while (i-- > 0)
	{
		k = 10;
		while (k-- > 1)
		{
			cnts[k] = 0;
		}
		j = 9;
		while (j-- > 0)
		{
			v = this->board[currOfst + j];
			if (v & 0x20)
			{
				cnts[1]++;
			}
			if (v & 0x40)
			{
				cnts[2]++;
			}
			if (v & 0x80)
			{
				cnts[3]++;
			}
			if (v & 0x100)
			{
				cnts[4]++;
			}
			if (v & 0x200)
			{
				cnts[5]++;
			}
			if (v & 0x400)
			{
				cnts[6]++;
			}
			if (v & 0x800)
			{
				cnts[7]++;
			}
			if (v & 0x1000)
			{
				cnts[8]++;
			}
			if (v & 0x2000)
			{
				cnts[9]++;
			}
		}

		k = 10;
		while (k-- > 1)
		{
			if (cnts[k] == 1)
			{
				v = (UInt16)(16 << k);
				j = 9;
				while (j-- > 0)
				{
					if (this->board[currOfst + j] & v)
					{
						this->board[currOfst + j] = (UInt16)k;
						modified = true;
						break;
					}
				}
			}
		}
		currOfst += 9;
	}
	return modified;
}

Bool Game::Sudoku::SudokuBoard::SolveLev2V()
{
	UInt8 cnts[10];
	UInt16 v;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt currOfst;
	Bool modified = false;
	currOfst = 0;
	i = 9;
	while (i-- > 0)
	{
		k = 10;
		while (k-- > 1)
		{
			cnts[k] = 0;
		}
		j = 9;
		while (j-- > 0)
		{
			v = this->board[currOfst + j * 9];
			if (v & 0x20)
			{
				cnts[1]++;
			}
			if (v & 0x40)
			{
				cnts[2]++;
			}
			if (v & 0x80)
			{
				cnts[3]++;
			}
			if (v & 0x100)
			{
				cnts[4]++;
			}
			if (v & 0x200)
			{
				cnts[5]++;
			}
			if (v & 0x400)
			{
				cnts[6]++;
			}
			if (v & 0x800)
			{
				cnts[7]++;
			}
			if (v & 0x1000)
			{
				cnts[8]++;
			}
			if (v & 0x2000)
			{
				cnts[9]++;
			}
		}

		k = 10;
		while (k-- > 1)
		{
			if (cnts[k] == 1)
			{
				v = (UInt16)(16 << k);
				j = 9;
				while (j-- > 0)
				{
					if (this->board[currOfst + j * 9] & v)
					{
						this->board[currOfst + j * 9] = (UInt16)k;
						modified = true;
						break;
					}
				}
			}
		}
		currOfst++;
	}
	return modified;
}

Bool Game::Sudoku::SudokuBoard::SolveLev2Box()
{
	UInt8 cnts[10];
	UInt16 v;
	UOSInt i1;
	UOSInt i2;
	UOSInt j1;
	UOSInt j2;
	UOSInt k;
	UOSInt currOfst;
	UOSInt currOfst2;
	Bool modified = false;
	currOfst = 0;
	i1 = 3;
	while (i1-- > 0)
	{
		currOfst2 = currOfst;
		j1 = 3;
		while (j1-- > 0)
		{
			k = 10;
			while (k-- > 1)
			{
				cnts[k] = 0;
			}
			i2 = 3;
			while (i2-- > 0)
			{
				j2 = 3;
				while (j2-- > 0)
				{
					v = this->board[currOfst2 + j2 * 9 + i2];
					if (v & 0x20)
					{
						cnts[1]++;
					}
					if (v & 0x40)
					{
						cnts[2]++;
					}
					if (v & 0x80)
					{
						cnts[3]++;
					}
					if (v & 0x100)
					{
						cnts[4]++;
					}
					if (v & 0x200)
					{
						cnts[5]++;
					}
					if (v & 0x400)
					{
						cnts[6]++;
					}
					if (v & 0x800)
					{
						cnts[7]++;
					}
					if (v & 0x1000)
					{
						cnts[8]++;
					}
					if (v & 0x2000)
					{
						cnts[9]++;
					}
				}
			}

			k = 10;
			while (k-- > 1)
			{
				if (cnts[k] == 1)
				{
					v = (UInt16)(16 << k);
					i2 = 3;
					while (i2-- > 0)
					{
						j2 = 3;
						while (j2-- > 0)
						{
							if (this->board[currOfst2 + j2 * 9 + i2] & v)
							{
								this->board[currOfst2 + j2 * 9 + i2] = (UInt16)k;
								modified = true;
								break;
							}
						}
					}
				}
			}
			currOfst2 += 3;
		}
		currOfst += 27;
	}
	return modified;
}

UOSInt Game::Sudoku::SudokuBoard::SolveLev3Inner(UOSInt initXOfst, UOSInt initYOfst, NN<SudokuBoard> succBoard)
{
	UOSInt modified;
	UInt16 v;
	this->SolveLev2();
	if (this->IsFinish())
	{
//		printf("--------------------------------\r\n");
//		printf("Init Pos: %d, %d\r\n", (UInt32)initXOfst, (UInt32)initYOfst);
//		this->PrintBoard();
		return 1;
	}
	UOSInt solutionCnt = 0;
	UOSInt x;
	UOSInt y;
	UInt8 num;
	Game::Sudoku::SudokuBoard tmpBoard;
	x = initXOfst;
	while (x-- > 0)
	{
		y = initYOfst;
		initYOfst = 9;
		while (y-- > 0)
		{
			v = this->board[x + y * 9];
			if ((v & 15) == 0)
			{
				num = 10;
				while (num-- > 1)
				{
					if (v & (16 << num) && num != succBoard->GetBoardNum(x, y, nullptr))
					{
						tmpBoard.CopyFrom(*this);
						tmpBoard.SetBoardNum(x, y, num, false);
						if (!tmpBoard.HasError(x, y))
						{
							modified = tmpBoard.SolveLev3Inner(x + 1, y, succBoard);
							if (modified && tmpBoard.IsFinish())
							{
								succBoard->CopyFrom(tmpBoard);
								solutionCnt += modified;
							}
						}
					}
				}
				break;
			}
		}
	}
	this->CopyFrom(succBoard);
	return solutionCnt;
}

Bool Game::Sudoku::SudokuBoard::HasHError(UOSInt xOfst, UOSInt yOfst) const
{
	UInt8 num = GetBoardNum(xOfst, yOfst, nullptr);
	if (num == 0)
	{
		return false;
	}
	UOSInt i = 0;
	while (i < 9)
	{
		if (i != xOfst)
		{
			if (GetBoardNum(i, yOfst, nullptr) == num)
			{
				return true;
			}
		}
		i++;
	}
	return false;
}

Bool Game::Sudoku::SudokuBoard::HasVError(UOSInt xOfst, UOSInt yOfst) const
{
	UInt8 num = GetBoardNum(xOfst, yOfst, nullptr);
	if (num == 0)
	{
		return false;
	}
	UOSInt i = 0;
	while (i < 9)
	{
		if (i != yOfst)
		{
			if (GetBoardNum(xOfst, i, nullptr) == num)
			{
				return true;
			}
		}
		i++;
	}
	return false;
}

Bool Game::Sudoku::SudokuBoard::HasBoxError(UOSInt xOfst, UOSInt yOfst) const
{
	UInt8 num = GetBoardNum(xOfst, yOfst, nullptr);
	if (num == 0)
	{
		return false;
	}
	UOSInt boxX = (xOfst / 3) * 3;
	UOSInt boxY = (yOfst / 3) * 3;
	UOSInt i = 0;
	while (i < 3)
	{
		UOSInt j = 0;
		while (j < 3)
		{
			if ((boxX + i) != xOfst || (boxY + j) != yOfst)
			{
				if (GetBoardNum(boxX + i, boxY + j, nullptr) == num)
				{
					return true;
				}
			}
			j++;
		}
		i++;
	}
	return false;
}

Game::Sudoku::SudokuBoard::SudokuBoard()
{
	this->board = MemAllocArr(UInt16, 81);
	ClearAll();
}

Game::Sudoku::SudokuBoard::SudokuBoard(const SudokuBoard &board)
{
	this->board = MemAllocArr(UInt16, 81);
	this->board.CopyFromNO(board.board, 81);
}

Game::Sudoku::SudokuBoard::~SudokuBoard()
{
	MemFreeArr(this->board);
}

void Game::Sudoku::SudokuBoard::SetBoardNum(UOSInt xOfst, UOSInt yOfst, UInt8 number, Bool isDefault)
{
	if (number == 0)
	{
		this->board[yOfst * 9 + xOfst] = 0;
	}
	else
	{
		UInt16 v = number;
		if (isDefault)
		{
			v |= 16;
		}
		this->board[yOfst * 9 + xOfst] = v;
		this->HintUpdate(xOfst, yOfst, (UInt16)(16 << number));
	}
}

void Game::Sudoku::SudokuBoard::ToggleHints(UOSInt xOfst, UOSInt yOfst, UInt8 number)
{
	UInt16 v = this->board[yOfst * 9 + xOfst];
	if ((v & 15) == 0)
	{
		this->board[yOfst * 9 + xOfst] = v ^ (16 << number);
	}	
}

UInt8 Game::Sudoku::SudokuBoard::GetBoardNum(UOSInt xOfst, UOSInt yOfst, OptOut<Bool> isDefault) const
{
	UInt16 v = this->board[yOfst * 9 + xOfst];
	isDefault.Set((v & 16) != 0);
	return v & 15;
}

UInt16 Game::Sudoku::SudokuBoard::GetBoardValue(UOSInt xOfst, UOSInt yOfst) const
{
	return this->board[yOfst * 9 + xOfst];
}

void Game::Sudoku::SudokuBoard::CopyFrom(NN<const SudokuBoard> board)
{
	this->board.CopyFromNO(board->board, 81);
}

Bool Game::Sudoku::SudokuBoard::IsFinish() const
{
	UOSInt i = 0;
	while (i < 9)
	{
		UOSInt j = 0;
		while (j < 9)
		{
			if (GetBoardNum(i, j, nullptr) == 0)
				return false;
			if (HasError(i, j))
				return false;
			j++;
		}
		i++;
	}
	return true;
}

Bool Game::Sudoku::SudokuBoard::IsEmpty() const
{
	OSInt i = 81;
	while (i-- > 0)
	{
		if (this->board[i] != 0)
			return false;
	}
	return true;
}

Bool Game::Sudoku::SudokuBoard::HasError(UOSInt xOfst, UOSInt yOfst) const
{
	UInt16 v = this->board[yOfst * 9 + xOfst];
	if ((v & 15) == 0 || (v & 16) != 0)
	{
		return false;
	}
	return HasHError(xOfst, yOfst) || HasVError(xOfst, yOfst) || HasBoxError(xOfst, yOfst);
}

UOSInt Game::Sudoku::SudokuBoard::ExportData(UnsafeArray<UInt8> boardOut) const
{
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrConcatC(boardOut, UTF8STRC("Sudoku\r\n"));
	UOSInt y = 0;
	while (y < 9)
	{
		UOSInt x = 0;
		while (x < 9)
		{
			UInt8 num = this->GetBoardNum(x, y, nullptr);
			*sptr++ = (num == 0) ? ' ' : (UTF8Char)('0' + num);
			x++;
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC("\r\n"));
		y++;
	}
	return (UOSInt)(sptr - boardOut);
}

Bool Game::Sudoku::SudokuBoard::ImportData(UnsafeArray<const UInt8> boardIn)
{
	UInt16 newBoard[81];
	if (!Text::StrStartsWithC(boardIn, 8, UTF8STRC("Sudoku\r\n")))
	{
		return false;
	}
	boardIn += 8;
	UOSInt y = 0;
	while (y < 9)
	{
		UOSInt x = 0;
		while (x < 9)
		{
			if (boardIn[0] == ' ')
			{
				newBoard[y * 9 + x] = 0;
			}
			else if (boardIn[0] >= '1' && boardIn[0] <= '9')
			{
				newBoard[y * 9 + x] = (UInt16)(16 | (boardIn[0] - '0'));
			}
			else
			{
				return false;
			}
			x++;
			boardIn++;
		}
		if (boardIn[0] != '\r' || boardIn[1] != '\n')
		{
			return false;
		}
		boardIn += 2;
		y++;
	}
	if (boardIn[0] != 0)
	{
		return false;
	}
	this->board.CopyFromNO(newBoard, 81);
	return true;
}

void Game::Sudoku::SudokuBoard::Clear()
{
	OSInt i;
	i = 81;
	while (i-- > 0)
	{
		if ((this->board[i] & 16) == 0)
		{
			this->board[i] = 0;
		}
	}
}

void Game::Sudoku::SudokuBoard::ClearAll()
{
	UOSInt i;
	i = 81;
	while (i-- > 0)
	{
		this->board[i] = 0;
	}
}

Bool Game::Sudoku::SudokuBoard::FixAll()
{
	Bool changed = false;
	UOSInt i = 81;
	while (i-- > 0)
	{
		if ((this->board[i] & 15) != 0 && (this->board[i] & 16) == 0)
		{
			this->board[i] |= 16;
			changed = true;
		}
	}
	return changed;
}

Bool Game::Sudoku::SudokuBoard::SolveLev1One()
{
	UInt16 v;
	Bool changed;
	changed = false;

	HintLev2();
	UOSInt i = 81;
	while (i-- > 0 && !changed)
	{
		v = this->board[i];
		if (v == 0x20)
		{
			this->board[i] = 1;
			this->HintUpdate(i % 9, i / 9, 0x20);
			changed = true;
		}
		else if (v == 0x40)
		{
			this->board[i] = 2;
			this->HintUpdate(i % 9, i / 9, 0x40);
			changed = true;
		}
		else if (v == 0x80)
		{
			this->board[i] = 3;
			this->HintUpdate(i % 9, i / 9, 0x80);
			changed = true;
		}
		else if (v == 0x100)
		{
			this->board[i] = 4;
			this->HintUpdate(i % 9, i / 9, 0x100);
			changed = true;
		}
		else if (v == 0x200)
		{
			this->board[i] = 5;
			this->HintUpdate(i % 9, i / 9, 0x200);
			changed = true;
		}
		else if (v == 0x400)
		{
			this->board[i] = 6;
			this->HintUpdate(i % 9, i / 9, 0x400);
			changed = true;
		}
		else if (v == 0x800)
		{
			this->board[i] = 7;
			this->HintUpdate(i % 9, i / 9, 0x800);
			changed = true;
		}
		else if (v == 0x1000)
		{
			this->board[i] = 8;
			this->HintUpdate(i % 9, i / 9, 0x1000);
			changed = true;
		}
		else if (v == 0x2000)
		{
			this->board[i] = 9;
			this->HintUpdate(i % 9, i / 9, 0x2000);
			changed = true;
		}
	}
	return changed;
}

Bool Game::Sudoku::SudokuBoard::SolveLev2()
{
	Bool changed;
	Bool modified;
	changed = false;
	modified = true;
	while (modified)
	{
		modified = SolveLev1() || SolveLev2H() || SolveLev2V() || SolveLev2Box();
		if (modified)
		{
			changed = true;
		}
	}
	return changed;
}

UOSInt Game::Sudoku::SudokuBoard::SolveLev3()
{
	Game::Sudoku::SudokuBoard succBoard(*this);
	UOSInt solutionCnt = this->SolveLev3Inner(9, 9, succBoard);
	this->CopyFrom(succBoard);
	return solutionCnt;
}

void Game::Sudoku::SudokuBoard::HintLev1()
{
	this->HintInit();
	this->HintHCheck();
	this->HintVCheck();
	this->HintBoxCheck();
}

void Game::Sudoku::SudokuBoard::HintLev2()
{
	Bool xExists[3];
	Bool yExists[3];
	this->HintLev1();
	UOSInt boxX = 0;
	while (boxX < 3)
	{
		UOSInt boxY = 0;
		while (boxY < 3)
		{
			UOSInt num = 1;
			while (num <= 9)
			{
				xExists[0] = false;
				xExists[1] = false;
				xExists[2] = false;
				yExists[0] = false;
				yExists[1] = false;
				yExists[2] = false;
				UOSInt x = 0;
				while (x < 3)
				{
					UOSInt y = 0;
					while (y < 3)
					{
						UOSInt currX = boxX * 3 + x;
						UOSInt currY = boxY * 3 + y;
						UInt16 v = this->board[currY * 9 + currX];
						if (v & (16 << num))
						{
							xExists[x] = true;
							yExists[y] = true;
						}	
						y++;
					}
					x++;
				}

				if (yExists[0] && !yExists[1] && !yExists[2])
				{
					UOSInt currY = boxY * 3 + 0;
					UOSInt x = 0;
					while (x < 9)
					{
						if (x / 3 != boxX)
						{
							this->board[currY * 9 + x] &= (UInt16)(~(16 << num));
						}
						x++;
					}
				}
				else if (!yExists[0] && yExists[1] && !yExists[2])
				{
					UOSInt currY = boxY * 3 + 1;
					UOSInt x = 0;
					while (x < 9)
					{
						if (x / 3 != boxX)
						{
							this->board[currY * 9 + x] &= (UInt16)(~(16 << num));
						}
						x++;
					}
				}
				else if (!yExists[0] && !yExists[1] && yExists[2])
				{
					UOSInt currY = boxY * 3 + 2;
					UOSInt x = 0;
					while (x < 9)
					{
						if (x / 3 != boxX)
						{
							this->board[currY * 9 + x] &= (UInt16)(~(16 << num));
						}
						x++;
					}
				}
				if (xExists[0] && !xExists[1] && !xExists[2])
				{
					UOSInt currX = boxX * 3 + 0;
					UOSInt y = 0;
					while (y < 9)
					{
						if (y / 3 != boxY)
						{
							this->board[y * 9 + currX] &= (UInt16)(~(16 << num));
						}
						y++;
					}
				}
				else if (!xExists[0] && xExists[1] && !xExists[2])
				{
					UOSInt currX = boxX * 3 + 1;
					UOSInt y = 0;
					while (y < 9)
					{
						if (y / 3 != boxY)
						{
							this->board[y * 9 + currX] &= (UInt16)(~(16 << num));
						}
						y++;
					}
				}
				else if (!xExists[0] && !xExists[1] && xExists[2])
				{
					UOSInt currX = boxX * 3 + 2;
					UOSInt y = 0;
					while (y < 9)
					{
						if (y / 3 != boxY)
						{
							this->board[y * 9 + currX] &= (UInt16)(~(16 << num));
						}
						y++;
					}
				}
				num++;
			}
			boxY++;
		}
		boxX++;
	}
}

void Game::Sudoku::SudokuBoard::ClearHints()
{
	UOSInt i = 0;
	while (i < 81)
	{
		this->board[i] &= 0x1f;
		i++;
	}
}

void Game::Sudoku::SudokuBoard::PrintBoard() const
{
	UTF8Char sbuff[32];
	sbuff[9] = '\r';
	sbuff[10] = '\n';
	sbuff[11] = 0;
	UOSInt y = 0;
	while (y < 9)
	{
		UOSInt x = 0;
		while (x < 9)
		{
			UOSInt v = this->board[y * 9 + x] & 0xf;
			if (v == 0)
			{
				sbuff[x] = ' ';
			}
			else
			{
				sbuff[x] = (UTF8Char)('0' + v);
			}
			x++;
		}
		printf("%s", sbuff);
		y++;
	}
}