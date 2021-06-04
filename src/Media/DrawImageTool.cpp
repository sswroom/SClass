#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/DrawImageTool.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

void Media::DrawImageTool::SplitString(Media::DrawImage *dimg, const UTF8Char *txt, Data::ArrayList<const UTF8Char *> *outStr, Media::DrawFont *f, Double width)
{
	OSInt i;
	OSInt j;
	OSInt k;
	Double sz[2];
	const UTF8Char *currTxt = txt;
	UTF8Char *tmpBuff;
	UOSInt txtLen;
	txtLen = Text::StrCharCnt(currTxt);
	tmpBuff = MemAlloc(UTF8Char, txtLen + 1);
	UTF8Char c;
	UTF8Char c2;
	while (true)
	{
		if (currTxt[0] == 0)
			break;
		txtLen = Text::StrCharCnt(currTxt);
		k = 0;
		i = 0;
		j = txtLen;
		while (true)
		{
			if (i >= j)
			{
				if (k == 0)
				{
					outStr->Add(Text::StrCopyNew(currTxt));
				}
				else
				{
					dimg->GetTextSize(f, currTxt, Text::StrCharCnt(currTxt), sz);
					if (sz[0] <= width)
					{
						outStr->Add(Text::StrCopyNew(currTxt));
					}
					else
					{
						Text::StrConcatC(tmpBuff, currTxt, k);
						Text::StrTrim(tmpBuff);
						outStr->Add(Text::StrCopyNew(tmpBuff));
						Text::StrConcat(tmpBuff, &currTxt[k]);
						Text::StrTrim(tmpBuff);
						outStr->Add(Text::StrCopyNew(tmpBuff));
					}
				}
				currTxt = &currTxt[j];
				break;
			}
			c = currTxt[i];
			if (c == ' ')
			{
				Text::StrConcatC(tmpBuff, currTxt, i);
				dimg->GetTextSize(f, tmpBuff, i, sz);
				if (sz[0] > width)
				{
					if (k == 0)
					{
						outStr->Add(Text::StrCopyNew(tmpBuff));
						currTxt = &currTxt[i + 1];
					}
					else
					{
						Text::StrConcatC(tmpBuff, currTxt, k);
						Text::StrTrim(tmpBuff);
						outStr->Add(Text::StrCopyNew(tmpBuff));
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
				dimg->GetTextSize(f, tmpBuff, i + 1, sz);
				if (sz[0] > width)
				{
					if (k == 0)
					{
						outStr->Add(Text::StrCopyNew(tmpBuff));
						currTxt = &currTxt[i + 1];
					}
					else
					{
						Text::StrConcatC(tmpBuff, currTxt, k);
						Text::StrTrim(tmpBuff);
						outStr->Add(Text::StrCopyNew(tmpBuff));
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
					outStr->Add(Text::StrCopyNew((const UTF8Char*)""));
				}
				else if (i == k)
				{
					Text::StrConcatC(tmpBuff, currTxt, i);
					outStr->Add(Text::StrCopyNew(tmpBuff));
				}
				else
				{
					Text::StrConcatC(tmpBuff, currTxt, i);
					dimg->GetTextSize(f, tmpBuff, i, sz);
					if (sz[0] > width)
					{
						if (k == 0)
						{
							outStr->Add(Text::StrCopyNew(tmpBuff));
						}
						else
						{
							Text::StrConcatC(tmpBuff, currTxt, k);
							Text::StrTrim(tmpBuff);
							outStr->Add(Text::StrCopyNew(tmpBuff));
							Text::StrConcatC(tmpBuff, &currTxt[k], i - k);
							Text::StrTrim(tmpBuff);
							outStr->Add(Text::StrCopyNew(tmpBuff));
						}
					}
					else
					{
						outStr->Add(Text::StrCopyNew(tmpBuff));
					}
				}
				i++;
				if (i >= j)
				{
					currTxt = &currTxt[j];
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
