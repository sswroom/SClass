#ifndef _SM_TEXT_DOC_DOCSECTION
#define _SM_TEXT_DOC_DOCSECTION
#include "Data/ArrayListNN.hpp"
#include "Media/PaperSize.h"
#include "Text/Doc/DocItem.h"

namespace Text
{
	namespace Doc
	{
		class DocSection : public Data::ReadingListNN<DocItem>
		{
		private:
			Data::ArrayListNN<DocItem> items;
			Media::PaperSize::PaperType paperType;
			Bool isLandscape;
		public:
			DocSection(Media::PaperSize::PaperType paperType, Bool isLandscape);
			virtual ~DocSection();

			Media::PaperSize::PaperType GetPaperType();
			Bool IsLandscape();

			virtual UOSInt Add(NN<DocItem> item);
			virtual UOSInt GetCount() const;
			virtual NN<DocItem> GetItemNoCheck(UOSInt index) const;
			virtual Optional<DocItem> GetItem(UOSInt index) const;
		};
	}
}
#endif
