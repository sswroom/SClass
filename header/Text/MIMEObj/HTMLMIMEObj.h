#ifndef _SM_TEXT_MIMEOBJ_HTMLMIMEOBJ
#define _SM_TEXT_MIMEOBJ_HTMLMIMEOBJ
#include "Text/MIMEObject.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	namespace MIMEObj
	{
		class HTMLMIMEObj : public Text::MIMEObject
		{
		private:
			UInt8 *textBuff;
			UIntOS buffSize;
			UInt32 codePage;
			Text::String *contType;

			void BuildContentType();
		public:
			HTMLMIMEObj(UnsafeArray<const UInt8> textBuff, UIntOS buffSize, UInt32 codePage);
			virtual ~HTMLMIMEObj();

			virtual Text::CStringNN GetClassName() const;
			virtual Text::CStringNN GetContentType() const;
			virtual UIntOS WriteStream(NN<IO::Stream> stm) const;
			virtual NN<MIMEObject> Clone() const;

			void GetText(NN<Text::StringBuilderUTF8> sb) const;
			UInt32 GetCodePage() const;
		};
	}
}
#endif
