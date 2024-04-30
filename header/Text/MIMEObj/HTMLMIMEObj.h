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
			UOSInt buffSize;
			UInt32 codePage;
			Text::String *contType;

			void BuildContentType();
		public:
			HTMLMIMEObj(const UInt8 *textBuff, UOSInt buffSize, UInt32 codePage);
			virtual ~HTMLMIMEObj();

			virtual Text::CStringNN GetClassName() const;
			virtual Text::CStringNN GetContentType() const;
			virtual UOSInt WriteStream(NN<IO::Stream> stm) const;
			virtual NN<IMIMEObj> Clone() const;

			void GetText(NN<Text::StringBuilderUTF8> sb) const;
			UInt32 GetCodePage() const;
		};
	}
}
#endif
