#include "Stdafx.h"
#include "Net/MIBReader.h"
#include "Text/CharUtil.h"

Bool Net::MIBReader::ReadLineInner(NN<Text::StringBuilderUTF8> sb)
{
	UOSInt initSize = sb->GetLength();
	if (!this->reader.ReadLine(sb, 512))
	{
		return false;
	}

	UOSInt i;
	UOSInt j;
	UOSInt k;
	if (this->escapeType == ET_MULTILINE_COMMENT)
	{
		i = sb->IndexOf(UTF8STRC("*/"), initSize);
		if (i == INVALID_INDEX)
		{
			sb->TrimToLength(initSize);
			return true;
		}
		sb->RemoveChars(initSize, (UOSInt)i + 2 - initSize);
		this->escapeType = ET_NONE;
	}
	else if (this->escapeType == ET_STRING)
	{
		i = sb->IndexOf(UTF8STRC("\""), initSize);
		if (i == INVALID_INDEX)
		{
			return true;
		}
		initSize = i + 1;
		this->escapeType = ET_NONE;
	}
	while (true)
	{
		i = sb->IndexOf(UTF8STRC("--"), initSize);
		j = sb->IndexOf(UTF8STRC("/*"), initSize);
		k = sb->IndexOf(UTF8STRC("\""), initSize);
		if (i == INVALID_INDEX && j == INVALID_INDEX && k == INVALID_INDEX)
		{
			break;
		}

		if (i != INVALID_INDEX && (j == INVALID_INDEX || j > i) && (k == INVALID_INDEX || k > i))
		{
			UOSInt j = sb->IndexOf(UTF8STRC("--"), i + 2);
			if (j != INVALID_INDEX)
			{
				sb->RemoveChars(i, (j - i + 2));
				initSize = i;
			}
			else
			{
				sb->TrimToLength(i);
				break;
			}
		}
		else if (j != INVALID_INDEX && (k == INVALID_INDEX || k > j))
		{
			i = sb->IndexOf(UTF8STRC("*/"), j + 2);
			if (i != INVALID_INDEX)
			{
				sb->RemoveChars(j, (i - j + 2));
			}
			else
			{
				sb->TrimToLength(j);
				this->escapeType = ET_MULTILINE_COMMENT;
				break;
			}
		}
		else
		{
			i = sb->IndexOf(UTF8STRC("\""), k + 1);
			if (i != INVALID_INDEX)
			{
				initSize = i + 1;
			}
			else
			{
				this->escapeType = ET_STRING;
				break;
			}
		}
	}
	sb->RTrim();
	return true;
}

Bool Net::MIBReader::ReadWord(NN<Text::StringBuilderUTF8> sb, Bool move)
{
	while (this->currOfst >= this->sbLine.GetCharCnt())
	{
		this->sbLine.ClearStr();
		if (!ReadLineInner(this->sbLine))
		{
			return false;
		}
		this->sbLine.Trim();
		this->currOfst = 0;
	}
	UTF8Char *sptr = this->sbLine.v;
	while (Text::CharUtil::IsWS(&sptr[this->currOfst]))
	{
		this->currOfst++;
	}
	if (sptr[this->currOfst] == '{')
	{
		UOSInt level = 0;
		UOSInt i = this->currOfst;
		while (true)
		{
			if (sptr[i] == 0)
			{
				this->sbLine.AppendUTF8Char(' ');
				if (!ReadLineInner(this->sbLine))
				{
					return false;
				}
				sptr = this->sbLine.v;
			}
			else if (sptr[i] == '{')
			{
				level++;
				i++;
			}
			else if (sptr[i] == '}')
			{
				level--;
				i++;
				if (level == 0)
				{
					sb->AppendC(&sptr[this->currOfst], i - this->currOfst);
					if (move)
					{
						this->currOfst = i;
					}
					return true;
				}
			}
			else
			{
				i++;
			}
		}
	}
	else if (sptr[this->currOfst] == ':' && sptr[this->currOfst + 1] == ':' && sptr[this->currOfst + 2] == '=')
	{
		sb->AppendC(UTF8STRC("::="));
		if (move)
		{
			this->currOfst += 3;
		}
		return true;
	}
	else if (Text::CharUtil::IsAlphaNumeric(sptr[this->currOfst]))
	{
		UOSInt i = this->currOfst;
		if (Text::StrStartsWith(&sptr[this->currOfst], (const UTF8Char*)"OCTET STRING") && !Text::CharUtil::IsAlphaNumeric(sptr[this->currOfst + 12]))
		{
			i += 12;
		}
		else if (Text::StrStartsWith(&sptr[this->currOfst], (const UTF8Char*)"OBJECT IDENTIFIER") && !Text::CharUtil::IsAlphaNumeric(sptr[this->currOfst + 17]))
		{
			i += 17;
		}
		else
		{
			while (Text::CharUtil::IsAlphaNumeric(sptr[i]) || sptr[i] == '-' || sptr[i] == '_')
			{
				i++;
			}
		}
		sb->AppendC(&sptr[this->currOfst], i - this->currOfst);
		if (move)
		{
			this->currOfst = i;
		}
		return true;
	}
	else if (sptr[this->currOfst] == ',' || sptr[this->currOfst] == ';')
	{
		sb->AppendChar(sptr[this->currOfst], 1);
		if (move)
		{
			this->currOfst++;
		}
		return true;
	}
	else if (sptr[this->currOfst] == '(')
	{
		UOSInt level = 0;
		UOSInt i = this->currOfst;
		while (true)
		{
			if (sptr[i] == 0)
			{
				this->sbLine.AppendUTF8Char(' ');
				if (!ReadLineInner(this->sbLine))
				{
					return false;
				}
				sptr = this->sbLine.v;
			}
			else if (sptr[i] == '(')
			{
				level++;
				i++;
			}
			else if (sptr[i] == ')')
			{
				level--;
				i++;
				if (level == 0)
				{
					sb->AppendC(&sptr[this->currOfst], i - this->currOfst);
					if (move)
					{
						this->currOfst = i;
					}
					return true;
				}
			}
			else
			{
				i++;
			}
		}
	}
	else if (sptr[this->currOfst] == '"')
	{
		UOSInt i;
		while (true)
		{
			i = Text::StrIndexOfChar(&sptr[this->currOfst + 1], '"');
			if (i != INVALID_INDEX)
			{
				break;
			}
			reader.GetLastLineBreak(this->sbLine);
			if (!ReadLineInner(this->sbLine))
			{
				return false;
			}
			sptr = this->sbLine.v;
		}
		sb->AppendC(&sptr[this->currOfst], i + 2);
		this->currOfst += i + 2;
		return true;
	}
	else
	{
		return false;
	}
}

Net::MIBReader::MIBReader(NN<IO::Stream> stm) : reader(stm)
{
	this->currOfst = 0;
	this->escapeType = ET_NONE;
}

Net::MIBReader::~MIBReader()
{
}

Bool Net::MIBReader::PeekWord(NN<Text::StringBuilderUTF8> sb)
{
	return ReadWord(sb, false);
}

Bool Net::MIBReader::NextWord(NN<Text::StringBuilderUTF8> sb)
{
	return ReadWord(sb, true);
}

Bool Net::MIBReader::ReadLine(NN<Text::StringBuilderUTF8> sb)
{
	if (this->currOfst >= this->sbLine.GetCharCnt())
	{
		return ReadLineInner(sb);
	}
	else
	{
		sb->AppendC(this->sbLine.ToString() + this->currOfst, this->sbLine.GetLength() - this->currOfst);
		this->currOfst = this->sbLine.GetCharCnt();
		return true;
	}
}

Bool Net::MIBReader::GetLastLineBreak(NN<Text::StringBuilderUTF8> sb)
{
	return this->reader.GetLastLineBreak(sb);
}
