#ifndef _SM_TEXT_DOC_DOCLINK
#define _SM_TEXT_DOC_DOCLINK
#include "Text/Doc/DocItem.h"

namespace Text
{
	namespace Doc
	{
		class DocLink : public DocItem
		{
		private:
			const UTF8Char *url;
		public:
			DocLink(const UTF8Char *url);
			virtual ~DocLink();

			virtual DocItemType GetItemType();

			virtual UOSInt Add(NN<DocItem> item);

			const UTF8Char *GetLink();
		};
	}
}
#endif
