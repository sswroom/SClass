#include "Stdafx.h"
#include "Text/RegEx.h"

UOSInt Text::RegEx::UnknownExpression::Match(Text::CStringNN s, UOSInt index)
{
	return INVALID_INDEX;
}

void Text::RegEx::UnknownExpression::ToString(NN<Text::StringBuilderUTF8> sb) const
{
}

UOSInt Text::RegEx::WSExpression::Match(Text::CStringNN s, UOSInt index)
{
	if (s.leng <= index)
		return INVALID_INDEX;
	UTF8Char c = s.v[index];
	if (c == ' ' || c == '\t' || c == '\n')
		return 1;
	if (c == '\r')
	{
		if (s.leng <= index + 1)
			return 1;
		else if (s.v[index + 1] == '\n')
			return 2;
		else
			return 1;
	}
	return INVALID_INDEX;
}

void Text::RegEx::WSExpression::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	sb->Append(CSTR("\\s"));
}

struct Text::RegEx::ParseState
{
	UnsafeArray<const Char> regEx;
	Char endChar;
	Bool hasError;
};

NN<Text::RegEx::Expression> Text::RegEx::ParseExpression(NN<ParseState> state)
{
	/*
	UnsafeArray<const Char> regEx = state->regEx;
	Char c;
	while (true)
	{
		c = regEx[0];
		regEx++;
		if (c == state->endChar)
		{
			state->regEx = regEx;

		}

	}*/
	////////////////////////////////////
	NN<Expression> exp;
	NEW_CLASSNN(exp, WSExpression());
	return exp;
}

Text::RegEx::RegEx(UnsafeArray<const Char> regEx)
{
	ParseState state;
	state.regEx = regEx;
	state.endChar = 0;
	state.hasError = false;
	this->exp = ParseExpression(state);
}

Text::RegEx::~RegEx()
{
	this->exp.Delete();
}

UOSInt Text::RegEx::Split(Text::CStringNN s, NN<Data::ArrayListStringNN> result) const
{
	UOSInt initCnt = result->GetCount();
	UOSInt lastIndex = 0;
	UOSInt res;
	UOSInt i = 0;
	UOSInt j = s.leng;
	while (i < j)
	{
		res = this->exp->Match(s, i);
		if (res != INVALID_INDEX)
		{
			if (res == 0)
			{
				if (i != lastIndex)
				{
					result->Add(Text::String::New(&s.v[lastIndex], i - lastIndex));
					lastIndex = i;
				}
				i++;
			}
			else
			{
				if (i != lastIndex)
				{
					result->Add(Text::String::New(&s.v[lastIndex], i - lastIndex));
				}
				result->Add(Text::String::New(&s.v[i], res));
				i += res;
				lastIndex = i;
			}
		}
		else
		{
			i++;
		}
	}
	if (i != lastIndex)
	{
		result->Add(Text::String::New(&s.v[lastIndex], i - lastIndex));
	}
	return result->GetCount() - initCnt;
}
