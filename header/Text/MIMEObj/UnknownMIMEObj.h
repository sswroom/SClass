#ifndef _SM_TEXT_MIMEOBJ_UNKNOWNMIMEOBJ
#define _SM_TEXT_MIMEOBJ_UNKNOWNMIMEOBJ
#include "Text/IMIMEObj.h"

namespace Text
{
	namespace MIMEObj
	{
		class UnknownMIMEObj : public Text::IMIMEObj
		{
		private:
			UInt8 *dataBuff;
			UOSInt buffSize;
			NN<Text::String> contType;

		public:
			UnknownMIMEObj(UInt8 *dataBuff, UOSInt buffSize, Text::CStringNN contentType);
			virtual ~UnknownMIMEObj();

			virtual Text::CStringNN GetClassName() const;
			virtual Text::CStringNN GetContentType() const;
			virtual UOSInt WriteStream(IO::Stream *stm) const;
			virtual IMIMEObj *Clone() const;

			const UInt8 *GetRAWData(UOSInt *dataSize) const;
		};
	}
}
#endif
