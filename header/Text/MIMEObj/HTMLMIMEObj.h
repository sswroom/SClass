#ifndef _SM_TEXT_MIMEOBJ_HTMLMIMEOBJ
#define _SM_TEXT_MIMEOBJ_HTMLMIMEOBJ
#include "Text/IMIMEObj.h"
#include "Text/StringBuilderUTF8.h"

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

			virtual Text::CString GetClassName() const;
			virtual Text::CString GetContentType() const;
			virtual UOSInt WriteStream(IO::Stream *stm) const;
			virtual IMIMEObj *Clone() const;

			void GetText(Text::StringBuilderUTF8 *sb) const;
			Int32 GetCodePage() const;
		};
	}
}
#endif
