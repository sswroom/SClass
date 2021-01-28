#ifndef _SM_TEXT_MIMEOBJ_MIMEHEADER
#define _SM_TEXT_MIMEOBJ_MIMEHEADER
#include "Data/ArrayList.h"

namespace Text
{
	namespace MIMEObj
	{
		class MIMEHeader
		{
		protected:
			Data::ArrayList<const UTF8Char *> *headerName;
			Data::ArrayList<const UTF8Char *> *headerValue;

		public:
			MIMEHeader();
			virtual ~MIMEHeader();

			void AddHeader(const UTF8Char *name, const UTF8Char *value);
			const UTF8Char *GetHeader(const UTF8Char *name);
			OSInt GetHeaderCount();
			const UTF8Char *GetHeaderName(OSInt index);
			const UTF8Char *GetHeaderValue(OSInt index);

			static UTF8Char *ParseHeaderStr(UTF8Char *sbuff, const UTF8Char *value);
		};
	}
}
#endif
