#ifndef _SM_SSWR_ORGANMGR_ORGANBOOK
#define _SM_SSWR_ORGANMGR_ORGANBOOK

#include "Data/DateTime.h"
#include "Text/StringBuilderUTF8.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganBook
		{
		private:
			Int32 id;
			const UTF8Char *title;
			const UTF8Char *dispAuthor;
			const UTF8Char *press;
			Data::DateTime *publishDate;
			Int32 groupId;
			const UTF8Char *url;

		public:
			OrganBook();
			~OrganBook();

			Int32 GetBookId();
			void SetBookId(Int32 bookId);
			const UTF8Char *GetTitle();
			void SetTitle(const UTF8Char *title);
			const UTF8Char *GetDispAuthor();
			void SetDispAuthor(const UTF8Char *dispAuthor);
			const UTF8Char *GetPress();
			void SetPress(const UTF8Char *press);
			Data::DateTime *GetPublishDate();
			void SetPublishDate(Data::DateTime *publishDate);
			Int32 GetGroupId();
			void SetGroupId(Int32 groupId);
			const UTF8Char *GetURL();
			void SetURL(const UTF8Char *url);

		//	WChar *GetString(WChar *sbuff);
			void GetString(Text::StringBuilderUTF8 *sb);
		};
	}
}
#endif