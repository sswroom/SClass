#ifndef _SM_TEXT_DOC_DOCHEADING
#define _SM_TEXT_DOC_DOCHEADING
#include "Text/Doc/DocItem.h"

namespace Text
{
	namespace Doc
	{
		class DocHeading : public DocItem
		{
		private:
			DocItem::HorizontalAlign halign;
		public:
			DocHeading();
			virtual ~DocHeading();

			virtual DocItemType GetItemType();

			void SetHAlignment(DocItem::HorizontalAlign halign);
			DocItem::HorizontalAlign GetHAlignment();
		};
	}
}
#endif
