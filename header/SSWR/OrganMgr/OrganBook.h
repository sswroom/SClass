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
			Data::Timestamp publishDate;
			Int32 groupId;
			Text::String *url;

		public:
			OrganBook();
			~OrganBook();

			Int32 GetBookId();
			void SetBookId(Int32 bookId);
			Text::String *GetTitle();
			void SetTitle(Text::CString title);
			void SetTitle(Text::StringBase<UTF8Char> *title);
			Text::String *GetDispAuthor();
			void SetDispAuthor(Text::CString dispAuthor);
			void SetDispAuthor(Text::StringBase<UTF8Char> *dispAuthor);
			Text::String *GetPress();
			void SetPress(Text::CString press);
			void SetPress(Text::StringBase<UTF8Char> *press);
			Data::Timestamp GetPublishDate();
			void SetPublishDate(const Data::Timestamp &publishDate);
			Int32 GetGroupId();
			void SetGroupId(Int32 groupId);
			Text::String *GetURL();
			void SetURL(Text::CString url);
			void SetURL(Text::StringBase<UTF8Char> *url);

		//	WChar *GetString(WChar *sbuff);
			void GetString(NotNullPtr<Text::StringBuilderUTF8> sb);
		};
	}
}
#endif