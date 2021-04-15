#ifndef _SM_TEXT_SEARCHINDEXER
#define _SM_TEXT_SEARCHINDEXER
#include "Data/IMap.h"
#include "Data/ArrayListInt64.h"
#include "Text/TextAnalyzer.h"

namespace Text
{
	class SearchIndexer
	{
	private:
		Data::IMap<const UTF8Char *, Data::ArrayListInt64*> *strIndex;
		Text::TextAnalyzer *ta;

	public:
		SearchIndexer(Text::TextAnalyzer* ta);
		~SearchIndexer();

		void IndexString(const UTF8Char *str, Int64 key);
		UOSInt SearchString(Data::ArrayListInt64 *outArr, const UTF8Char *searchStr, UOSInt maxResults);
	};
}
#endif
