#ifndef _SM_TEXT_BUILDER_HTMLBODYBUILDER
#define _SM_TEXT_BUILDER_HTMLBODYBUILDER
#include "Data/ArrayListObj.hpp"
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
			Data::ArrayListObj<Text::CString> elements;
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
			void BeginTableRowPixelHeight(UIntOS pxHeight);
			void EndElement();
			void AddTableHeader(Text::CStringNN content);
			void AddTableData(Text::CStringNN content);
			void AddTableData(Text::CStringNN content, UIntOS colSpan, UIntOS rowSpan, HAlignment halign, VAlignment valign);
			void AddInputText(Text::CString id, Text::CString name, Text::CString value);
			void AddInputButton(Text::CString id, Text::CString label, Text::CString onClick);

			void WriteHeading1(Text::CStringNN content);
			void WriteHeading2(Text::CStringNN content);
			void WriteHeading3(Text::CStringNN content);
			void WriteHeading4(Text::CStringNN content);
		};
	}
}
#endif
