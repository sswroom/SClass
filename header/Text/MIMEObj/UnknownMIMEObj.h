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
			OSInt buffSize;
			const UTF8Char *contType;

		public:
			UnknownMIMEObj(UInt8 *dataBuff, OSInt buffSize, const UTF8Char *contentType);
			virtual ~UnknownMIMEObj();

			virtual const UTF8Char *GetClassName();
			virtual const UTF8Char *GetContentType();
			virtual OSInt WriteStream(IO::Stream *stm);
			virtual IMIMEObj *Clone();

			const UInt8 *GetRAWData(OSInt *dataSize);
		};
	}
}
#endif
