#include "Stdafx.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/RegEx.h"

UOSInt Text::RegEx::UnknownExpression::Match(Text::CStringNN s, UOSInt index) const
{
	return INVALID_INDEX;
}

void Text::RegEx::UnknownExpression::ToString(NN<Text::StringBuilderUTF8> sb) const
{
}

void Text::RegEx::UnknownExpression::Summary(NN<Text::StringBuilderUTF8> sb, UOSInt level) const
{
	sb->AppendChar('\t', level);
	sb->Append(CSTR("Unknown: "));
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

void Text::RegEx::WSExpression::Summary(NN<Text::StringBuilderUTF8> sb, UOSInt level) const
{
	sb->AppendChar('\t', level);
	sb->Append(CSTR("WS: \\s"));
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

void Text::RegEx::SeqExpression::Summary(NN<Text::StringBuilderUTF8> sb, UOSInt level) const
{
	sb->AppendChar('\t', level);
	sb->Append(CSTR("Seq:"));
	UOSInt i = 0;
	UOSInt j = this->expList.GetCount();
	while (i < j)
	{
		sb->Append(CSTR("\r\n"));
		this->expList.GetItemNoCheck(i)->Summary(sb, level + 1);
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

void Text::RegEx::SubExpression::Summary(NN<Text::StringBuilderUTF8> sb, UOSInt level) const
{
	sb->AppendChar('\t', level);
	sb->Append(CSTR("Sub: ()"));
	UOSInt i = 0;
	UOSInt j = this->expList.GetCount();
	while (i < j)
	{
		sb->Append(CSTR("\r\n"));
		this->expList.GetItemNoCheck(i)->Summary(sb, level + 1);
		i++;
	}
}

UOSInt Text::RegEx::BracketExpression::Match(Text::CStringNN s, UOSInt index) const
{
	UTF32Char c;
	UnsafeArray<const UTF8Char> sptr = Text::StrReadChar(&s.v[index], c);
	UOSInt i = this->charList.GetCount();
	while (i-- > 0)
	{
		if (this->charList.GetItem(i) == c)
		{
			if (this->notMatch)
			{
				return INVALID_INDEX;
			}
			else
			{
				return (UOSInt)(sptr - (&s.v[index]));
			}
		}
	}
	if (this->notMatch)
		return (UOSInt)(sptr - (&s.v[index]));
	else
		return INVALID_INDEX;
}

void Text::RegEx::BracketExpression::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	UOSInt i;
	UOSInt j;
	sb->AppendUTF8Char('[');
	if (this->notMatch)
		sb->AppendUTF8Char('^');
	UTF32Char c;
	i = 0;
	j = this->charList.GetCount();
	while (i < j)
	{
		c = this->charList.GetItem(i);
		sb->AppendChar(c, 1);
		i++;
	}
	sb->AppendUTF8Char(']');
}

void Text::RegEx::BracketExpression::Summary(NN<Text::StringBuilderUTF8> sb, UOSInt level) const
{
	sb->AppendChar('\t', level)->Append(CSTR("Bracket: "));
	this->ToString(sb);
}

UOSInt Text::RegEx::OrExpression::Match(Text::CStringNN s, UOSInt index) const
{
	UOSInt ret;
	UOSInt i = 0;
	UOSInt j = this->expList.GetCount();
	while (i < j)
	{
		ret = this->expList.GetItemNoCheck(i)->Match(s, index);
		if (ret != INVALID_INDEX)
			return ret;
		i++;
	}
	return INVALID_INDEX;
}

void Text::RegEx::OrExpression::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	UOSInt i = 0;
	UOSInt j = this->expList.GetCount();
	while (i < j)
	{
		if (i > 0) sb->AppendUTF8Char('|');
		this->expList.GetItemNoCheck(i)->ToString(sb);
		i++;
	}
}

void Text::RegEx::OrExpression::Summary(NN<Text::StringBuilderUTF8> sb, UOSInt level) const
{
	sb->AppendChar('\t', level)->Append(CSTR("Or Begin: "));
	UOSInt i = 0;
	UOSInt j = this->expList.GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sb->Append(CSTR("\r\n"))->AppendChar('\t', level)->Append(CSTR("Or"));
		}
		sb->Append(CSTR("\r\n"));
		this->expList.GetItemNoCheck(i)->Summary(sb, level + 1);
		i++;
	}
}

void Text::RegEx::OrExpression::AddExpList(NN<Data::ArrayListNN<Expression>> expList)
{
	NN<Expression> exp;
	if (expList->GetCount() == 1)
	{
		exp = expList->GetItemNoCheck(0);
		this->expList.Add(exp);
	}
	else
	{
		NEW_CLASSNN(exp, SeqExpression(expList));
		this->expList.Add(exp);
	}
}

UOSInt Text::RegEx::CharMatchExpression::Match(Text::CStringNN s, UOSInt index) const
{
	UTF32Char c;
	UnsafeArray<const UTF8Char> sptr = Text::StrReadChar(&s.v[index], c);
	if (c == this->c)
		return (UOSInt)(sptr - (&s.v[index]));
	return INVALID_INDEX;
}


struct Text::RegEx::ParseState
{
	UnsafeArray<const UTF8Char> regEx;
	UTF32Char endChar;
	Bool hasError;
};

NN<Text::RegEx::Expression> Text::RegEx::ParseExpression(NN<ParseState> state)
{
	NN<Expression> exp;
	NN<OrExpression> orExp;
	Data::ArrayListNN<Expression> expList;
	UnsafeArray<const UTF8Char> regEx = state->regEx;
	UTF32Char c;
	while (true)
	{
		regEx = Text::StrReadChar(regEx, c);
		if (c == state->endChar)
		{
			state->regEx = regEx;
			if (expList.GetCount() == 0)
			{
				NEW_CLASSNN(exp, UnknownExpression());
				return exp;
			}
			else
			{
				exp = expList.GetItemNoCheck(0);
				if (exp->GetType() == ExpressionType::Or)
				{
					orExp = NN<OrExpression>::ConvertFrom(exp);
					if (expList.GetCount() == 1)
					{
						expList.DeleteAll();
						NEW_CLASSNN(exp, UnknownExpression());
						return exp;
					}
					else
					{
						expList.RemoveAt(0);
						orExp->AddExpList(expList);
					}
					if (state->endChar == ')')
					{
						expList.Clear();
						expList.Add(orExp);
						NEW_CLASSNN(exp, SubExpression(expList));
						return exp;
					}
					else
					{
						return orExp;
					}
				}
				else
				{
					if (state->endChar == ')')
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
			UTF32Char lastEndCh = state->endChar;
			state->regEx = regEx;
			state->endChar = ')';
			expList.Add(ParseExpression(state));
			state->endChar = lastEndCh;
			regEx = state->regEx;
		}
		else if (c == '[')
		{
			NN<BracketExpression> bExp;
			regEx = Text::StrReadChar(regEx, c);
			if (c == '^')
			{
				NEW_CLASSNN(bExp, BracketExpression(true));
				regEx = Text::StrReadChar(regEx, c);
			}
			else
			{
				NEW_CLASSNN(bExp, BracketExpression(false));
			}
			if (c == ']')
			{
				bExp->AddChar(c);
				regEx = Text::StrReadChar(regEx, c);
			}
			while (true)
			{
				if (c == 0)
				{
					printf("RegEx: End bracket not found\r\n");
					bExp.Delete();
					expList.DeleteAll();
					state->regEx = regEx;
					NEW_CLASSNN(exp, UnknownExpression());
					return exp;
				}
				else if (c == ']')
				{
					expList.Add(bExp);
					break;
				}
				else
				{
					bExp->AddChar(c);
					regEx = Text::StrReadChar(regEx, c);
				}
			}
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
		else if (c == '|')
		{
			if (expList.GetCount() == 0)
			{
				printf("RegEx: '|' found without expression\r\n");
				state->regEx = regEx;
				NEW_CLASSNN(exp, UnknownExpression());
				return exp;
			}
			exp = expList.GetItemNoCheck(0);
			if (exp->GetType() == ExpressionType::Or)
			{
				if (expList.GetCount() == 1)
				{
					printf("RegEx: '|' found without additional expression\r\n");
					orExp.Delete();
					state->regEx = regEx;
					NEW_CLASSNN(exp, UnknownExpression());
					return exp;
				}
				orExp = NN<OrExpression>::ConvertFrom(exp);
				expList.RemoveAt(0);
				orExp->AddExpList(expList);
				expList.Clear();
				expList.Add(orExp);
			}
			else
			{
				NEW_CLASSNN(orExp, OrExpression());
				orExp->AddExpList(expList);
				expList.Clear();
				expList.Add(orExp);
			}
		}
		else
		{
			NEW_CLASSNN(exp, CharMatchExpression(c));
			expList.Add(exp);

			/*printf("RegEx: Unknown character: %c\r\n", c);
			expList.DeleteAll();
			state->regEx = regEx;
			NEW_CLASSNN(exp, UnknownExpression());
			return exp;*/
		}
	}
}

Text::RegEx::RegEx(UnsafeArray<const Char> regEx)
{
	ParseState state;
	state.regEx = UnsafeArray<const UTF8Char>::ConvertFrom(regEx);
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

void Text::RegEx::Summary(NN<Text::StringBuilderUTF8> sb) const
{
	this->exp->Summary(sb, 0);
}

void Text::RegEx::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	this->exp->ToString(sb);
}
