#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/TextAnalyzer.h"

struct Text::TextAnalyzer::TextSession
{
	UnsafeArray<const UTF8Char> currPos;
};

Text::TextAnalyzer::TextAnalyzer()
{
}

Text::TextAnalyzer::~TextAnalyzer()
{
}

NN<Text::TextAnalyzer::TextSession> Text::TextAnalyzer::BeginAnalyze(UnsafeArray<const UTF8Char> s)
{
	NN<TextSession> tsess;
	tsess = MemAllocNN(TextSession);
	tsess->currPos = s;
	return tsess;
}

UnsafeArrayOpt<UTF8Char> Text::TextAnalyzer::NextWord(UnsafeArray<UTF8Char> sbuff, NN<TextSession> sess)
{
	NN<TextSession> tsess = sess;
	UTF32Char c;
	UnsafeArray<UTF8Char> sptr = sbuff;
	while (true)
	{
		tsess->currPos = Text::StrReadChar(tsess->currPos, c);
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

void Text::TextAnalyzer::EndAnalyze(NN<TextSession> sess)
{
	MemFreeNN(sess);
}
