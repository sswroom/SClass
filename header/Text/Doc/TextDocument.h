#ifndef _SM_TEXT_DOC_TEXTDOCUMENT
#define _SM_TEXT_DOC_TEXTDOCUMENT
#include "Data/ArrayList.h"
#include "IO/ParsedObject.h"
#include "Media/PrintDocument.h"
#include "Text/Doc/DocSection.h"

namespace Text
{
	namespace Doc
	{
		class TextDocument : public IO::ParsedObject, public Data::ReadingListNN<DocSection>, public Media::PrintHandler
		{
		private:
			typedef enum
			{
				PF_TEXTCOLOR = 1,
				PF_BGCOLOR = 2,
				PF_LINKCOLOR = 4,
				PF_ACTIVELINKCOLOR = 8,
				PF_VISITEDLINKCOLOR = 16
			} PropertiesFlags;

			typedef struct
			{
				OSInt currSection;
			} PrintStatus;

		private:
			NN<Text::String> docName;
			PropertiesFlags pflags;
			UInt32 textColor;
			UInt32 bgColor;
			UInt32 linkColor;
			UInt32 activeLinkColor;
			UInt32 visitedLinkColor;

			PrintStatus pStatus;
			Data::ArrayListNN<DocSection> items;
		public:
			TextDocument();
			TextDocument(Text::CStringNN name);
			virtual ~TextDocument();

			virtual IO::ParserType GetParserType() const;

			void SetDocumentName(Text::CStringNN docName);
			UnsafeArray<UTF8Char> GetDocumentName(UnsafeArray<UTF8Char> docName) const;
			void SetTextColor(UInt32 textColor);
			Bool GetTextColor(UInt32 *textColor) const;
			void SetBGColor(UInt32 bgColor);
			Bool GetBGColor(UInt32 *bgColor) const;
			void SetLinkColor(UInt32 linkColor);
			Bool GetLinkColor(UInt32 *linkColor) const;
			void SetActiveLinkColor(UInt32 activeLinkColor);
			Bool GetActiveLinkColor(UInt32 *activeLinkColor) const;
			void SetVisitedLinkColor(UInt32 visitedLinkColor);
			Bool GetVisitedLinkColor(UInt32 *visitedLinkColor) const;

			virtual UOSInt Add(NN<DocSection> section);
			virtual UOSInt GetCount() const;
			virtual NN<DocSection> GetItemNoCheck(UOSInt Index) const;
			virtual Optional<DocSection> GetItem(UOSInt Index) const;

			virtual Bool BeginPrint(NN<Media::PrintDocument> doc);
			virtual Bool PrintPage(NN<Media::DrawImage> printPage); 
			virtual Bool EndPrint(NN<Media::PrintDocument> doc);
		};
	}
}
#endif
