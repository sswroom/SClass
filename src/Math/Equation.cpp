#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Equation.h"
#include "Text/MyString.h"

Int32 Math::Equation::ParseEquation(NN<Data::ArrayListInt32> equOut, UTF8Char *equation, OSInt nChars, UTF8Char **endPos)
{
//	UTF8Char name[128];
	UTF8Char *currPtr = equation;
	UTF8Char *nextPtr;
//	UTF8Char *nameStart;
	UTF8Char c;
	Int32 ret;
	//Bool isDigit = false;
	Bool isName = false;
	Bool isNeg = false;
	while (nChars > 0)
	{
		c = *currPtr++;
		if (c == '(')
		{
			ret = ParseEquation(equOut, currPtr, nChars, &nextPtr);
			if (isName)
			{
				//////////////////////////////
				isName = false;
			}
			if (isNeg)
			{
				equOut->Add(4);
				equOut->Add(4);
				isNeg = false;
			}
			nChars -= nextPtr - currPtr;
			currPtr = nextPtr;
		}
		else if (c == ')')
		{
			if (endPos)
				*endPos = currPtr;
			////////////////////////////////////
			return 1;
		}
		else if (c == '-')
		{
		/////////////////////	if (
		}
		else if (c == '+')
		{
			//////////////////////////////
		}
		else if (c == '*')
		{
			////////////////////////////////
		}

		nChars--;
	}

	return 0;
	/////////////////////////////////////////////////////////
}

Math::Equation::Equation()
{
}

Math::Equation::~Equation()
{
	UOSInt i;
	i = this->equationVari.GetCount();
	while (i-- > 0)
	{
		this->equationVari.GetItemNoCheck(i)->Release();
	}

	this->equationLeft.DeleteAll();
	this->equationRight.DeleteAll();
}

UOSInt Math::Equation::AddEquation(UTF8Char *equation)
{
	UTF8Char *currPtr = equation;
	UTF8Char *leftStart = currPtr;
	UTF8Char *signStart = 0;
	UTF8Char *rightStart = 0;
	UTF8Char c;
	while ((c = *currPtr++) != 0)
	{
		if (c == '>' || c == '=' || c == '<')
		{
			if (signStart == 0)
			{
				signStart = currPtr - 1;
				rightStart = currPtr;
				if (c == '>' || c == '<')
				{
					if (*currPtr == '=')
					{
						rightStart = ++currPtr;
					}
				}
			}
			else
			{
				return INVALID_INDEX;
			}
		}
	}

	if (rightStart == 0)
	{
		return INVALID_INDEX;
	}

	NN<Data::ArrayListInt32> right;
	NN<Data::ArrayListInt32> left;
	NEW_CLASSNN(right, Data::ArrayListInt32());
	NEW_CLASSNN(left, Data::ArrayListInt32());
	if (ParseEquation(right, rightStart, currPtr - rightStart - 1, 0) != 0)
	{
		right.Delete();
		left.Delete();
		return INVALID_INDEX;
	}
	if (ParseEquation(left, leftStart, signStart - leftStart, 0) != 0)
	{
		right.Delete();
		left.Delete();
		return INVALID_INDEX;
	}
	Int32 sign;
	if (*signStart == '=')
	{
		sign = 0;
	}
	else if (*signStart == '>')
	{
		if (signStart[1] == '=')
		{
			sign = 1;
		}
		else
		{
			sign = 2;
		}
	}
	else if (*signStart == '<')
	{
		if (signStart[1] == '=')
		{
			sign = 3;
		}
		else
		{
			sign = 4;
		}
	}

	UOSInt ret = this->equationLeft.Add(left);
	this->equationRight.Add(right);
	this->equationSign.Add(sign);
	return ret;
}

void Math::Equation::Solve(UTF8Char *variName, UTF8Char *outBuff)
{
	//////////////////////////////////////////////
}

UTF8Char *Math::Equation::GetEquationStr(UTF8Char *buff, Int32 index)
{
	//////////////////////////////////////////////
	return 0;
}
