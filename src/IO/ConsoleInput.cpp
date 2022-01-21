#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Console.h"
#include "IO/ConsoleInput.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

IO::ConsoleInput::InputReturnType IO::ConsoleInput::InputInt32(IO::ConsoleWriter *console, Int32 *output, Bool showOriVal)
{
	IO::ConsoleWriter::ConsoleState state;
	if (console->IsFileOutput())
	{
		WChar sbuff[256];
		console->ReadLine(sbuff, 256);
		if (Text::StrEquals(sbuff, L"0"))
		{
			*output = 0;
			return IRT_ENTER;
		}
		else
		{
			*output = Text::StrToInt32(sbuff);
			if (*output == 0)
			{
				return IRT_ESCAPE;
			}
			else
			{
				return IRT_ENTER;
			}
		}
	}

	console->GetConsoleState(&state);
	console->SetTextColor(IO::ConsoleWriter::CC_WHITE, IO::ConsoleWriter::CC_DARK_BLUE);

	if ((state.consoleWidth - state.currX) < 11)
	{
		console->WriteLine();
		state.currX = 0;
		state.currY += 1;
	}
	console->WriteStrC(UTF8STRC("           "));
	console->SetCursorPos(state.currX, state.currY);

	UTF8Char cbuff[12];
	UOSInt currPos = 0;
	UOSInt currSize = 0;
	Int32 i;
	UOSInt j;
	if (showOriVal)
	{
		currSize = (UOSInt)(Text::StrInt32(cbuff, *output) - cbuff);
		currPos = currSize;
		console->WriteStrC(cbuff, currSize);
	}
	while (true)
	{
		i = IO::Console::GetKey();
		if (i == 0x1b)
			break;
		if (i == 0x0d)
			break;
		if (i == 9)
			break;
		else if (i == 0x2d)
		{
			if (currPos == 0)
			{
				if (currSize >= 11)
				{
				
				}
				else if (currSize > 0 && cbuff[0] == '-')
				{
				}
				else
				{
					j = currSize;
					cbuff[j + 1] = 0;
					while (j-- > 0)
					{
						cbuff[j + 1] = cbuff[j];
					}
					cbuff[0] = '-';
					currPos++;
					currSize++;
					console->SetCursorPos(state.currX, state.currY);
					console->WriteStrC(cbuff, currSize);
					console->SetCursorPos(state.currX + (UInt32)currPos, state.currY);
				}
			}
		}
		else if (i >= 0x30 && i <= 0x39)
		{
			if (currSize >= 11)
			{
			}
			else if (currPos == 0 && currSize > 0 && cbuff[0] == '-')
			{
			}
			else if (currSize == 10 && cbuff[0] != '-')
			{
			}
			else 
			{
				j = currSize;
				cbuff[j + 1] = 0;
				while (j-- > currPos)
				{
					cbuff[j + 1] = cbuff[j];
				}
				cbuff[currPos] = (UTF8Char)i;
				currPos++;
				currSize++;
				console->SetCursorPos(state.currX, state.currY);
				console->WriteStrC(cbuff, currSize);
				console->SetCursorPos(state.currX + (UInt32)currPos, state.currY);
			}
		}
		else if (i == 8)
		{
			if (currPos == 0)
			{
			}
			else
			{
				j = currPos;
				while (++j < currSize)
				{
					cbuff[j - 1] = cbuff[j];
				}
				currSize--;
				currPos--;
				cbuff[currSize] = 0;

				console->SetCursorPos(state.currX, state.currY);
				console->WriteStrC(cbuff, currSize);
				console->WriteStrC(UTF8STRC(" "));
				console->SetCursorPos(state.currX + (UInt32)currPos, state.currY);
			}
		}
		else if (i == 0 || i == 0xe0)
		{
			i = IO::Console::GetKey();
			if (i == 0x4b)
			{
				if (currPos > 0)
				{
					currPos--;
					console->SetCursorPos(state.currX + (UInt32)currPos, state.currY);
				}
			}
			else if (i == 0x4d)
			{
				if (currPos < currSize)
				{
					currPos++;
					console->SetCursorPos(state.currX + (UInt32)currPos, state.currY);
				}
			}
		}
		else
		{
			i = i + 1;
		}
	}

	console->ResetTextColor();
//	console->SetTextColor(state.fgColor, state.bgColor);
	console->WriteLine();
	if (i == 0x1b)
		return IRT_ESCAPE;
	if (currSize == 0)
	{
		if (i == 0xd)
			return IRT_ENTEREMPTY;
		else
			return IRT_TABEMPTY;
	}
	*output = Text::StrToInt32(cbuff);
	if (i == 0xd)
		return IRT_ENTER;
	else
		return IRT_TAB;
}

IO::ConsoleInput::InputReturnType IO::ConsoleInput::InputBool(IO::ConsoleWriter *console, Bool *output)
{
	IO::ConsoleWriter::ConsoleState state;
	if (console->IsFileOutput())
	{
		WChar sbuff[256];
		console->ReadLine(sbuff, 256);
		if (sbuff[0] == 'T' || sbuff[0] == 't')
		{
			*output = true;
			return IRT_ENTER;
		}
		else if (sbuff[0] == 'F' || sbuff[0] == 'f')
		{
			*output = false;
			return IRT_ENTER;
		}
		else 
		{
			*output = Text::StrToInt32(sbuff) != 0;
			return IRT_ENTER;
		}
	}

	console->GetConsoleState(&state);
	console->SetTextColor(IO::ConsoleWriter::CC_WHITE, IO::ConsoleWriter::CC_DARK_BLUE);

	if ((state.consoleWidth - state.currX) < 5)
	{
		console->WriteLine();
		state.currX = 0;
		state.currY += 1;
	}

	Bool o = *output;
	Int32 i;
	while (true)
	{
		if (o)
		{
			console->WriteStrC(UTF8STRC("True "));
		}
		else
		{
			console->WriteStrC(UTF8STRC("False"));
		}
		console->SetCursorPos(state.currX, state.currY);

		i = IO::Console::GetKey();
		if (i == 0x1b)
			break;
		if (i == 0x0d)
			break;
		if (i == 9)
			break;
		if (i == 0 || i == 0xe0)
		{
			i = IO::Console::GetKey();
			if (i == 0x48 || i == 0x4b || i == 0x4d || i == 0x50)
			{
				o = !o;
			}
		}
		else if (i == 0x46 || i == 0x66)
		{
			o = false;
		}
		else if (i == 0x54 || i == 0x74)
		{
			o = true;
		}
	}

	console->ResetTextColor();
//	console->SetTextColor(state.fgColor, state.bgColor);
	console->WriteLine();
	if (i == 0x1b)
		return IRT_ESCAPE;
	*output = o;
	if (i == 0xd)
		return IRT_ENTER;
	else
		return IRT_TAB;
}

IO::ConsoleInput::InputReturnType IO::ConsoleInput::InputSelect(IO::ConsoleWriter *console, Text::String **names, UOSInt nNames, UOSInt *selection)
{
	IO::ConsoleWriter::ConsoleState state;
	UOSInt i;
	UOSInt k;
	UOSInt maxStrSize;
	UOSInt j = 0;
	if (nNames <= 0 || console->IsFileOutput())
		return IRT_UNKNOWN;
	maxStrSize = 0;
	i = nNames;
	while (i-- > 0)
	{
		j = names[i]->leng;
		if (j > maxStrSize)
		{
			maxStrSize = j;
		}
	}
	if (j <= 0)
		return IRT_UNKNOWN;
	k = *selection;
	if (k < 0)
		k = 0;
	if (k >= nNames)
		k = nNames - 1;

	console->GetConsoleState(&state);
	console->SetTextColor(IO::ConsoleWriter::CC_WHITE, IO::ConsoleWriter::CC_DARK_BLUE);

	if ((state.consoleWidth - state.currX) < maxStrSize)
	{
		console->WriteLine();
		state.currX = 0;
		state.currY += 1;
	}

	while (true)
	{
		console->WriteStrC(names[k]->v, names[k]->leng);
		j = (maxStrSize - names[k]->leng);
		while (j-- > 0)
		{
			console->WriteStrC(UTF8STRC(" "));
		}
		console->SetCursorPos(state.currX, state.currY);

		i = (UOSInt)IO::Console::GetKey();
		if (i == 0x1b)
			break;
		if (i == 0x0d)
			break;
		if (i == 9)
			break;
		if (i == 0 || i == 0xe0)
		{
			i = (UOSInt)IO::Console::GetKey();
			if (i == 0x48)
			{
				if (k == 0)
				{
					k = nNames - 1;
				}
				else 
				{
					k = k - 1;
				}
			}
			else if (i == 0x50)
			{
				if (++k >= nNames)
				{
					k = 0;
				}
			}
		}
		else if (i >= 0x30 && i <= 0x39)
		{
			j = k;
			while (++j < nNames)
			{
				if (names[j]->v[0] == (UTF8Char)i)
				{
					k = j;
					break;
				}
			}
			if (j >= nNames)
			{
				j = 0;
				while (j < k)
				{
					if (names[j]->v[0] == (UTF8Char)i)
					{
						k = j;
						break;
					}
					j++;
				}
			}
		}
		else if (i >= 'A' && i <= 'Z')
		{
			j = k;
			while (++j < nNames)
			{
				if (names[j]->v[0] == (UTF8Char)i || names[j]->v[0] == (UTF8Char)(i + 32))
				{
					k = j;
					break;
				}
			}
			if (j >= nNames)
			{
				j = 0;
				while (j < k)
				{
					if (names[j]->v[0] == (UTF8Char)i || names[j]->v[0] == (UTF8Char)(i + 32))
					{
						k = j;
						break;
					}
					j++;
				}
			}
		}
		else if (i >= 'a' && i <= 'z')
		{
			j = k;
			while (++j < nNames)
			{
				if (names[j]->v[0] == (UTF8Char)i || names[j]->v[0] == (UTF8Char)(i - 32))
				{
					k = j;
					break;
				}
			}
			if (j >= nNames)
			{
				j = 0;
				while (j < k)
				{
					if (names[j]->v[0] == (UTF8Char)i || names[j]->v[0] == (UTF8Char)(i - 32))
					{
						k = j;
						break;
					}
					j++;
				}
			}
		}
	}
	console->ResetTextColor();
//	console->SetTextColor(state.fgColor, state.bgColor);
	console->WriteLine();
	if (i == 0x1b)
		return IRT_ESCAPE;
	*selection = k;
	if (i == 0xd)
		return IRT_ENTER;
	else
		return IRT_TAB;
}

IO::ConsoleInput::InputReturnType IO::ConsoleInput::InputDateTime(IO::ConsoleWriter *console, Data::DateTime *output)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	IO::ConsoleWriter::ConsoleState state;
	if (output == 0)
		return IRT_UNKNOWN;
	if (console->IsFileOutput())
	{
		WChar sbuff2[256];
		console->ReadLine(sbuff2, 256);
		sptr = Text::StrWChar_UTF8(sbuff, sbuff2);
		output->SetValue(sbuff, (UOSInt)(sptr - sbuff));
		return IRT_ENTER;
	}
	Int32 values[6];
	Int32 currMax;
	Int32 currMin;
	Int32 currPos = 0;

	Int32 i;
	values[0] = output->GetYear();
	values[1] = output->GetMonth();
	values[2] = output->GetDay();
	values[3] = output->GetHour();
	values[4] = output->GetMinute();
	values[5] = output->GetSecond();

	console->GetConsoleState(&state);

	if ((state.consoleWidth - state.currX) < 19)
	{
		console->WriteLine();
		state.currX = 0;
		state.currY += 1;
	}

	while (true)
	{
		console->SetCursorPos(state.currX, state.currY);
		console->SetTextColor(IO::ConsoleWriter::CC_WHITE, IO::ConsoleWriter::CC_BLUE);
		sptr = Text::StrInt32(sbuff, values[0]);
		i = 4 - (Int32)(sptr - sbuff);
		while (i-- > 0)
		{
			*sptr++ = ' ';
		}
		*sptr = 0;
		console->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		console->ResetTextColor();
//		console->SetTextColor(state.fgColor, state.bgColor);
		console->WriteStrC(UTF8STRC("-"));
		console->SetTextColor(IO::ConsoleWriter::CC_WHITE, IO::ConsoleWriter::CC_BLUE);
		sptr = Text::StrInt32(sbuff, values[1]);
		if ((sptr - sbuff) == 1)
			console->WriteStrC(UTF8STRC(" "));
		console->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		console->ResetTextColor();
//		console->SetTextColor(state.fgColor, state.bgColor);
		console->WriteStrC(UTF8STRC("-"));
		console->SetTextColor(IO::ConsoleWriter::CC_WHITE, IO::ConsoleWriter::CC_BLUE);
		sptr = Text::StrInt32(sbuff, values[2]);
		if ((sptr - sbuff) == 1)
			console->WriteStrC(UTF8STRC(" "));
		console->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		console->ResetTextColor();
//		console->SetTextColor(state.fgColor, state.bgColor);
		console->WriteStrC(UTF8STRC(" "));
		console->SetTextColor(IO::ConsoleWriter::CC_WHITE, IO::ConsoleWriter::CC_BLUE);
		sptr = Text::StrInt32(sbuff, values[3]);
		if ((sptr - sbuff) == 1)
			console->WriteStrC(UTF8STRC(" "));
		console->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		console->ResetTextColor();
//		console->SetTextColor(state.fgColor, state.bgColor);
		console->WriteStrC(UTF8STRC(":"));
		console->SetTextColor(IO::ConsoleWriter::CC_WHITE, IO::ConsoleWriter::CC_BLUE);
		sptr = Text::StrInt32(sbuff, values[4]);
		if ((sptr - sbuff) == 1)
			console->WriteStrC(UTF8STRC(" "));
		console->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		console->ResetTextColor();
//		console->SetTextColor(state.fgColor, state.bgColor);
		console->WriteStrC(UTF8STRC(":"));
		console->SetTextColor(IO::ConsoleWriter::CC_WHITE, IO::ConsoleWriter::CC_BLUE);
		sptr = Text::StrInt32(sbuff, values[5]);
		if ((sptr - sbuff) == 1)
			console->WriteStrC(UTF8STRC(" "));
		console->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));

		if (currPos == 0)
		{
			currMax = 9999;
			currMin = 1;
			console->SetCursorPos(state.currX, state.currY);
		}
		else if (currPos == 1)
		{
			currMax = 12;
			currMin = 1;
			console->SetCursorPos(state.currX + 5, state.currY);
		}
		else if (currPos == 2)
		{
			currMax = Data::DateTime::DayInMonth((UInt16)values[0], (UInt8)values[1]);
			currMin = 1;
			console->SetCursorPos(state.currX + 8, state.currY);
		}
		else if (currPos == 3)
		{
			currMax = 23;
			currMin = 0;
			console->SetCursorPos(state.currX + 11, state.currY);
		}
		else if (currPos == 4)
		{
			currMax = 59;
			currMin = 0;
			console->SetCursorPos(state.currX + 14, state.currY);
		}
		else if (currPos == 5)
		{
			currMax = 59;
			currMin = 0;
			console->SetCursorPos(state.currX + 17, state.currY);
		}
		else
		{
			currMin = 0;
			currMax = 0;
		}

		i = IO::Console::GetKey();
		if (i == 0x1b)
			break;
		if (i == 0x0d)
			break;
		if (i == 9)
			break;
		if (i == 0 || i == 0xe0)
		{
			if (values[currPos] < currMin)
				values[currPos] = currMin;
			i = IO::Console::GetKey();
			if (i == 0x48)
			{
				if (++(values[currPos]) > currMax)
					values[currPos] = currMin;
			}
			else if (i == 0x50)
			{
				if (--(values[currPos]) < currMin)
					values[currPos] = currMax;
			}
			else if (i == 0x4b)
			{
				if (--currPos < 0)
					currPos = 5;
			}
			else if (i == 0x4d)
			{
				if (++currPos >= 6)
					currPos = 0;
			}
		}
		else if (i >= 0x30 && i <= 0x39)
		{
			values[currPos] = values[currPos] * 10 + (i - 0x30);
			if (values[currPos] > currMax)
				values[currPos] = i - 0x30;
		}
	}

	console->ResetTextColor();
//	console->SetTextColor(state.fgColor, state.bgColor);
	console->WriteLine();
	if (i == 0x1b)
		return IRT_ESCAPE;
	if (values[currPos] < currMin)
		values[currPos] = currMin;
	if (values[2] > Data::DateTime::DayInMonth((UInt16)values[0], (UInt8)values[1]))
		values[2] = Data::DateTime::DayInMonth((UInt16)values[0], (UInt8)values[1]);
	output->SetValue((UInt16)values[0], (UInt8)values[1], (UInt8)values[2], (UInt8)values[3], (UInt8)values[4], (UInt8)values[5], 0);
	if (i == 0xd)
		return IRT_ENTER;
	else
		return IRT_TAB;
}

IO::ConsoleInput::InputReturnType IO::ConsoleInput::InputHexBytes(IO::ConsoleWriter *console, UInt8 *buff, UOSInt buffSize, UOSInt *inputSize)
{
	IO::ConsoleWriter::ConsoleState state;
	UOSInt i;
	UOSInt j;

	if (console->IsFileOutput())
	{
		WChar sbuff[256];
		console->ReadLine(sbuff, buffSize * 2);
		*inputSize = Text::StrHex2Bytes(sbuff, buff);
		return IRT_ENTER;
	}

	console->GetConsoleState(&state);
	console->SetTextColor(IO::ConsoleWriter::CC_WHITE, IO::ConsoleWriter::CC_BLUE);

	if ((state.consoleWidth - state.currX) < buffSize * 2)
	{
		console->WriteLine();
		state.currX = 0;
		state.currY += 1;
	}
	i = buffSize;
	while (i-- > 0)
	{
		console->WriteStrC(UTF8STRC("  "));
	}
	console->SetCursorPos(state.currX, state.currY);

	UTF8Char *cbuff = MemAlloc(UTF8Char, (buffSize << 1) + 1);
	UInt32 currPos = 0;
	UInt32 currSize = 0;
	while (true)
	{
		i = (UOSInt)IO::Console::GetKey();
		if (i == 0x1b)
			break;
		if (i == 0x0d)
			break;
		if (i == 9)
			break;
		else if (i >= 0x30 && i <= 0x39)
		{
			if (currSize >= buffSize * 2)
			{
			}
			else 
			{
				j = currSize;
				cbuff[j + 1] = 0;
				while (j-- > currPos)
				{
					cbuff[j + 1] = cbuff[j];
				}
				cbuff[currPos] = (UTF8Char)i;
				currPos++;
				currSize++;
				console->SetCursorPos(state.currX, state.currY);
				console->WriteStrC(cbuff, currSize);
				console->SetCursorPos(state.currX + currPos, state.currY);
			}
		}
		else if (i >= 0x41 && i <= 0x46)
		{
			if (currSize >= buffSize * 2)
			{
			}
			else 
			{
				j = currSize;
				cbuff[j + 1] = 0;
				while (j-- > currPos)
				{
					cbuff[j + 1] = cbuff[j];
				}
				cbuff[currPos] = (UTF8Char)i;
				currPos++;
				currSize++;
				console->SetCursorPos(state.currX, state.currY);
				console->WriteStrC(cbuff, currSize);
				console->SetCursorPos(state.currX + currPos, state.currY);
			}
		}
		else if (i >= 0x61 && i <= 0x66)
		{
			if (currSize >= buffSize * 2)
			{
			}
			else 
			{
				j = currSize;
				cbuff[j + 1] = 0;
				while (j-- > currPos)
				{
					cbuff[j + 1] = cbuff[j];
				}
				cbuff[currPos] = (UTF8Char)(i - 32);
				currPos++;
				currSize++;
				console->SetCursorPos(state.currX, state.currY);
				console->WriteStrC(cbuff, currSize);
				console->SetCursorPos(state.currX + currPos, state.currY);
			}
		}
		else if (i == 8)
		{
			if (currPos == 0)
			{
			}
			else
			{
				j = currPos;
				while (++j < currSize)
				{
					cbuff[j - 1] = cbuff[j];
				}
				currSize--;
				currPos--;
				cbuff[currSize] = 0;

				console->SetCursorPos(state.currX, state.currY);
				console->WriteStrC(cbuff, currSize);
				console->WriteStrC(UTF8STRC(" "));
				console->SetCursorPos(state.currX + currPos, state.currY);
			}
		}
		else if (i == 0 || i == 0xe0)
		{
			i = (UOSInt)IO::Console::GetKey();
			if (i == 0x4b)
			{
				if (currPos > 0)
				{
					currPos--;
					console->SetCursorPos(state.currX + currPos, state.currY);
				}
			}
			else if (i == 0x4d)
			{
				if (currPos < currSize)
				{
					currPos++;
					console->SetCursorPos(state.currX + currPos, state.currY);
				}
			}
		}
		else
		{
			i = i + 1;
		}
	}

	console->ResetTextColor();
//	console->SetTextColor(state.fgColor, state.bgColor);
	console->WriteLine();
	if (i == 0x1b)
	{
		MemFree(cbuff);
		return IRT_ESCAPE;
	}
	if (currSize == 0)
	{
		MemFree(cbuff);
		*inputSize = 0;
		if (i == 0xd)
			return IRT_ENTEREMPTY;
		else
			return IRT_TABEMPTY;
	}
	*inputSize = Text::StrHex2Bytes(cbuff, buff);
	MemFree(cbuff);
	if (i == 0xd)
		return IRT_ENTER;
	else
		return IRT_TAB;
}


IO::ConsoleInput::InputReturnType IO::ConsoleInput::InputString(IO::ConsoleWriter *console, UTF8Char *output, UOSInt maxCharCnt, UOSInt *inputSize)
{
	IO::ConsoleWriter::ConsoleState state;
	UOSInt i;
	UOSInt j;

	if (console->IsFileOutput())
	{
		WChar sbuff[256];
		*inputSize = (UOSInt)(console->ReadLine(sbuff, maxCharCnt) - sbuff);
		return IRT_ENTER;
	}

	console->GetConsoleState(&state);
	console->SetTextColor(IO::ConsoleWriter::CC_WHITE, IO::ConsoleWriter::CC_BLUE);

	if ((state.consoleWidth - state.currX) < maxCharCnt)
	{
		console->WriteLine();
		state.currX = 0;
		state.currY += 1;
	}
	i = maxCharCnt;
	while (i-- > 0)
	{
		console->WriteStrC(UTF8STRC(" "));
	}
	console->SetCursorPos(state.currX, state.currY);

	UTF8Char *cbuff = output;
	UInt32 currPos = 0;
	UInt32 currSize = 0;
	while (true)
	{
		i = (UOSInt)IO::Console::GetKey();
		if (i == 0x1b)
			break;
		if (i == 0x0d)
			break;
		if (i == 9)
			break;

		if (i == 8)
		{
			if (currPos == 0)
			{
			}
			else
			{
				j = currPos;
				while (j < currSize)
				{
					cbuff[j - 1] = cbuff[j];
					j++;
				}
				currSize--;
				currPos--;
				cbuff[currSize] = 0;

				console->SetCursorPos(state.currX, state.currY);
				console->WriteStrC(cbuff, currSize);
				console->WriteStrC(UTF8STRC(" "));
				console->SetCursorPos(state.currX + currPos, state.currY);
			}
		}
		else if (i == 0 || i == 0xe0)
		{
			i = (UOSInt)IO::Console::GetKey();
			if (i == 0x4b)
			{
				if (currPos > 0)
				{
					currPos--;
					console->SetCursorPos(state.currX + currPos, state.currY);
				}
			}
			else if (i == 0x4d)
			{
				if (currPos < currSize)
				{
					currPos++;
					console->SetCursorPos(state.currX + currPos, state.currY);
				}
			}
		}
		else
		{
			if (currSize >= maxCharCnt)
			{
			}
			else 
			{
				j = currSize;
				cbuff[j + 1] = 0;
				while (j-- > currPos)
				{
					cbuff[j + 1] = cbuff[j];
				}
				cbuff[currPos] = (UTF8Char)i;
				currPos++;
				currSize++;
				console->SetCursorPos(state.currX, state.currY);
				console->WriteStrC(cbuff, currSize);
				console->SetCursorPos(state.currX + currPos, state.currY);
			}
		}
	}

	console->ResetTextColor();
//	console->SetTextColor(state.fgColor, state.bgColor);
	console->WriteLine();
	if (i == 0x1b)
	{
		return IRT_ESCAPE;
	}
	if (currSize == 0)
	{
		*inputSize = 0;
		if (i == 0xd)
			return IRT_ENTEREMPTY;
		else
			return IRT_TABEMPTY;
	}
	*inputSize = currSize;
	if (i == 0xd)
		return IRT_ENTER;
	else
		return IRT_TAB;
}

