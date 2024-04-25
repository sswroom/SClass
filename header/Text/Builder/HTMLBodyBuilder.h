#ifndef _SM_TEXT_BUILDER_HTMLBODYBUILDER
#define _SM_TEXT_BUILDER_HTMLBODYBUILDER
#include "Data/ArrayList.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextCommon.h"
#include "Text/Builder/HTMLBuilder.h"

namespace Text
{
	namespace Builder
	{
		class HTMLBodyBuilder : public HTMLBuilder
		{
		private:
			Data::ArrayList<Text::CString> elements;
		public:
			HTMLBodyBuilder(NN<Text::StringBuilderUTF8> sb);
			virtual ~HTMLBodyBuilder();

			void BeginLink(Text::CString url);
			void BeginHeading1();
			void BeginHeading2();
			void BeginHeading3();
			void BeginHeading4();
			void BeginTable();
			void BeginTableHeader();
			void BeginTableData();
			void BeginTableRow();
			void BeginTableRowPixelHeight(UOSInt pxHeight);
			void EndElement();
			void AddTableHeader(Text::CString content);
			void AddTableData(Text::CString content);
			void AddTableData(Text::CString content, UOSInt colSpan, UOSInt rowSpan, HAlignment halign, VAlignment valign);
			void AddInputText(Text::CString id, Text::CString name, Text::CString value);
			void AddInputButton(Text::CString id, Text::CString label, Text::CString onClick);

			void WriteHeading1(Text::CString content);
			void WriteHeading2(Text::CString content);
			void WriteHeading3(Text::CString content);
			void WriteHeading4(Text::CString content);
		};
	}
}
#endif
