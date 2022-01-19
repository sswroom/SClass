#ifndef _SM_TEXT_MIMEOBJ_HTMLMIMEOBJ
#define _SM_TEXT_MIMEOBJ_HTMLMIMEOBJ
#include "Text/IMIMEObj.h"
#include "Text/StringBuilderUTF.h"

namespace Text
{
	namespace MIMEObj
	{
		class HTMLMIMEObj : public Text::IMIMEObj
		{
		private:
			UInt8 *textBuff;
			OSInt buffSize;
			Int32 codePage;
			Text::String *contType;

			void BuildContentType();
		public:
			HTMLMIMEObj(UInt8 *textBuff, OSInt buffSize, Int32 codePage);
			virtual ~HTMLMIMEObj();

			virtual const UTF8Char *GetClassName();
			virtual Text::CString GetContentType();
			virtual UOSInt WriteStream(IO::Stream *stm);
			virtual IMIMEObj *Clone();

			void GetText(Text::StringBuilderUTF *sb);
			Int32 GetCodePage();
		};
	}
}
#endif
