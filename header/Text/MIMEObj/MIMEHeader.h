#ifndef _SM_TEXT_MIMEOBJ_MIMEHEADER
#define _SM_TEXT_MIMEOBJ_MIMEHEADER
#include "Data/ArrayList.h"
#include "Text/String.h"

namespace Text
{
	namespace MIMEObj
	{
		class MIMEHeader
		{
		protected:
			Data::ArrayList<Text::String *> *headerName;
			Data::ArrayList<Text::String *> *headerValue;

		public:
			MIMEHeader();
			virtual ~MIMEHeader();

			void AddHeader(const UTF8Char *name, UOSInt nameLen, const UTF8Char *value, UOSInt valueLen);
			void AddHeader(Text::String *name, Text::String *value);
			Text::String *GetHeader(const UTF8Char *name, UOSInt nameLen);
			UOSInt GetHeaderCount();
			Text::String *GetHeaderName(UOSInt index);
			Text::String *GetHeaderValue(UOSInt index);

			static UTF8Char *ParseHeaderStr(UTF8Char *sbuff, const UTF8Char *value);
		};
	}
}
#endif
