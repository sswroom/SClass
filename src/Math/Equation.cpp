#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Equation.h"
#include "Text/MyString.h"

Int32 Math::Equation::ParseEquation(NN<Data::ArrayListInt32> equOut, UnsafeArray<UTF8Char> equation, IntOS nChars, OptOut<UnsafeArray<UTF8Char>> endPos)
{
//	UTF8Char name[128];
	UnsafeArray<UTF8Char> currPtr = equation;
	UnsafeArray<UTF8Char> nextPtr;
//	UnsafeArray<UTF8Char> nameStart;
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
			ret = ParseEquation(equOut, currPtr, nChars, nextPtr);
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
			endPos.Set(currPtr);
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
	UIntOS i;
	i = this->equationVari.GetCount();
	while (i-- > 0)
	{
		this->equationVari.GetItemNoCheck(i)->Release();
	}

	this->equationLeft.DeleteAll();
	this->equationRight.DeleteAll();
}

UIntOS Math::Equation::AddEquation(UnsafeArray<UTF8Char> equation)
{
	UnsafeArray<UTF8Char> currPtr = equation;
	UnsafeArray<UTF8Char> leftStart = currPtr;
	UnsafeArrayOpt<UTF8Char> signStart = nullptr;
	UnsafeArrayOpt<UTF8Char> rightStart = nullptr;
	UTF8Char c;
	while ((c = *currPtr++) != 0)
	{
		if (c == '>' || c == '=' || c == '<')
		{
			if (signStart.IsNull())
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

	UnsafeArray<UTF8Char> nnrightStart;
	UnsafeArray<UTF8Char> nnsignStart;
	if (!rightStart.SetTo(nnrightStart) || !signStart.SetTo(nnsignStart))
	{
		return INVALID_INDEX;
	}

	NN<Data::ArrayListInt32> right;
	NN<Data::ArrayListInt32> left;
	NEW_CLASSNN(right, Data::ArrayListInt32());
	NEW_CLASSNN(left, Data::ArrayListInt32());
	if (ParseEquation(right, nnrightStart, currPtr - nnrightStart - 1, 0) != 0)
	{
		right.Delete();
		left.Delete();
		return INVALID_INDEX;
	}
	if (ParseEquation(left, leftStart, nnsignStart - leftStart, 0) != 0)
	{
		right.Delete();
		left.Delete();
		return INVALID_INDEX;
	}
	Int32 sign;
	if (*nnsignStart == '=')
	{
		sign = 0;
	}
	else if (*nnsignStart == '>')
	{
		if (nnsignStart[1] == '=')
		{
			sign = 1;
		}
		else
		{
			sign = 2;
		}
	}
	else if (*nnsignStart == '<')
	{
		if (nnsignStart[1] == '=')
		{
			sign = 3;
		}
		else
		{
			sign = 4;
		}
	}

	UIntOS ret = this->equationLeft.Add(left);
	this->equationRight.Add(right);
	this->equationSign.Add(sign);
	return ret;
}

void Math::Equation::Solve(UnsafeArray<UTF8Char> variName, UnsafeArray<UTF8Char> outBuff)
{
	//////////////////////////////////////////////
}

UnsafeArrayOpt<UTF8Char> Math::Equation::GetEquationStr(UnsafeArray<UTF8Char> buff, Int32 index)
{
	//////////////////////////////////////////////
	return nullptr;
}
