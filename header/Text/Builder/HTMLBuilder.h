#ifndef _SM_TEXT_BUILDER_HTMLBUILDER
#define _SM_TEXT_BUILDER_HTMLBUILDER
#include "Text/StringBuilderUTF8.h"
#include "Text/StyledTextWriter.h"

namespace Text
{
	namespace Builder
	{
		class HTMLBuilder : public Text::StyledTextWriter
		{
		protected:
			NotNullPtr<Text::StringBuilderUTF8> sb;
			Bool bodyContent;
		private:
			Bool fontStarted;

		public:
			HTMLBuilder(NotNullPtr<Text::StringBuilderUTF8> sb, Bool bodyContent);
			virtual ~HTMLBuilder();

			virtual Bool WriteChar(UTF8Char c);
			virtual void SetTextColor(StandardColor fgColor);
			virtual void ResetTextColor();
		};
	}
}
#endif
