#ifndef _SM_TEXT_BUILDER_HTMLDOCUMENTBUILDER
#define _SM_TEXT_BUILDER_HTMLDOCUMENTBUILDER
#include "Text/StringBuilderUTF8.h"
#include "Text/Builder/HTMLBodyBuilder.h"
#include "Text/Builder/HTMLHeaderBuilder.h"

namespace Text
{
	namespace Builder
	{
		class HTMLDocumentBuilder
		{
		public:
			enum class DocType
			{
				Unknown,
				HTML4_01,
				XHTML1_1,
				HTML5
			};
		private:
			Text::StringBuilderUTF8 sb;
			HTMLHeaderBuilder *headerBuilder;
			HTMLBodyBuilder *bodyBuilder;

		public:
			HTMLDocumentBuilder(DocType docType, Text::CString title);
			~HTMLDocumentBuilder();

			HTMLBodyBuilder *StartBody(Text::CString onLoadScript);

			Text::CStringNN Build();

			static Text::CString DocTypeGetText(DocType docType);
		};
	}
}
#endif
