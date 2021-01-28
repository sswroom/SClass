#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/TextAnalyzer.h"

Text::TextAnalyzer::TextAnalyzer()
{
}

Text::TextAnalyzer::~TextAnalyzer()
{
}

void *Text::TextAnalyzer::BeginAnalyze(const UTF8Char *s)
{
	TextSession *tsess;
	if (s)
	{
		tsess = MemAlloc(TextSession, 1);
		tsess->currPos = s;
		return tsess;
	}
	else
	{
		return 0;
	}
}

UTF8Char *Text::TextAnalyzer::NextWord(UTF8Char *sbuff, void *sess)
{
	TextSession *tsess = (TextSession*)sess;
	UTF32Char c;
	UTF8Char *sptr = sbuff;
	while (true)
	{
		tsess->currPos = Text::StrReadChar(tsess->currPos, &c);
		if (c == 0)
		{
			break;
		}
		else if (c < 256)
		{
			if (c == ' ' || c == '\t' || c == '!' || c == '\"' || c == '.')
			{
				if (sptr != sbuff)
					break;
			}
			else
			{
				sptr = Text::StrWriteChar(sptr, c);
			}
		}
		else if (c >= 0x3f00 && c <= 0x9f00)
		{
			if (sptr != sbuff)
				break;
			sptr = Text::StrWriteChar(sptr, c);
			break;
		}
		else
		{
			if (sptr != sbuff)
				break;
			sptr = Text::StrWriteChar(sptr, c);
			break;
		}
		
	}
	if (sptr != sbuff)
	{
		*sptr = 0;
		return sptr;
	}
	else
	{
		return 0;
	}
}

void Text::TextAnalyzer::EndAnalyze(void *sess)
{
	MemFree(sess);
}
