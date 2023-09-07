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
			TextMIMEObj(const UInt8 *textBuff, UOSInt buffSize, UInt32 codePage);
			TextMIMEObj(const WChar *txt, UInt32 codePage);
			virtual ~TextMIMEObj();

			virtual Text::CStringNN GetClassName() const;
			virtual Text::CStringNN GetContentType() const;
			virtual UOSInt WriteStream(IO::Stream *stm) const;
			virtual IMIMEObj *Clone() const;

			void GetText(NotNullPtr<Text::StringBuilderUTF8> sb) const;
			UInt32 GetCodePage() const;
		};
	}
}
#endif
