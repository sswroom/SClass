#ifndef _SM_NET_EMAIL_EMAILTEMPLATE
#define _SM_NET_EMAIL_EMAILTEMPLATE
#include "Data/StringUTF8Map.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	namespace Email
	{
		class EmailTemplate
		{
		private:
			const UTF8Char *itemTemplate;
			UOSInt itemOfst;
			Text::StringBuilderUTF8 *sbSubj;
			Text::StringBuilderUTF8 *sbPre;
			Text::StringBuilderUTF8 *sbItem;
			Text::StringBuilderUTF8 *sbPost;
			Bool error;

			Bool ParseTemplate(const UTF8Char *tpl, Text::StringBuilderUTF *sb, Data::StringUTF8Map<const UTF8Char *> *vars);
		public:
			EmailTemplate(const UTF8Char *tpl, Data::StringUTF8Map<const UTF8Char *> *vars);
			~EmailTemplate();

			Bool IsError();
			
			Bool AddItem(Data::StringUTF8Map<const UTF8Char *> *itemVars);
			const UTF8Char *GetSubject();
			void GetContent(Text::StringBuilderUTF *sb);
		};
	}
}
#endif
