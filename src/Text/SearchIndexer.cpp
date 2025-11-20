#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt32.h"
#include "Data/ICaseBTreeUTF8Map.hpp"
#include "Text/SearchIndexer.h"

Text::SearchIndexer::SearchIndexer(NN<Text::TextAnalyzer> ta)
{
	this->ta = ta;
}

Text::SearchIndexer::~SearchIndexer()
{
	UOSInt cnt;
	Data::ArrayListInt64**vals = this->strIndex.ToArray(cnt);
	while (cnt-- > 0)
	{
		DEL_CLASS(vals[cnt]);
	}
	MemFree(vals);
}

void Text::SearchIndexer::IndexString(UnsafeArray<const UTF8Char> str, Int64 key)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	OSInt i;
	NN<Text::TextAnalyzer::TextSession> sess = this->ta->BeginAnalyze(str);
	while (this->ta->NextWord(sbuff, sess).SetTo(sptr))
	{
		Data::ArrayListInt64 *tmpVal = this->strIndex.Get(CSTRP(sbuff, sptr));
		if (tmpVal == 0)
		{
			NEW_CLASS(tmpVal, Data::ArrayListInt64());
			this->strIndex.Put(CSTRP(sbuff, sptr), tmpVal);
		}
		i = tmpVal->SortedIndexOf(key);
		if (i < 0)
		{
			tmpVal->Insert((UOSInt)~i, key);
		}
	}
	this->ta->EndAnalyze(sess);
}

UOSInt Text::SearchIndexer::SearchString(NN<Data::ArrayListInt64> outArr, UnsafeArray<const UTF8Char> searchStr, UOSInt maxResults)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Data::ArrayListInt64 *tmpIndex;
	Data::ArrayListInt64 *tmpIndex2;
	NN<Text::TextAnalyzer::TextSession> sess = this->ta->BeginAnalyze(searchStr);
	Data::ArrayList<Data::ArrayListInt64*> resultList;
	Data::ArrayListInt32 resultListCnt;
	while (this->ta->NextWord(sbuff, sess).SetTo(sptr))
	{
		tmpIndex = this->strIndex.Get(CSTRP(sbuff, sptr));
		if (tmpIndex == 0)
		{
			this->ta->EndAnalyze(sess);
			return 0;
		}
		resultList.Insert(resultListCnt.SortedInsert((Int32)tmpIndex->GetCount()), tmpIndex);
	}
	this->ta->EndAnalyze(sess);
	
	UOSInt retCnt = 0;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Int64 ind;
	j = resultList.GetCount();
	if (j > 0)
	{
		tmpIndex = resultList.GetItem(0);
		k = tmpIndex->GetCount();
		i = 0;
		while (retCnt < maxResults && i < k)
		{
			ind = tmpIndex->GetItem(i);
			l = 1;
			while (l < j)
			{
				tmpIndex2 = resultList.GetItem(l);
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
	return retCnt;
}
