#ifndef _SM_TEXT_IMIMEOBJ
#define _SM_TEXT_IMIMEOBJ
#include "IO/IStreamData.h"
#include "IO/ParsedObject.h"
#include "IO/Stream.h"

namespace Text
{
	class IMIMEObj : public IO::ParsedObject
	{
	protected:
		IMIMEObj(const UTF8Char *sourceName);
	public:
		virtual ~IMIMEObj();

		virtual const UTF8Char *GetClassName() = 0;
		virtual const UTF8Char *GetContentType() = 0;
		virtual UOSInt WriteStream(IO::Stream *stm) = 0;
		virtual IMIMEObj *Clone() = 0;

		virtual IO::ParserType GetParserType();

		static Text::IMIMEObj *ParseFromData(IO::IStreamData *data, const UTF8Char *contentType);
		static Text::IMIMEObj *ParseFromFile(const UTF8Char *fileName);
	};
}
#endif
