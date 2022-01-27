#ifndef _SM_SSWR_ORGANMGR_ORGANBOOK
#define _SM_SSWR_ORGANMGR_ORGANBOOK

#include "Data/DateTime.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganBook
		{
		private:
			Int32 id;
			Text::String *title;
			Text::String *dispAuthor;
			Text::String *press;
			Data::DateTime *publishDate;
			Int32 groupId;
			Text::String *url;

		public:
			OrganBook();
			~OrganBook();

			Int32 GetBookId();
			void SetBookId(Int32 bookId);
			Text::String *GetTitle();
			void SetTitle(const UTF8Char *title);
			void SetTitle(Text::StringBase<UTF8Char> *title);
			Text::String *GetDispAuthor();
			void SetDispAuthor(const UTF8Char *dispAuthor);
			void SetDispAuthor(Text::StringBase<UTF8Char> *dispAuthor);
			Text::String *GetPress();
			void SetPress(const UTF8Char *press);
			void SetPress(Text::StringBase<UTF8Char> *press);
			Data::DateTime *GetPublishDate();
			void SetPublishDate(Data::DateTime *publishDate);
			Int32 GetGroupId();
			void SetGroupId(Int32 groupId);
			Text::String *GetURL();
			void SetURL(const UTF8Char *url);
			void SetURL(Text::StringBase<UTF8Char> *url);

		//	WChar *GetString(WChar *sbuff);
			void GetString(Text::StringBuilderUTF8 *sb);
		};
	}
}
#endif