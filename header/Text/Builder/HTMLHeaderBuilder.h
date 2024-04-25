#ifndef _SM_TEXT_BUILDER_HTMLHEADERBUILDER
#define _SM_TEXT_BUILDER_HTMLHEADERBUILDER
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	namespace Builder
	{
		class HTMLHeaderBuilder
		{
		private:
			NN<Text::StringBuilderUTF8> sb;

		public:
			HTMLHeaderBuilder(NN<Text::StringBuilderUTF8> sb, Text::CString title);
			~HTMLHeaderBuilder();
		};
	}
}
#endif
