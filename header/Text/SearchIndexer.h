#ifndef _SM_TEXT_SEARCHINDEXER
#define _SM_TEXT_SEARCHINDEXER
#include "Data/ArrayListInt64.h"
#include "Data/ICaseBTreeStringMapObj.hpp"
#include "Text/CString.h"
#include "Text/TextAnalyzer.h"

namespace Text
{
	class SearchIndexer
	{
	private:
		Data::ICaseBTreeStringMapObj<Data::ArrayListInt64*> strIndex;
		NN<Text::TextAnalyzer> ta;

	public:
		SearchIndexer(NN<Text::TextAnalyzer> ta);
		~SearchIndexer();

		void IndexString(UnsafeArray<const UTF8Char> str, Int64 key);
		UIntOS SearchString(NN<Data::ArrayListInt64> outArr, UnsafeArray<const UTF8Char> searchStr, UIntOS maxResults);
	};
}
#endif
