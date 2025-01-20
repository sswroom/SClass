#ifndef _SM_TEXT_MIMEOBJ_TEXTMIMEOBJ
#define _SM_TEXT_MIMEOBJ_TEXTMIMEOBJ
#include "Text/MIMEObject.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	namespace MIMEObj
	{
		class TextMIMEObj : public Text::MIMEObject
		{
		private:
			UInt8 *textBuff;
			UOSInt buffSize;
			UInt32 codePage;
			Text::String *contType;

			void BuildContentType();
		public:
			TextMIMEObj(UnsafeArray<const UInt8> textBuff, UOSInt buffSize, UInt32 codePage);
			TextMIMEObj(const WChar *txt, UInt32 codePage);
			virtual ~TextMIMEObj();

			virtual Text::CStringNN GetClassName() const;
			virtual Text::CStringNN GetContentType() const;
			virtual UOSInt WriteStream(NN<IO::Stream> stm) const;
			virtual NN<MIMEObject> Clone() const;

			void GetText(NN<Text::StringBuilderUTF8> sb) const;
			UInt32 GetCodePage() const;
		};
	}
}
#endif
