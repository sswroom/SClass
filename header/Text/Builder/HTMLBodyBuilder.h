#ifndef _SM_TEXT_BUILDER_HTMLBODYBUILDER
#define _SM_TEXT_BUILDER_HTMLBODYBUILDER
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	namespace Builder
	{
		class HTMLBodyBuilder
		{
		private:
			Text::StringBuilderUTF8 *sb;

		public:
			HTMLBodyBuilder(Text::StringBuilderUTF8 *sb);
			~HTMLBodyBuilder();
		};
	}
}
#endif
