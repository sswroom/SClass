#ifndef _SM_TEXT_DOC_DOCTEXT
#define _SM_TEXT_DOC_DOCTEXT
#include "Text/Doc/DocItem.h"

namespace Text
{
	namespace Doc
	{
		class DocText : public DocItem
		{
		private:
			const UTF8Char *text;
		public:
			DocText(const UTF8Char *text);
			virtual ~DocText();

			virtual DocItemType GetItemType();

			virtual UOSInt Add(DocItem *item);

			const UTF8Char *GetText();
		};
	}
}
#endif
