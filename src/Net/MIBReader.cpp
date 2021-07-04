#include "Stdafx.h"
#include "Net/MIBReader.h"
#include "Text/CharUtil.h"

Bool Net::MIBReader::ReadLineInner(Text::StringBuilderUTF8 *sb)
{
	if (!this->reader->ReadLine(sb, 512))
	{
		return false;
	}

	OSInt i = sb->IndexOf((const UTF8Char*)"--");
	if (i >= 0)
	{
		OSInt j = sb->IndexOf((const UTF8Char*)"--", i + 2);
		if (j >= 0)
		{
			sb->RemoveChars((UOSInt)i, (UOSInt)(j - i + 2));
		}
		else
		{
			sb->TrimToLength((UOSInt)i);
		}
	}
	sb->TrimRight();
	return true;
}

Bool Net::MIBReader::ReadWord(Text::StringBuilderUTF *sb, Bool move)
{
	while (this->currOfst >= this->sbLine->GetCharCnt())
	{
		this->sbLine->ClearStr();
		if (!ReadLineInner(this->sbLine))
		{
			return false;
		}
		this->sbLine->Trim();
		this->currOfst = 0;
	}
	UTF8Char *sptr = this->sbLine->ToString();
	while (Text::CharUtil::IsWS(&sptr[this->currOfst]))
	{
		this->currOfst++;
	}
	if (sptr[this->currOfst] == '{')
	{
		OSInt i;
		while (true)
		{
			i = Text::StrIndexOf(&sptr[this->currOfst], '}');
			if (i >= 0)
			{
				break;
			}
			if (!ReadLineInner(this->sbLine))
			{
				return false;
			}
			sptr = this->sbLine->ToString();
		}
		sb->AppendC(&sptr[this->currOfst], (UOSInt)i + 1);
		if (move)
		{
			this->currOfst += (UOSInt)i + 1;
		}
		return true;
	}
	else if (sptr[this->currOfst] == ':' && sptr[this->currOfst + 1] == ':' && sptr[this->currOfst + 2] == '=')
	{
		sb->Append((const UTF8Char*)"::=");
		if (move)
		{
			this->currOfst += 3;
		}
		return true;
	}
	else if (Text::CharUtil::IsAlphaNumeric(sptr[this->currOfst]))
	{
		UOSInt i = this->currOfst;
		while (Text::CharUtil::IsAlphaNumeric(sptr[i]) || sptr[i] == '-' || sptr[i] == '_')
		{
			i++;
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
				this->sbLine->AppendChar(' ', 1);
				if (!ReadLineInner(this->sbLine))
				{
					return false;
				}
				sptr = this->sbLine->ToString();
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
	else
	{
		return false;
	}
}

Net::MIBReader::MIBReader(IO::Stream *stm)
{
	NEW_CLASS(this->reader, Text::UTF8Reader(stm));
	NEW_CLASS(this->sbLine, Text::StringBuilderUTF8());
	this->currOfst = 0;
}

Net::MIBReader::~MIBReader()
{
	DEL_CLASS(this->sbLine);
	DEL_CLASS(this->reader);
}

Bool Net::MIBReader::PeekWord(Text::StringBuilderUTF *sb)
{
	return ReadWord(sb, false);
}

Bool Net::MIBReader::NextWord(Text::StringBuilderUTF *sb)
{
	return ReadWord(sb, true);
}

Bool Net::MIBReader::ReadLine(Text::StringBuilderUTF8 *sb)
{
	if (this->currOfst >= this->sbLine->GetCharCnt())
	{
		return ReadLineInner(sb);
	}
	else
	{
		sb->Append(this->sbLine->ToString() + this->currOfst);
		this->currOfst = this->sbLine->GetCharCnt();
		return true;
	}
}

Bool Net::MIBReader::GetLastLineBreak(Text::StringBuilderUTF *sb)
{
	return this->reader->GetLastLineBreak(sb);
}
