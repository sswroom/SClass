#ifndef _SM_TEXT_MIMEOBJ_TEXTMIMEOBJ
#define _SM_TEXT_MIMEOBJ_TEXTMIMEOBJ
#include "Text/IMIMEObj.h"
#include "Text/StringBuilderUTF.h"

namespace Text
{
	namespace MIMEObj
	{
		class TextMIMEObj : public Text::IMIMEObj
		{
		private:
			UInt8 *textBuff;
			OSInt buffSize;
			Int32 codePage;
			const UTF8Char *contType;

			void BuildContentType();
		public:
			TextMIMEObj(UInt8 *textBuff, OSInt buffSize, Int32 codePage);
			TextMIMEObj(const WChar *txt, Int32 codePage);
			virtual ~TextMIMEObj();

			virtual const UTF8Char *GetClassName();
			virtual const UTF8Char *GetContentType();
			virtual OSInt WriteStream(IO::Stream *stm);
			virtual IMIMEObj *Clone();

			void GetText(Text::StringBuilderUTF *sb);
			Int32 GetCodePage();
		};
	}
}
#endif
