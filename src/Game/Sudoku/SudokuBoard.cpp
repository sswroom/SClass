#include "Stdafx.h"
#include "MyMemory.h"
#include "Game/Sudoku/SudokuBoard.h"

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

void Game::Sudoku::SudokuBoard::HintCheckLev1()
{
	this->HintInit();
	this->HintHCheck();
	this->HintVCheck();
	this->HintBoxCheck();
}

Bool Game::Sudoku::SudokuBoard::SolveLev1()
{
	UInt16 v;
	Bool changed;
	Bool modified;
	changed = false;
	modified = true;
	while (modified)
	{
		modified = false;
		HintCheckLev1();
		UOSInt i = 81;
		while (i-- > 0)
		{
			v = this->board[i];
			if (v == 0x20)
			{
				this->board[i] = 1;
				modified = true;
			}
			else if (v == 0x40)
			{
				this->board[i] = 2;
				modified = true;
			}
			else if (v == 0x80)
			{
				this->board[i] = 3;
				modified = true;
			}
			else if (v == 0x100)
			{
				this->board[i] = 4;
				modified = true;
			}
			else if (v == 0x200)
			{
				this->board[i] = 5;
				modified = true;
			}
			else if (v == 0x400)
			{
				this->board[i] = 6;
				modified = true;
			}
			else if (v == 0x800)
			{
				this->board[i] = 7;
				modified = true;
			}
			else if (v == 0x1000)
			{
				this->board[i] = 8;
				modified = true;
			}
			else if (v == 0x2000)
			{
				this->board[i] = 9;
				modified = true;
			}
		}
		if (modified)
			changed = true;
	}
	return changed;
}

Bool Game::Sudoku::SudokuBoard::SolveLev1One()
{
	UInt16 v;
	Bool changed;
	changed = false;

	HintCheckLev1();
	OSInt i = 81;
	while (i-- > 0 && !changed)
	{
		v = this->board[i];
		if (v == 0x20)
		{
			this->board[i] = 1;
			changed = true;
		}
		else if (v == 0x40)
		{
			this->board[i] = 2;
			changed = true;
		}
		else if (v == 0x80)
		{
			this->board[i] = 3;
			changed = true;
		}
		else if (v == 0x100)
		{
			this->board[i] = 4;
			changed = true;
		}
		else if (v == 0x200)
		{
			this->board[i] = 5;
			changed = true;
		}
		else if (v == 0x400)
		{
			this->board[i] = 6;
			changed = true;
		}
		else if (v == 0x800)
		{
			this->board[i] = 7;
			changed = true;
		}
		else if (v == 0x1000)
		{
			this->board[i] = 8;
			changed = true;
		}
		else if (v == 0x2000)
		{
			this->board[i] = 9;
			changed = true;
		}
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

Bool Game::Sudoku::SudokuBoard::SolveLev3()
{
	Bool changed;
	Bool modified;
	Bool fin;
	UInt16 v;
	changed = this->SolveLev2();
	if (this->IsFinish())
	{
		return changed;
	}
	UOSInt i;
	UOSInt j;
	UInt8 k;
	Game::Sudoku::SudokuBoard *tmpBoard;
	NEW_CLASS(tmpBoard, Game::Sudoku::SudokuBoard());
	fin = false;
	i = 9;
	while (i-- > 0 && !fin)
	{
		j = 9;
		while (j-- > 0)
		{
			v = this->board[i + j * 9];
			if ((v & 15) == 0)
			{
				k = 10;
				while (k-- > 1)
				{
					if (v & (16 << k))
					{
						tmpBoard->CopyFrom(this);
						tmpBoard->SetBoardNum(i, j, k, false);
						modified = tmpBoard->Solve();
						if (modified && tmpBoard->IsFinish())
						{
							this->CopyFrom(tmpBoard);
							changed = true;
							fin = true;
							break;
						}
					}
				}
			}
		}
	}

	DEL_CLASS(tmpBoard);
	return changed;
}

Game::Sudoku::SudokuBoard::SudokuBoard()
{
	this->board = MemAlloc(UInt16, 81);
	Clear();
}

Game::Sudoku::SudokuBoard::~SudokuBoard()
{
	MemFree(this->board);
}

void Game::Sudoku::SudokuBoard::SetBoardNum(UOSInt xOfst, UOSInt yOfst, UInt8 number, Bool isDefault)
{
	UInt16 v = number;
	if (isDefault)
	{
		v |= 16;
	}
	this->board[yOfst * 9 + xOfst] = v;
}

UInt8 Game::Sudoku::SudokuBoard::GetBoardNum(UOSInt xOfst, UOSInt yOfst, Bool *isDefault)
{
	UInt16 v = this->board[yOfst * 9 + xOfst];
	if (isDefault)
	{
		*isDefault = (v & 16) != 0;
	}
	return v & 15;
}

void Game::Sudoku::SudokuBoard::CopyFrom(SudokuBoard *board)
{
	MemCopyNO(this->board, board->board, 81 * sizeof(*this->board));
}

Bool Game::Sudoku::SudokuBoard::IsFinish()
{
	OSInt i = 81;
	while (i-- > 0)
	{
		if ((this->board[i] & 15) == 0)
			return false;
	}
	return true;
}

void Game::Sudoku::SudokuBoard::Clear()
{
	OSInt i;
	i = 81;
	while (i-- > 0)
	{
		this->board[i] = 0;
	}
}

Bool Game::Sudoku::SudokuBoard::Solve()
{
	return SolveLev3();
}

Bool Game::Sudoku::SudokuBoard::SolveOne()
{
	return SolveLev1One();
}
