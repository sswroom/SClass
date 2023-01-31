#ifndef _SM_TEXT_BUILDER_HTMLBODYBUILDER
#define _SM_TEXT_BUILDER_HTMLBODYBUILDER
#include "Data/ArrayList.h"
#include "Text/StringBuilderUTF8.h"
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
			HTMLBodyBuilder(Text::StringBuilderUTF8 *sb);
			virtual ~HTMLBodyBuilder();

			void BeginLink(Text::CString url);
			void BeginHeading1();
			void BeginHeading2();
			void BeginHeading3();
			void BeginHeading4();
			void EndElement();

			void WriteHeading1(Text::CString content);
		};
	}
}
#endif
