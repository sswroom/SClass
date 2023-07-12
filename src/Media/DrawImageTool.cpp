#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/DrawImageTool.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

void Media::DrawImageTool::SplitString(Media::DrawImage *dimg, Text::CString txt, Data::ArrayListNN<Text::String> *outStr, Media::DrawFont *f, Double width)
{
	UOSInt i;
	UOSInt k;
	Math::Size2DDbl sz;
	const UTF8Char *currTxt = txt.v;
	const UTF8Char *currTxtEnd = txt.v + txt.leng;
	UTF8Char *tmpBuff;
	UTF8Char *sptr;
	UOSInt txtLen = txt.leng;
	tmpBuff = MemAlloc(UTF8Char, txtLen + 1);
	UTF8Char c;
	UTF8Char c2;
	while (true)
	{
		if (currTxt[0] == 0)
			break;
		txtLen = (UOSInt)(currTxtEnd - currTxt);
		k = 0;
		i = 0;
		while (true)
		{
			if (i >= txtLen)
			{
				if (k == 0)
				{
					outStr->Add(Text::String::NewP(currTxt, currTxtEnd));
				}
				else
				{
					sz = dimg->GetTextSize(f, CSTRP(currTxt, currTxtEnd));
					if (sz.x <= width)
					{
						outStr->Add(Text::String::NewP(currTxt, currTxtEnd));
					}
					else
					{
						Text::StrConcatC(tmpBuff, currTxt, k);
						sptr = Text::StrTrim(tmpBuff);
						outStr->Add(Text::String::NewP(tmpBuff, sptr));
						Text::StrConcatC(tmpBuff, &currTxt[k], (UOSInt)(currTxtEnd - &currTxt[k]));
						sptr = Text::StrTrim(tmpBuff);
						outStr->Add(Text::String::NewP(tmpBuff, sptr));
					}
				}
				currTxt = &currTxt[txtLen];
				break;
			}
			c = currTxt[i];
			if (c == ' ')
			{
				Text::StrConcatC(tmpBuff, currTxt, i);
				sz = dimg->GetTextSize(f, {tmpBuff, i});
				if (sz.x > width)
				{
					if (k == 0)
					{
						outStr->Add(Text::String::New(tmpBuff, i));
						currTxt = &currTxt[i + 1];
					}
					else
					{
						Text::StrConcatC(tmpBuff, currTxt, k);
						sptr = Text::StrTrim(tmpBuff);
						outStr->Add(Text::String::New(tmpBuff, (UOSInt)(sptr - tmpBuff)));
						currTxt = &currTxt[k];
					}
					break;
				}
				else
				{
					k = i + 1;
				}
			}
			else if (c == '-')
			{
				Text::StrConcatC(tmpBuff, currTxt, i + 1);
				sz = dimg->GetTextSize(f, {tmpBuff, i + 1});
				if (sz.x > width)
				{
					if (k == 0)
					{
						outStr->Add(Text::String::New(tmpBuff, i + 1));
						currTxt = &currTxt[i + 1];
					}
					else
					{
						Text::StrConcatC(tmpBuff, currTxt, k);
						sptr = Text::StrTrim(tmpBuff);
						outStr->Add(Text::String::NewP(tmpBuff, sptr));
						currTxt = &currTxt[k];
					}
					break;
				}
				else
				{
					k = i + 1;
				}
			}
			else if (c == '\r' || c == '\n')
			{
				if (i == 0)
				{
					outStr->Add(Text::String::NewEmpty());
				}
				else if (i == k)
				{
					Text::StrConcatC(tmpBuff, currTxt, i);
					outStr->Add(Text::String::New(tmpBuff, i));
				}
				else
				{
					Text::StrConcatC(tmpBuff, currTxt, i);
					sz = dimg->GetTextSize(f, {tmpBuff, i});
					if (sz.x > width)
					{
						if (k == 0)
						{
							outStr->Add(Text::String::New(tmpBuff, i));
						}
						else
						{
							Text::StrConcatC(tmpBuff, currTxt, k);
							sptr = Text::StrTrim(tmpBuff);
							outStr->Add(Text::String::NewP(tmpBuff, sptr));
							Text::StrConcatC(tmpBuff, &currTxt[k], i - k);
							sptr = Text::StrTrim(tmpBuff);
							outStr->Add(Text::String::NewP(tmpBuff, sptr));
						}
					}
					else
					{
						outStr->Add(Text::String::New(tmpBuff, i));
					}
				}
				i++;
				if (i >= txtLen)
				{
					currTxt = &currTxt[txtLen];
				}
				else
				{
					c2 = currTxt[i];
					if (c2 != c && (c2 == '\r' || c2 == '\n'))
					{
						i++;
					}
					currTxt = &currTxt[i];
				}
				break;
			}
			i++;
		}
	}
	MemFree(tmpBuff);
}
