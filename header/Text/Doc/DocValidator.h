#ifndef _SM_TEXT_DOC_DOCVALIDATOR
#define _SM_TEXT_DOC_DOCVALIDATOR
#include "Text/Doc/DocItem.h"

namespace Text
{
	namespace Doc
	{
		class DocValidator : public DocItem
		{
		public:
			DocValidator();
			virtual ~DocValidator();

			virtual DocItemType GetItemType();

			virtual UOSInt Add(DocItem *item);
		};
	}
}
#endif
