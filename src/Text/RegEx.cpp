#include "Stdafx.h"
#include "Text/RegEx.h"

UOSInt Text::RegEx::UnknownExpression::Match(Text::CStringNN s, UOSInt index) const
{
	return INVALID_INDEX;
}

void Text::RegEx::UnknownExpression::ToString(NN<Text::StringBuilderUTF8> sb) const
{
}

UOSInt Text::RegEx::WSExpression::Match(Text::CStringNN s, UOSInt index) const
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

UOSInt Text::RegEx::SeqExpression::Match(Text::CStringNN s, UOSInt index) const
{
	return this->MatchInner(s, index, 0);
}

void Text::RegEx::SeqExpression::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	UOSInt i = 0;
	UOSInt j = this->expList.GetCount();
	while (i < j)
	{
		this->expList.GetItemNoCheck(i)->ToString(sb);
		i++;
	}
}

UOSInt Text::RegEx::SeqExpression::MatchInner(Text::CStringNN s, UOSInt sIndex, UOSInt expIndex) const
{
	NN<Expression> exp;
	ExpressionType type;
	UOSInt currIndex = sIndex;
	UOSInt len;
	UOSInt i = expIndex;
	UOSInt j = this->expList.GetCount();
	while (i < j)
	{
		exp = this->expList.GetItemNoCheck(i);
		type = exp->GetType();
		if (type == ExpressionType::ZeroOrOne)
		{
			len = this->MatchInner(s, currIndex, expIndex + 1);
			if (len != INVALID_INDEX)
			{
				return len + (currIndex - sIndex);
			}
			len = exp->Match(s, currIndex);
			if (len == INVALID_INDEX)
			{
				return INVALID_INDEX;
			}
			currIndex += len;
			len = this->MatchInner(s, currIndex, expIndex + 1);
			if (len == INVALID_INDEX)
			{
				return INVALID_INDEX;
			}
			return len + (currIndex - sIndex);
		}
		else if (type == ExpressionType::OneOrMany)
		{
			while (true)
			{
				len = exp->Match(s, currIndex);
				if (len == INVALID_INDEX)
				{
					return INVALID_INDEX;
				}
				currIndex += len;
				len = this->MatchInner(s, currIndex, expIndex + 1);
				if (len != INVALID_INDEX)
				{
					return len + (currIndex - sIndex);
				}
			}
		}
		else if (type == ExpressionType::ZeroOrMany)
		{
			while (true)
			{
				len = this->MatchInner(s, currIndex, expIndex + 1);
				if (len != INVALID_INDEX)
				{
					return len + (currIndex - sIndex);
				}
				len = exp->Match(s, currIndex);
				if (len == INVALID_INDEX)
				{
					return INVALID_INDEX;
				}
				currIndex += len;
			}
		}
		else
		{
			len = exp->Match(s, currIndex);
			if (len == INVALID_INDEX)
			{
				return INVALID_INDEX;
			}
			currIndex += len;
		}
		i++;
	}
	return currIndex - sIndex;
}

void Text::RegEx::SubExpression::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	sb->AppendUTF8Char('(');
	this->SeqExpression::ToString(sb);
	sb->AppendUTF8Char(')');
}

struct Text::RegEx::ParseState
{
	UnsafeArray<const Char> regEx;
	Char endChar;
	Bool hasError;
};

NN<Text::RegEx::Expression> Text::RegEx::ParseExpression(NN<ParseState> state)
{
	NN<Expression> exp;
	Data::ArrayListNN<Expression> expList;
	UnsafeArray<const Char> regEx = state->regEx;
	Char c;
	while (true)
	{
		c = regEx[0];
		regEx++;
		if (c == state->endChar)
		{
			state->regEx = regEx;
			if (expList.GetCount() == 0)
			{
				NEW_CLASSNN(exp, UnknownExpression());
				return exp;
			}
			else if (state->endChar == ')')
			{
				NEW_CLASSNN(exp, SubExpression(expList));
				return exp;
			}
			else
			{
				NEW_CLASSNN(exp, SeqExpression(expList));
				return exp;
			}
		}
		else if (c == 0)
		{
			expList.DeleteAll();
			state->regEx = regEx - 1;
			NEW_CLASSNN(exp, UnknownExpression());
			return exp;
		}
		else if (c == '(')
		{
			Char lastEndCh = state->endChar;
			state->regEx = regEx;
			state->endChar = ')';
			expList.Add(ParseExpression(state));
			state->endChar = lastEndCh;
			regEx = state->regEx;
		}
		else if (c == '\\')
		{
			c = regEx[0];
			regEx++;
			if (c == 's')
			{
				NEW_CLASSNN(exp, WSExpression());
				expList.Add(exp);
			}
			else
			{
				printf("RegEx: Unknown escape sequence: \\%c\r\n", c);
				expList.DeleteAll();
				state->regEx = regEx;
				NEW_CLASSNN(exp, UnknownExpression());
				return exp;
			}
		}
		else if (c == '.')
		{
			NEW_CLASSNN(exp, AnyCharExpression());
			expList.Add(exp);
		}
		else if (c == '+' || c == '*' || c == '?')
		{
			if (expList.GetCount() == 0)
			{
				printf("RegEx: Quantification found without expression: %c\r\n", c);
				state->regEx = regEx;
				NEW_CLASSNN(exp, UnknownExpression());
				return exp;
			}
			exp = expList.GetItemNoCheck(expList.GetCount() - 1);
			if (c == '+')
			{
				NEW_CLASSNN(exp, OneOrManyExpression(exp));
			}
			else if (c == '*')
			{
				NEW_CLASSNN(exp, ZeroOrManyExpression(exp));
			}
			else
			{
				NEW_CLASSNN(exp, ZeroOrOneExpression(exp));
			}
			expList.SetItem(expList.GetCount() - 1, exp);
		}
		else
		{
			////////////////////////////////////
			printf("RegEx: Unknown character: %c\r\n", c);
			expList.DeleteAll();
			state->regEx = regEx;
			NEW_CLASSNN(exp, UnknownExpression());
			return exp;
		}
	}
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
