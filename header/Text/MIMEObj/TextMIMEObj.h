#ifndef _SM_TEXT_MIMEOBJ_TEXTMIMEOBJ
#define _SM_TEXT_MIMEOBJ_TEXTMIMEOBJ
#include "Text/IMIMEObj.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	namespace MIMEObj
	{
		class TextMIMEObj : public Text::IMIMEObj
		{
		private:
			UInt8 *textBuff;
			UOSInt buffSize;
			UInt32 codePage;
			Text::String *contType;

			void BuildContentType();
		public:
			TextMIMEObj(UInt8 *textBuff, UOSInt buffSize, UInt32 codePage);
			TextMIMEObj(const WChar *txt, UInt32 codePage);
			virtual ~TextMIMEObj();

			virtual Text::CString GetClassName();
			virtual Text::CString GetContentType();
			virtual UOSInt WriteStream(IO::Stream *stm);
			virtual IMIMEObj *Clone();

			void GetText(Text::StringBuilderUTF8 *sb);
			UInt32 GetCodePage();
		};
	}
}
#endif
