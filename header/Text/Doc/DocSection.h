#ifndef _SM_TEXT_DOC_DOCSECTION
#define _SM_TEXT_DOC_DOCSECTION
#include "Media/PaperSize.h"
#include "Text/Doc/DocItem.h"

namespace Text
{
	namespace Doc
	{
		class DocSection : public Data::List<DocItem*>
		{
		private:
			Data::ArrayList<DocItem*> *items;
			Media::PaperSize::PaperType paperType;
			Bool isLandscape;
		public:
			DocSection(Media::PaperSize::PaperType paperType, Bool isLandscape);
			virtual ~DocSection();

			Media::PaperSize::PaperType GetPaperType();
			Bool IsLandscape();

			virtual UOSInt Add(DocItem *item);
			virtual UOSInt GetCount();
			virtual DocItem *GetItem(UOSInt index);
		};
	}
}
#endif
