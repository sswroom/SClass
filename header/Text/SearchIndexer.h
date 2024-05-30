#ifndef _SM_TEXT_SEARCHINDEXER
#define _SM_TEXT_SEARCHINDEXER
#include "Data/ArrayListInt64.h"
#include "Data/ICaseBTreeUTF8Map.h"
#include "Text/CString.h"
#include "Text/TextAnalyzer.h"

namespace Text
{
	class SearchIndexer
	{
	private:
		Data::ICaseBTreeUTF8Map<Data::ArrayListInt64*> strIndex;
		Text::TextAnalyzer *ta;

	public:
		SearchIndexer(Text::TextAnalyzer* ta);
		~SearchIndexer();

		void IndexString(UnsafeArray<const UTF8Char> str, Int64 key);
		UOSInt SearchString(NN<Data::ArrayListInt64> outArr, UnsafeArray<const UTF8Char> searchStr, UOSInt maxResults);
	};
}
#endif
