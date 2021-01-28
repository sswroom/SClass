#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Equation.h"
#include "Text/MyString.h"

Int32 Math::Equation::ParseEquation(Data::ArrayListInt *equOut, WChar *equation, OSInt nChars, WChar **endPos)
{
//	WChar name[128];
	WChar *currPtr = equation;
	WChar *nextPtr;
//	WChar *nameStart;
	WChar c;
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
	NEW_CLASS(equationLeft, Data::ArrayList<Data::ArrayListInt*>());
	NEW_CLASS(equationRight, Data::ArrayList<Data::ArrayListInt*>());
	NEW_CLASS(equationSign, Data::ArrayListInt());
	NEW_CLASS(equationVari, Data::ArrayList<WChar*>());
}

Math::Equation::~Equation()
{
	OSInt i;
	i = equationVari->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew((WChar*)equationVari->GetItem(i));
	}
	DEL_CLASS(equationVari);

	i = equationLeft->GetCount();
	while (i-- > 0)
	{
		DEL_CLASS((Data::ArrayListInt*)equationLeft->GetItem(i));
		DEL_CLASS((Data::ArrayListInt*)equationRight->GetItem(i));
	}
	DEL_CLASS(equationLeft);
	DEL_CLASS(equationRight);
	DEL_CLASS(equationSign);
}

Int32 Math::Equation::AddEquation(WChar *equation)
{
	WChar *currPtr = equation;
	WChar *leftStart = currPtr;
	WChar *signStart = 0;
	WChar *rightStart = 0;
	WChar c;
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
				return -1;
			}
		}
	}

	if (rightStart == 0)
	{
		return -1;
	}

	Data::ArrayListInt *right;
	Data::ArrayListInt *left;
	NEW_CLASS(right, Data::ArrayListInt());
	NEW_CLASS(left, Data::ArrayListInt());
	if (ParseEquation(right, rightStart, currPtr - rightStart - 1, 0) != 0)
	{
		DEL_CLASS(right);
		DEL_CLASS(left);
		return -1;
	}
	if (ParseEquation(left, leftStart, signStart - leftStart, 0) != 0)
	{
		DEL_CLASS(right);
		DEL_CLASS(left);
		return -1;
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

	OSInt ret = equationLeft->Add(left);
	equationRight->Add(right);
	equationSign->Add(sign);
	return (Int32)ret;
}

void Math::Equation::Solve(WChar *variName, WChar *outBuff)
{
	//////////////////////////////////////////////
}

WChar *Math::Equation::GetEquationStr(WChar *buff, Int32 index)
{
	//////////////////////////////////////////////
	return 0;
}
