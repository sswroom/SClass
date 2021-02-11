#ifndef _SM_TEXT_DOC_DOCITEM
#define _SM_TEXT_DOC_DOCITEM
#include "IO/ParsedObject.h"
#include "Data/ArrayList.h"

namespace Text
{
	namespace Doc
	{
		class DocItem : public Data::ReadingList<DocItem*>
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
			Data::ArrayList<DocItem*> *items;
		public:
			DocItem();
			virtual ~DocItem();

			virtual DocItemType GetItemType() = 0;

			virtual UOSInt Add(DocItem *item);
			virtual UOSInt GetCount();
			virtual DocItem *GetItem(UOSInt index);
		};
	}
}
#endif
