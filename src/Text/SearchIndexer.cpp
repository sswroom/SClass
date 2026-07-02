#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt32.h"
#include "Data/ArrayListNN.hpp"
#include "Data/ArrayListObj.hpp"
#include "Data/ICaseBTreeStringMapObj.hpp"
#include "Text/SearchIndexer.h"

Text::SearchIndexer::SearchIndexer(NN<Text::TextAnalyzer> ta)
{
	this->ta = ta;
}

Text::SearchIndexer::~SearchIndexer()
{
	UIntOS cnt;
	UnsafeArray<Optional<Data::ArrayListInt64>> vals = this->strIndex.ToArray(cnt);
	while (cnt-- > 0)
	{
		vals[cnt].Delete();
	}
	MemFreeArr(vals);
}

void Text::SearchIndexer::IndexString(UnsafeArray<const UTF8Char> str, Int64 key)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	IntOS i;
	NN<Text::TextAnalyzer::TextSession> sess = this->ta->BeginAnalyze(str);
	while (this->ta->NextWord(sbuff, sess).SetTo(sptr))
	{
		NN<Data::ArrayListInt64> tmpVal;
		if (!this->strIndex.Get(CSTRP(sbuff, sptr)).SetTo(tmpVal))
		{
			NEW_CLASSNN(tmpVal, Data::ArrayListInt64());
			this->strIndex.Put(CSTRP(sbuff, sptr), tmpVal);
		}
		i = tmpVal->SortedIndexOf(key);
		if (i < 0)
		{
			tmpVal->Insert((UIntOS)~i, key);
		}
	}
	this->ta->EndAnalyze(sess);
}

UIntOS Text::SearchIndexer::SearchString(NN<Data::ArrayListInt64> outArr, UnsafeArray<const UTF8Char> searchStr, UIntOS maxResults)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	NN<Data::ArrayListInt64> tmpIndex;
	NN<Data::ArrayListInt64> tmpIndex2;
	NN<Text::TextAnalyzer::TextSession> sess = this->ta->BeginAnalyze(searchStr);
	Data::ArrayListNN<Data::ArrayListInt64> resultList;
	Data::ArrayListInt32 resultListCnt;
	while (this->ta->NextWord(sbuff, sess).SetTo(sptr))
	{
		if (!this->strIndex.Get(CSTRP(sbuff, sptr)).SetTo(tmpIndex))
		{
			this->ta->EndAnalyze(sess);
			return 0;
		}
		resultList.Insert(resultListCnt.SortedInsert((Int32)tmpIndex->GetCount()), tmpIndex);
	}
	this->ta->EndAnalyze(sess);
	
	UIntOS retCnt = 0;
	UIntOS i;
	UIntOS j;
	UIntOS k;
	UIntOS l;
	Int64 ind;
	j = resultList.GetCount();
	if (j > 0)
	{
		tmpIndex = resultList.GetItemNoCheck(0);
		k = tmpIndex->GetCount();
		i = 0;
		while (retCnt < maxResults && i < k)
		{
			ind = tmpIndex->GetItem(i);
			l = 1;
			while (l < j)
			{
				tmpIndex2 = resultList.GetItemNoCheck(l);
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
