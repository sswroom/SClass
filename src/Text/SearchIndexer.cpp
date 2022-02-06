#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt32.h"
#include "Data/ICaseBTreeUTF8Map.h"
#include "Text/SearchIndexer.h"

Text::SearchIndexer::SearchIndexer(Text::TextAnalyzer* ta)
{
	this->ta = ta;
	NEW_CLASS(this->strIndex, Data::ICaseBTreeUTF8Map<Data::ArrayListInt64*>());
}

Text::SearchIndexer::~SearchIndexer()
{
	UOSInt cnt;
	Data::ArrayListInt64**vals = ((Data::ICaseBTreeUTF8Map<Data::ArrayListInt64*>*)this->strIndex)->ToArray(&cnt);
	while (cnt-- > 0)
	{
		DEL_CLASS(vals[cnt]);
	}
	MemFree(vals);
	DEL_CLASS(this->strIndex);
}

void Text::SearchIndexer::IndexString(const UTF8Char *str, Int64 key)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	OSInt i;
	void *sess = this->ta->BeginAnalyze(str);
	if (sess == 0)
		return;
	while ((sptr = this->ta->NextWord(sbuff, sess)) != 0)
	{
		Data::ArrayListInt64 *tmpVal = this->strIndex->Get(CSTRP(sbuff, sptr));
		if (tmpVal == 0)
		{
			NEW_CLASS(tmpVal, Data::ArrayListInt64());
			this->strIndex->Put(CSTRP(sbuff, sptr), tmpVal);
		}
		i = tmpVal->SortedIndexOf(key);
		if (i < 0)
		{
			tmpVal->Insert((UOSInt)~i, key);
		}
	}
	this->ta->EndAnalyze(sess);
}

UOSInt Text::SearchIndexer::SearchString(Data::ArrayListInt64 *outArr, const UTF8Char *searchStr, UOSInt maxResults)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Data::ArrayList<Data::ArrayListInt64*> *resultList;
	Data::ArrayListInt32 *resultListCnt;
	Data::ArrayListInt64 *tmpIndex;
	Data::ArrayListInt64 *tmpIndex2;
	void *sess = this->ta->BeginAnalyze(searchStr);
	if (sess == 0)
		return 0;
	NEW_CLASS(resultList, Data::ArrayList<Data::ArrayListInt64*>());
	NEW_CLASS(resultListCnt, Data::ArrayListInt32());
	while ((sptr = this->ta->NextWord(sbuff, sess)) != 0)
	{
		tmpIndex = this->strIndex->Get(CSTRP(sbuff, sptr));
		if (tmpIndex == 0)
		{
			this->ta->EndAnalyze(sess);
			DEL_CLASS(resultList);
			DEL_CLASS(resultListCnt);
			return 0;
		}
		resultList->Insert(resultListCnt->SortedInsert((Int32)tmpIndex->GetCount()), tmpIndex);
	}
	this->ta->EndAnalyze(sess);
	
	UOSInt retCnt = 0;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Int64 ind;
	j = resultList->GetCount();
	if (j > 0)
	{
		tmpIndex = resultList->GetItem(0);
		k = tmpIndex->GetCount();
		i = 0;
		while (retCnt < maxResults && i < k)
		{
			ind = tmpIndex->GetItem(i);
			l = 1;
			while (l < j)
			{
				tmpIndex2 = resultList->GetItem(l);
				if (tmpIndex2->SortedIndexOf(ind) < 0)
				{
					break;
				}
				l++;
			}
			if (l >= j)
			{
				outArr->Add(ind);
				retCnt++;
			}
			i++;
		}
	}
	DEL_CLASS(resultList);
	DEL_CLASS(resultListCnt);
	return retCnt;
}
