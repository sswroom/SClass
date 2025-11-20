#ifndef _SM_TEXT_DOC_DOCITEM
#define _SM_TEXT_DOC_DOCITEM
#include "Data/ArrayListNN.hpp"
#include "IO/ParsedObject.h"

namespace Text
{
	namespace Doc
	{
		class DocItem : public Data::ReadingListNN<DocItem>
		{
		public:
			typedef enum
			{
				DIT_TEXT,
				DIT_HORICENTER,
				DIT_URL,
				DIT_HEADING,
				DIT_VALIDATOR
			} DocItemType;
			typedef enum
			{
				HALIGN_NONE,
				HALIGN_CENTER
			} HorizontalAlign;
		protected:
			Data::ArrayListNN<DocItem> items;
		public:
			DocItem();
			virtual ~DocItem();

			virtual DocItemType GetItemType() = 0;

			virtual UOSInt Add(NN<DocItem> item);
			virtual UOSInt GetCount() const;
			virtual NN<DocItem> GetItemNoCheck(UOSInt index) const;
			virtual Optional<DocItem> GetItem(UOSInt index) const;
		};
	}
}
#endif
